#include <iostream>
using namespace std;

int main(int argc, char* argv[]) {
  // 不合法
  // auto x0{1, 2, 3};
  auto x1 = {1, 2, 3};        // std::initializer_list<int>
  cout << x1.size() << endl;  // 3
  auto x2{1};  // int，在之前推断为 std::initializer_list<int>
               // 然而对于 GCC，仅有 5 之前 4.8 之后的才这么推断
  cout << x2 << endl;
  // cout << x2.size() << endl;
  return 0;
}
