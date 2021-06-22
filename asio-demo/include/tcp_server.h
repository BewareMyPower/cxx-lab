#pragma once

#include "tcp_connection.h"

class TcpServer {
 public:
  TcpServer(boost::asio::io_context& io_context, unsigned short port)
      : io_context_(io_context), acceptor_(io_context_, {tcp::v4(), port}) {}

  virtual std::shared_ptr<TcpConnection> newConnection() = 0;

  void startAccept() {
    const auto new_connection = newConnection();
    acceptor_.async_accept(
        new_connection->socket_,
        [this, new_connection](const boost::system::error_code& error) {
          if (!error) {
            new_connection->run();
          }
          // accept connections one by one
          startAccept();
        });
  }

 protected:
  boost::asio::io_context& io_context_;
  boost::asio::ip::tcp::acceptor acceptor_;
};
