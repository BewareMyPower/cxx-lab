#include <atomic>
#include <boost/asio.hpp>
#include <chrono>
#include <thread>

#include "simple_logger.h"
using boost::asio::thread_pool;

inline void sleep(int timeMs) {
  std::this_thread::sleep_for(std::chrono::milliseconds(timeMs));
}

int main(int argc, char* argv[]) {
  thread_pool pool{1};
  std::atomic_int numTaskDone{0};
  constexpr int N = 5;
  for (int i = 0; i < N; i++) {
    boost::asio::dispatch(pool, [i, &numTaskDone] {
      sleep(100);
      LOG_INFO("XYZ ", i);
      numTaskDone++;
    });
  }
  boost::asio::dispatch(pool, [] {
    sleep(3000);
    LOG_INFO("last task");
  });
  boost::asio::dispatch(pool, [] { LOG_INFO("It will be cancelled"); });
  while (numTaskDone < N) {
    sleep(10);
  }
  pool.stop();
  LOG_INFO("Try pool.join()...");
  // join() will wait the last long term running task. Even if join() is not
  // called, the destructor of pool will still wait for existing tasks.
  pool.join();
  LOG_INFO("pool.join() done.");
  return 0;
}
