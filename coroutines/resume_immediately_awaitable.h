#pragma once

#include <concepts>
#include <coroutine>

#include "noncopyable.h"

template <typename T>
requires std::default_initializable<T>&& std::movable<T> class Promise
    : public noncopyable {
 public:
  auto get_return_object();
  auto initial_suspend() const noexcept { return std::suspend_always{}; }
  auto return_value(T&& value) { value_ = std::move(value); }
  auto final_suspend() const noexcept { return std::suspend_always{}; }
  auto unhandled_exception() const noexcept {}

  decltype(auto) release() noexcept { return std::move(value_); }
  const auto& value() const noexcept { return value_; }

 private:
  T value_;
};

// An awaitable that resumes immediately in current thread
template <typename T>
requires std::default_initializable<T>&& std::movable<T> class Awaitable
    : public noncopyable {
  using Handle = std::coroutine_handle<Promise<T>>;

 public:
  using promise_type = Promise<T>;
  Awaitable(Promise<T>& promise) : handle_(Handle::from_promise(promise)) {}
  ~Awaitable() { handle_.destroy(); }

  decltype(auto) operator()() const {
    handle_.resume();
    return handle_.promise().value();
  }

  constexpr auto await_ready() const noexcept { return false; }
  auto await_suspend(std::coroutine_handle<> caller) const {
    handle_.resume();
    return caller;
  }
  decltype(auto) await_resume() noexcept { return handle_.promise().release(); }

 private:
  Handle handle_;
};

template <typename T>
requires std::default_initializable<T>&& std::movable<T> inline auto
Promise<T>::get_return_object() {
  return Awaitable<T>{*this};
}
