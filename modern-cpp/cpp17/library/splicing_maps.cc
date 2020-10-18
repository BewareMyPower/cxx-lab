#include <iostream>
#include <unordered_map>
using namespace std;

int main(int argc, char* argv[]) {
  std::unordered_map<int, string> m1{{1, "one"}, {2, "two"}, {3, "three"}};
  decltype(m1) m2;

  auto show = [&m1, &m2] {
    cout << "m1: " << endl;
    for (const auto& [key, value] : m1) {
      cout << key << " => " << value << endl;
    }
    cout << "m2: " << endl;
    for (const auto& [key, value] : m2) {
      cout << key << " => " << value << endl;
    }
  };

  m2.insert(m1.extract(1));  // 以廉价的开销将 key 为 1 的节点从 m1 移动到 m2
  // m2.insert(m1.extract(m1.find(1))); // 与上面一行等价操作
  show();
  // m1:
  // 3 => three
  // 2 => two
  // m2:
  // 1 => one

  // 以廉价的操作合并 m2 到 m1，平均时间复杂度 O(N)，最坏时间复杂度为
  // O(N*size()+N)，N 为 m2 的大小
  m1.merge(m2);
  show();
  // m1:
  // 1 => one
  // 3 => three
  // 2 => two
  // m2:
  return 0;
}
