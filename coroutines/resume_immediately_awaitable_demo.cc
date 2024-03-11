#include <iostream>
#include <memory>

#include "resume_immediately_awaitable.h"

Awaitable<int> f(int x) { co_return x + 1; }
Awaitable<int> g(int x) { co_return(co_await f(x)) + 2; }

Awaitable<std::unique_ptr<int>> ptr_f(int x) {
  co_return std::make_unique<int>(x + 1);
}

Awaitable<std::unique_ptr<int>> ptr_g(int x) {
  auto ptr = std::move(co_await ptr_f(x));
  *ptr += 2;
  co_return std::move(ptr);
}

int main() {
  auto coro_int = g(100);
  std::cout << coro_int() << std::endl;
  auto coro_ptr = ptr_g(100);
  std::cout << *coro_ptr() << std::endl;
}
