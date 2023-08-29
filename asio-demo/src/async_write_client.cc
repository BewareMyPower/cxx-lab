#include <atomic>
#include <boost/asio.hpp>
#include <chrono>
#include <string>
#include <thread>

#include "simple_logger.h"

using boost::asio::ip::tcp;

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
  std::string messages[2];
  messages[0].resize(1048576, 'a');
  messages[0].back() = '\n';
  messages[1].resize(100, 'b');
  messages[1].back() = '\n';

  constexpr int kNumLines = 10;
  for (int i = 0; i < kNumLines; i++) {
    // TODO: message disorder
    auto& msg = messages[i % 2];
    boost::asio::async_write(socket,
                             boost::asio::const_buffer(msg.data(), msg.size()),
                             [i, &num_sent](const boost::system::error_code& ec,
                                            size_t bytes_transferred) {
                               LOG_INFO(i, ec.message(), bytes_transferred);
                               num_sent++;
                             });
  }
  while (num_sent < kNumLines) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  io.stop();
  event_loop.join();
  return 0;
}
