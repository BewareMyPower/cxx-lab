#include <atomic>
#include <boost/asio.hpp>
#include <chrono>
#include <memory>
#include <string>
#include <thread>

#include "simple_logger.h"

class PeriodicalTask : public std::enable_shared_from_this<PeriodicalTask> {
 public:
  static std::shared_ptr<PeriodicalTask> create(
      boost::asio::io_context& io_context, int period_ms) {
    return std::shared_ptr<PeriodicalTask>(
        new PeriodicalTask(io_context, period_ms));
  }

  void run() {
    auto self = shared_from_this();
    LOG_INFO("Start timer with refcnt", self.use_count());
    timer_.async_wait(
        [this, self](const boost::system::error_code& ec) { handle(ec); });
  }

  void cancel() { timer_.cancel(); }

 private:
  std::atomic_int count_{0};
  boost::asio::steady_timer timer_;
  int period_ms_;

  PeriodicalTask(boost::asio::io_context& io_context, int period_ms)
      : timer_(io_context, boost::asio::chrono::milliseconds(period_ms)),
        period_ms_(period_ms) {}

  void handle(const boost::system::error_code& ec) {
    if (ec == boost::asio::error::operation_aborted) {
      LOG_DEBUG("PeriodicalTask is cancelled");
    } else if (ec) {
      LOG_ERROR("PeriodicalTask encountered error", ec.message());
    } else {
      auto self = shared_from_this();
      LOG_INFO("Triggered for", ++count_, "times, refcnt is", self.use_count());
      // We must call expires_after before async_wait
      timer_.expires_after(std::chrono::milliseconds(period_ms_));
      timer_.async_wait(
          [this, self](const boost::system::error_code& ec) { handle(ec); });
    }
  }
};

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " total_time_ms period_ms"
              << std::endl;
    return 1;
  }

  boost::asio::io_context io_context;

  const auto task = PeriodicalTask::create(io_context, std::stoi(argv[2]));
  task->run();

  boost::asio::steady_timer total_timer(
      io_context, std::chrono::milliseconds(std::stoi(argv[1])));
  total_timer.expires_after(std::chrono::milliseconds(std::stoi(argv[1])));
  total_timer.async_wait(
      [task](const boost::system::error_code& ec) { task->cancel(); });

  io_context.run();
  LOG_INFO("io_context stopped");
  return 0;
}
