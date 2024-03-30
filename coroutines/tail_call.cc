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
    auto return_value(T value) noexcept {
      sum += value;
      std::cout << "Promise::return_value " << sum << " " << this << std::endl;
    }
    auto final_suspend() const noexcept {
      std::cout << "Promise::final_suspend " << this << std::endl;
      return std::suspend_always{};
    }
    auto unhandled_exception() const noexcept {}

    T sum;

    promise_type() {
      std::cout << "Promise(" << sum << ") " << this << std::endl;
    }

    ~promise_type() {
      std::cout << "~Promise(" << sum << ") " << this << std::endl;
    }
  };

  Task(promise_type& promise)
      : handle_(std::coroutine_handle<promise_type>::from_promise(promise)) {}
  ~Task() { handle_.destroy(); }

  decltype(auto) operator()() noexcept {
    handle_.resume();
    return std::move(handle_.promise().sum);
  }

  constexpr auto await_ready() const noexcept { return false; }
  auto await_suspend(std::coroutine_handle<promise_type> caller) const {
    handle_.resume();
    return caller;
  }
  decltype(auto) await_resume() noexcept {
    return std::move(handle_.promise().sum);
  }

 private:
  std::coroutine_handle<promise_type> handle_;
};

template <Addable T>
inline auto Task<T>::promise_type::get_return_object() {
  return Task{*this};
}

Task<int> f(int initial, int x) {
  std::cout << "f(" << initial << ", " << x << ")\n";
  if (x <= 0) {
    co_return initial;
  }
  co_return co_await f(initial + x, x - 1);
}

int main() {
  auto coro = f(0, 3);
  std::cout << coro() << std::endl;
}
