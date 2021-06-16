#include <boost/asio.hpp>
#include <ctime>
#include <memory>
#include <string>

#include "async_write_message.h"
#include "simple_logger.h"
#include "string_helper.h"

using boost::asio::ip::tcp;
using string_helper::join;

inline std::string make_daytime_string() {
  auto now = std::time(nullptr);
  return std::ctime(&now);
}

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
 public:
  static auto create(boost::asio::io_context& io_context, int id) {
    return std::shared_ptr<TcpConnection>(new TcpConnection(io_context, id));
  }

  auto& socket() { return socket_; }

  void start() {
    message_ = make_daytime_string();
    asyncWrite(socket_, message_.c_str(),
               [this](const boost::system::error_code& error, std::size_t n) {
                 LOG_DEBUG("connection", join("[", id_, "]"),
                           "async_write with callback");
                 if (!error) {
                   LOG_DEBUG(n, "bytes transferred");
                 } else {
                   LOG_ERROR(error.message());
                 }
               });
  }

 private:
  TcpConnection(boost::asio::io_context& io_context, int id)
      : socket_(io_context), id_(id) {}

  tcp::socket socket_;
  const int id_;
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
    static std::atomic_int connection_id{0};
    const auto new_connection =
        TcpConnection::create(io_context_, connection_id++);

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
