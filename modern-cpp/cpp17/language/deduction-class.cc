#include <iostream>
#include <string>
using namespace std;

template <typename T = std::string>
struct Foo {
  T value;

  Foo() : value{} {}
  Foo(T value) : value{value} {}
};

int main(int argc, char* argv[]) {
  // 1. 对于默认构造，直接使用模板参数的默认值
  Foo foo1;  // Foo<std::string>
  // 2. 对于带参构造，根据参数类型推导
  Foo foo2{1};  // Foo<int>

  foo1.value = "hello";
  cout << foo1.value << endl;
  cout << foo2.value << endl;

  return 0;
}
