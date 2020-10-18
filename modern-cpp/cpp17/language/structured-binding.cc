#include <iostream>
#include <map>
using namespace std;

namespace cxx14 {
// key: name, value: age
inline std::map<std::string, int> studentAges{
    {"Jason", 24}, {"Dick", 25}, {"Damian", 18}};

inline void f() {
  for (const auto& kv : studentAges) {
    // 手动定义引用从而使 key 和 value 更具意义
    const auto& name = kv.first;
    int age = kv.second;
    std::cout << name << ": " << age << std::endl;
  }
}
}  // namespace cxx14

namespace cxx17 {
// key: name, value: age
inline std::map<std::string, int> studentAges{
    {"Jason", 24}, {"Dick", 25}, {"Damian", 18}};

inline void f() {
  // 结构化绑定，使代码更优雅
  for (const auto& [name, age] : studentAges) {
    std::cout << name << ": " << age << std::endl;
  }
}
}  // namespace cxx17

int main(int argc, char* argv[]) {
  cxx14::f();
  cxx17::f();
  return 0;
}
