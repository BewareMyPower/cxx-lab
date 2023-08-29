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
    char buf[1024];
    auto len = socket.read_some(boost::asio::buffer(buf, sizeof(buf)), ec);
    if (ec) {
      if (ec == boost::asio::error::eof) {
        std::cout << ec.message() << std::endl;
        break;
      } else {
        throw std::runtime_error("Failed to read " + ec.message());
      }
    }
    buf[len] = '\0';
    std::cout << buf;
    std::cout.flush();
  }
  // TODO: it might cause segfault.
  // See https://github.com/chriskohlhoff/asio/issues/1347
  socket.close(ec);
  acceptor.close(ec);

  io.stop();
  event_loop.join();
  return 0;
}
