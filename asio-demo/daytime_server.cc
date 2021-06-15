#include <boost/asio.hpp>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>

using boost::asio::ip::tcp;

inline std::string make_daytime_string() {
  auto now = std::time(nullptr);
  return std::ctime(&now);
}

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
 public:
  static auto create(boost::asio::io_context& io_context) {
    return std::shared_ptr<TcpConnection>(new TcpConnection(io_context));
  }

  auto& socket() { return socket_; }

  void start() {
    message_ = make_daytime_string();

    auto self = shared_from_this();
    boost::asio::async_write(
        socket_, boost::asio::buffer(message_),
        [this, self](const boost::system::error_code& error,
                     std::size_t bytes_transferred) {
          if (error) {
            std::cerr << "[ERROR] Failed to write: " << error << std::endl;
          } else {
            std::cout << "[DEBUG] write " << bytes_transferred
                      << " bytes to client" << std::endl;
          }
        });
  }

 private:
  TcpConnection(boost::asio::io_context& io_context) : socket_(io_context) {}

  tcp::socket socket_;
  std::string message_;
};

class TcpServer {
 public:
  TcpServer(boost::asio::io_context& io_context)
      : io_context_(io_context),
        acceptor_(io_context_, tcp::endpoint(tcp::v4(), 13)) {
    startAccept();
  }

 private:
  boost::asio::io_context& io_context_;
  tcp::acceptor acceptor_;

  void startAccept() {
    const auto new_connection = TcpConnection::create(io_context_);

    acceptor_.async_accept(
        new_connection->socket(),
        [this, new_connection](const boost::system::error_code& error) {
          if (!error) {
            new_connection->start();
          }
          startAccept();
        });
  }
};

int main(int argc, char* argv[]) {
  boost::asio::io_context io_context;
  TcpServer server(io_context);
  io_context.run();
  return 0;
}
