#include <concepts>
#include <coroutine>
#include <iostream>

template <typename T>
concept Addable = std::default_initializable<T>&& requires(T x, T& y) {
  y += x;
};

template <Addable T>
class Task {
 public:
  struct promise_type {
    auto get_return_object();
    auto initial_suspend() const noexcept { return std::suspend_always{}; }
    auto return_value(T value) noexcept { sum = value; }
    auto final_suspend() const noexcept { return FinalAwaiter{}; }
    auto unhandled_exception() const noexcept {}

    T sum;
    std::coroutine_handle<promise_type> caller;

    struct FinalAwaiter : std::suspend_always {
      std::coroutine_handle<> await_suspend(
          std::coroutine_handle<promise_type> caller) noexcept;
    };
  };

  Task(promise_type& promise)
      : handle_(std::coroutine_handle<promise_type>::from_promise(promise)) {}
  ~Task() { handle_.destroy(); }

  decltype(auto) operator()() noexcept { handle_.resume(); }

  auto result() const noexcept { return handle_.promise().sum; }

  constexpr auto await_ready() const noexcept { return false; }

  auto await_suspend(std::coroutine_handle<promise_type> caller) const {
    handle_.promise().caller = caller;
    return handle_;
  }
  decltype(auto) await_resume() noexcept { return handle_.promise().sum; }

 private:
  std::coroutine_handle<promise_type> handle_;
};

template <Addable T>
inline auto Task<T>::promise_type::get_return_object() {
  return Task{*this};
}

template <Addable T>
inline std::coroutine_handle<>
Task<T>::promise_type::FinalAwaiter::await_suspend(
    std::coroutine_handle<promise_type> handle) noexcept {
  auto caller = handle.promise().caller;
  if (caller) {
    return caller;
  } else {
    return std::noop_coroutine();
  }
}

Task<long> f(long initial, long x) {
  if (x <= 0) {
    co_return initial;
  }
  co_return co_await f(initial + x, x - 1);
}

int main(int argc, char* argv[]) {
  auto n = (argc > 1) ? std::stol(argv[1]) : 5;
  auto coro = f(0, n);
  coro();
  std::cout << coro.result() << std::endl;
}
