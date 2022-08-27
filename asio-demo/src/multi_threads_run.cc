#include <atomic>
#include <boost/asio.hpp>
#include <chrono>
#include <functional>
#include <thread>

#include "simple_logger.h"

static void wait(std::function<bool()> condition, long timeoutMs) {
  while (timeoutMs > 0 && !condition()) {
    std::this_thread::sleep_for(std::chrono::microseconds(50));
    timeoutMs -= 50;
  }
}

int main(int argc, char* argv[]) {
  boost::asio::io_context io_context;
  boost::asio::io_context::work work(io_context);

  std::atomic_int num_threads{0};
  for (int i = 0; i < 3; i++) {
    num_threads++;
    std::thread t([&io_context, &num_threads] {
      boost::system::error_code ec;
      io_context.run(ec);
      LOG_INFO("io_context stopped");
      num_threads--;
    });
    t.detach();
  }

  // Wait until all threads call `io_context::run`
  std::this_thread::sleep_for(std::chrono::microseconds(100));

  std::atomic_int num_tasks{0};
  for (int i = 0; i < 10; i++) {
    num_tasks++;
    boost::asio::post(io_context, [i, &num_tasks] {
      LOG_INFO("Execute task ", i);
      num_tasks--;
    });
  }

  wait([&num_tasks] { return num_tasks == 0; }, 3000);
  io_context.stop();
  wait([&num_threads] { return num_threads == 0; }, 1000);
  return 0;
}
