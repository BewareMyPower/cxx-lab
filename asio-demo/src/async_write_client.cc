#include <atomic>
#include <boost/asio.hpp>
#include <chrono>
#include <deque>
#include <mutex>
#include <string>
#include <thread>

#include "simple_logger.h"

using boost::asio::ip::tcp;

struct Message {
  char* data;
  size_t size;

  Message(size_t n, char ch) : size(n), data(new char[n]) {
    memset(data, ch, n);
    data[n - 1] = '\n';
  }

  ~Message() {
    if (data) {
      delete[] data;
    }
  }

  Message(const Message&) = delete;
  Message& operator=(const Message&) = delete;

  Message(Message&& rhs) noexcept : data(rhs.data), size(rhs.size) {
    rhs.data = nullptr;
  }
  Message& operator=(Message&& rhs) noexcept {
    if (this == &rhs) {
      return *this;
    }
    if (data) {
      delete[] data;
    }
    data = rhs.data;
    size = rhs.size;
    rhs.data = nullptr;
    return *this;
  }
};

using SendCallback =
    std::function<void(const boost::system::error_code&, size_t)>;

class Connection : public std::enable_shared_from_this<Connection> {
 public:
  Connection(tcp::socket& socket) : socket_(socket) {}

  void asyncWrite(Message&& msg, SendCallback&& callback, bool cache) {
    boost::asio::const_buffer buffer{msg.data, msg.size};
    if (cache) {
      std::lock_guard<std::mutex> lock{mutex_};
      pending_messages_.emplace_back(std::move(msg));
    }
    bool expected = false;
    if (has_pending_message_.compare_exchange_strong(expected, true)) {
      LOG_INFO("Send ", msg.size, "bytes");
    } else {
      return;
    }

    auto self = shared_from_this();
    boost::asio::async_write(
        socket_, buffer, boost::asio::transfer_all(),
        [this, self, callback{std::move(callback)}](
            const boost::system::error_code& ec, size_t n) mutable {
          callback(ec, n);
          if (ec) {
            return;
          }
          std::unique_lock<std::mutex> lock{mutex_};
          if (pending_messages_.empty()) {
            lock.unlock();
            LOG_INFO("All messages are sent");
            return;
          }
          pending_messages_.pop_front();
          has_pending_message_ = false;
          if (pending_messages_.empty()) {
            return;
          }
          auto msg = std::move(pending_messages_.front());
          asyncWrite(std::move(msg), std::move(callback), false);
        });
  }

 private:
  tcp::socket& socket_;
  std::deque<Message> pending_messages_;
  std::atomic_bool has_pending_message_{false};
  std::mutex mutex_;
};

int main(int argc, char* argv[]) {
  boost::asio::io_context io;
  std::thread event_loop{[&io] {
    boost::asio::io_context::work work{io};
    boost::system::error_code ec;
    io.run(ec);
    LOG_INFO("Event loop exits with", ec.message());
  }};

  tcp::resolver resolver{io};
  tcp::resolver::query query{"localhost", "8888"};
  tcp::socket socket{io};
  for (auto it = resolver.resolve(query); it != tcp::resolver::iterator{};
       ++it) {
    boost::system::error_code ec;
    socket.connect(*it, ec);
    if (!ec) {
      LOG_INFO("Connected to ", it->endpoint());
      break;
    }
    LOG_WARN("Failed to connect", it->endpoint(), ec.message());
    socket.close(ec);
  }

  std::atomic_int num_sent{0};
  constexpr int kNumLines = 10;

  auto cnx = std::make_shared<Connection>(socket);
  for (int i = 0; i < kNumLines; i++) {
    auto msg = (i % 2 == 0) ? Message{1048576, 'a'} : Message{100, 'b'};
    msg.data[0] = ('0' + i % 10);
    cnx->asyncWrite(
        std::move(msg),
        [i, &num_sent](const boost::system::error_code& ec,
                       size_t bytes_transferred) {
          LOG_INFO(i, ec.message(), bytes_transferred);
          num_sent++;
        },
        true);
  }
  while (num_sent < kNumLines) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  io.stop();
  event_loop.join();
  return 0;
}
