#include <boost/asio.hpp>
#include <memory>

class TcpServer;

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
  friend class TcpServer;

 public:
  TcpConnection(boost::asio::io_context& io_context) : socket_(io_context) {}

  virtual void run() = 0;

 protected:
  boost::asio::ip::tcp::socket socket_;
};
