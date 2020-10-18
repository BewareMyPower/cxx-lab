#include <functional>
#include <iostream>
#include <memory>
using namespace std;

namespace cxx17 {
struct Foo {
  std::shared_ptr<int> p = std::make_shared<int>(0);
  auto getCallback() {
    return [*this] {
      (*p)++;
      std::cout << "value: " << *p << " (refcnt = " << p.use_count() << ")"
                << std::endl;
    };
  }
};
}  // namespace cxx17

int main(int argc, char* argv[]) {
  auto callback = cxx17::Foo().getCallback();
  callback();  // value: 1 (refcnt = 1)
  callback();  // value: 2 (refcnt = 1)
  return 0;
}
