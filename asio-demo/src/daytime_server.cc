#include <boost/asio.hpp>
#include <ctime>
#include <memory>
#include <string>

#include "async_write_message.h"
#include "simple_logger.h"
#include "string_helper.h"
#include "tcp_server.h"

using boost::asio::ip::tcp;
using string_helper::join;

class DaytimeConnection : public TcpConnection {
 public:
  DaytimeConnection(boost::asio::io_context& io_context)
      : TcpConnection(io_context) {}

  void run() override {
    const std::string message = makeDaytimeString();
    asyncWrite(socket_, message.c_str(),
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
  static std::atomic_int count;
  const int id_{count++};

  static std::string makeDaytimeString() {
    auto now = std::time(nullptr);
    return std::ctime(&now);
  }
};

std::atomic_int DaytimeConnection::count{0};

class DaytimeServer : public TcpServer {
 public:
  DaytimeServer(boost::asio::io_context& io_context)
      : TcpServer(io_context, 13) {}

  std::shared_ptr<TcpConnection> newConnection() override {
    return std::shared_ptr<DaytimeConnection>{
        new DaytimeConnection(io_context_)};
  }
};

int main(int argc, char* argv[]) {
  boost::asio::io_context io_context;
  DaytimeServer server(io_context);
  server.startAccept();
  io_context.run();
  return 0;
}
