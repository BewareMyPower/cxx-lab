#include <array>
#include <boost/asio.hpp>
#include <iostream>
#include <stdexcept>

using boost::asio::ip::tcp;

int main(int argc, char* argv[]) {
  boost::asio::io_context io;
  std::thread event_loop{[&io] {
    boost::asio::io_context::work work{io};
    boost::system::error_code ec;
    io.run(ec);
  }};

  tcp::acceptor acceptor{io, tcp::endpoint(tcp::v4(), 8888)};
  tcp::socket socket{io};
  acceptor.accept(socket);

  boost::system::error_code ec;
  while (true) {
    std::array<char, 1024> buf;
    // Don't fill the last position to ensure a '\0' at the tail
    auto len =
        socket.read_some(boost::asio::buffer(buf.data(), buf.size() - 1), ec);
    if (ec) {
      if (ec == boost::asio::error::eof) {
        std::cout << ec.message() << std::endl;
        break;
      } else {
        throw std::runtime_error("Failed to read " + ec.message());
      }
    }
    buf[len] = '\0';
    std::cout << buf.data();
    std::cout.flush();
  }
  socket.close(ec);
  acceptor.close(ec);

  io.stop();
  event_loop.join();
  return 0;
}
