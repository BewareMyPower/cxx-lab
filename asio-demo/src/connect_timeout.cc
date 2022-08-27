#include <boost/asio.hpp>
#include <iostream>

#include "simple_logger.h"

using boost::asio::steady_timer;
using boost::asio::ip::tcp;
using ErrorCode = boost::system::error_code;

class Client {
 public:
  Client(boost::asio::io_context& io_context)
      : socket_(io_context), timer_(io_context) {}

  void start(tcp::resolver::results_type endpoints, int period_ms) {
    endpoints_ = endpoints;
    period_ms_ = period_ms;
    if (period_ms_ > 0) {
      timer_.expires_at(steady_timer::time_point::max());
      timer_.async_wait([this](const ErrorCode& ec) { handleWait(); });
    }
    connect(endpoints_.begin());
  }

  void stop() {
    if (stopped_) {
      return;
    }
    stopped_ = true;
    ErrorCode ignored_ec;
    socket_.close(ignored_ec);
    timer_.cancel();
    LOG_DEBUG("Stop completed with socket close error code:",
              ignored_ec.message());
  }

 private:
  bool stopped_{false};
  tcp::socket socket_;
  steady_timer timer_;
  tcp::resolver::results_type endpoints_;
  int period_ms_;

  void handleWait() {
    if (stopped_) {
      return;
    }

    if (timer_.expiry() <= steady_timer::clock_type::now()) {
      LOG_INFO("timer_.expiry() < now(), set expires_at max time");
      socket_.close();
      timer_.expires_at(steady_timer::time_point::max());
    }

    timer_.async_wait([this](const ErrorCode& ec) { handleWait(); });
  }

  void connect(tcp::resolver::results_type::iterator endpoint_iter) {
    if (endpoint_iter == endpoints_.end()) {
      stop();
      return;
    }
    LOG_DEBUG("connect to", endpoint_iter->endpoint());

    if (period_ms_ >= 0) {
      timer_.expires_after(boost::asio::chrono::milliseconds(period_ms_));
    }

    socket_.async_connect(
        endpoint_iter->endpoint(), [this, endpoint_iter](const ErrorCode& ec) {
          if (stopped_) {
            return;
          }

          if (!socket_.is_open()) {
            LOG_WARN("Connect timed out");
            auto next_endpoint_iter = endpoint_iter;
            connect(++next_endpoint_iter);
          } else if (ec) {
            LOG_ERROR("Connect error:", ec.message());
            socket_.close();
            auto next_endpoint_iter = endpoint_iter;
            connect(++next_endpoint_iter);
          } else {
            LOG_INFO("Connected to", endpoint_iter->endpoint());
            // TODO: read some bytes and stop
            stop();
          }
        });
  }
};

int main(int argc, char* argv[]) {
  if (argc < 4) {
    std::cerr << "Usage: " << argv[0] << " <host> <port> <connect-timeout-ms>"
              << std::endl;
    return 1;
  }

  boost::asio::io_context io_context;
  tcp::resolver r(io_context);

  Client client(io_context);
  client.start(r.resolve(argv[1], argv[2]), std::stoi(argv[3]));

  io_context.run();
  client.stop();

  return 0;
}
