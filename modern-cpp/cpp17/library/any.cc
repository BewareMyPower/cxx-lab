#include <any>
#include <functional>
#include <iostream>
#include <type_traits>
#include <unordered_map>
#include <vector>
using namespace std;

class Manager {
 public:
  template <typename T>
  void add(const T& value) {
    if (std::is_integral<T>::value) {
      callbacks["integral"].emplace_back(value);
    } else {
      callbacks["non-integral"].emplace_back(value);
    }
  }

  void show() {
    std::cout << "# " << callbacks["integral"].size() << " integrals" << endl;
    for (const auto& x : callbacks["integral"]) {
      // NOTE: 这里为简化逻辑直接将 x 转换为 int，实际上 x 可能是其他的整型
      std::cout << std::any_cast<int>(x) << std::endl;
    }
    std::cout << "# " << callbacks["non-integral"].size() << " non-integrals"
              << std::endl;
  }

 private:
  std::unordered_map<std::string, std::vector<std::any>> callbacks;
};

int main(int argc, char* argv[]) {
  Manager manager;
  manager.add(1);
  manager.add(-1);
  manager.add("hello");
  // 'x' 也是整型，但无法转换成 int，std::any_cast 会抛出 std::bad_any_cast 异常
  // manager.add('x');
  manager.show();
  return 0;
}
