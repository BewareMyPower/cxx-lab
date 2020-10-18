#include <iostream>
#include <variant>
using namespace std;

namespace cxx14 {
union IntOrDouble {
  int i;
  double d;
};

inline void f() {
  IntOrDouble x;
  x.i = 100;
  cout << "i = " << x.i << endl;  // i = 100
  // 访问 d 是类型不安全的，因为 double 占 8 个字节，而 int 只占 4
  // 个字节，也就是结果因运行环境平台而异，比如以下注释是在 MacOS Clang 11.0.3
  // 运行的结果
  cout << "d = " << x.d << endl;  // d = 4.94066e-322
}
}  // namespace cxx14

namespace cxx17 {
inline void f() {
  std::variant<int, double> v{100};
  cout << "i = " << std::get<int>(v) << endl;  // i = 100
  cout << "i = " << std::get<0>(v) << endl;    // i = 100
  // 以下调用都会抛出 std::bad_variant_access 异常
  //  cout << "d = " << std::get<double>(v) << endl;
  //  cout << "d = " << std::get<1>(v) << endl;
  // 即使是 std::variant<int, unsigned int>
  // 这种字节大小一样的两种类型，也只能访问其中一种类型
}
}  // namespace cxx17

int main(int argc, char* argv[]) {
  cxx14::f();
  cxx17::f();
  return 0;
}
