#include <boost/asio.hpp>
#include <ctime>
#include <string>

using boost::asio::ip::tcp;

inline std::string make_daytime_string() {
  auto now = std::time(nullptr);
  return std::ctime(&now);
}

int main(int argc, char* argv[]) {
  boost::asio::io_context io_context;
  tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 13));

  for (;;) {
    tcp::socket socket(io_context);
    acceptor.accept(socket);

    const auto message = make_daytime_string();

    boost::system::error_code ignored_error;
    boost::asio::write(socket, boost::asio::buffer(message), ignored_error);
  }
  return 0;
}
