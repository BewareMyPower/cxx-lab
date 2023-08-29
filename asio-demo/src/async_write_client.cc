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
  for (int i = 0; i < 100; i++) {
    std::string s(1000, 'a' + i % 25);
    s.back() = '\n';
    // TODO: transfer_all, the following logic is wrong
    boost::asio::async_write(socket,
                             boost::asio::const_buffer(s.data(), s.size()),
                             [i, &num_sent](const boost::system::error_code& ec,
                                            size_t bytes_transferred) {
                               LOG_INFO(i, ec.message(), bytes_transferred);
                               num_sent++;
                             });
  }
  while (num_sent < 100) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  io.stop();
  event_loop.join();
  return 0;
}
