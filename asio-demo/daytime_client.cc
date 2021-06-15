#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::tcp;

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <host>" << std::endl;
    return 1;
  }

  boost::asio::io_context io_context;

  tcp::resolver resolver(io_context);
  const auto endpoints = resolver.resolve(argv[1], "daytime");

  tcp::socket socket(io_context);
  boost::asio::connect(socket, endpoints);

  for (;;) {
    boost::array<char, 128> buf;
    boost::system::error_code error;

    const auto len = socket.read_some(boost::asio::buffer(buf), error);

    if (error == boost::asio::error::eof) {
      break;  // Connection closed cleanly by peer
    } else if (error) {
      throw boost::system::system_error(error);
    }

    std::cout.write(buf.data(), len);
  }
  return 0;
}
