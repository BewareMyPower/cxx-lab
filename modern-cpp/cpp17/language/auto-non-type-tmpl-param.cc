#include <iostream>
#include <utility>
using namespace std;

// C++17 之前，必须显式定义模板参数类型
namespace cxx14 {
template <typename T1, T1 x, typename T2, T2 y, typename T3, T3 z>
struct Triple {
  static constexpr auto sum = x + y + z;
};
}  // namespace cxx14

// C++17 开始，可以用 auto 自动推断模板参数类型
namespace cxx17 {
template <auto x, auto y, auto z>
struct Triple {
  static constexpr auto sum = x + y + z;
};
}  // namespace cxx17

int main(int argc, char* argv[]) {
  // sum: 48 + 1 + 100 = 149 (long)
  cout << cxx14::Triple<char, '0', int, 1, long, 100L>::sum << endl;
  cout << cxx17::Triple<'0', 1, 100L>::sum << endl;
  return 0;
}
