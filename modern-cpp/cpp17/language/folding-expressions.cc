#include <iostream>
using namespace std;

// C++17 之前，需要用递归的方式实现
namespace cxx14 {

template <typename T>
constexpr auto sum(T t) {
  return t;
}

template <typename T, typename... Args>
constexpr auto sum(T first, Args... args) {
  return first + sum(args...);
}

}  // namespace cxx14

// C++17 开始可以折叠表达式
namespace cxx17 {
template <typename... Args>
auto sum(Args... args) {
  return (... + args);
}
}  // namespace cxx17

int main(int argc, char* argv[]) {
  // 1+1+2+3+5 = 12
  cout << cxx14::sum(1, 1, 2, 3, 5) << endl;
  cout << cxx17::sum(1, 1, 2, 3, 5) << endl;
  return 0;
}
