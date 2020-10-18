#include <iostream>
#include <string_view>
#include <vector>
using namespace std;

// 使用 string_view 保存分割结果，每个 string_view
// 仅保存两个指针，因此不会有复制整个字符串的开销
inline auto splitToViews(const std::string& s, const std::string& separator) {
  std::vector<std::string_view> tokens;
  size_t pos = 0;
  while (true) {
    if (auto new_pos = s.find(separator, pos); new_pos != std::string::npos) {
      tokens.emplace_back(&s[pos], new_pos - pos);
      pos = new_pos + separator.size();
    } else {
      tokens.emplace_back(&s[pos]);
      break;
    }
  }
  return tokens;
}

int main(int argc, char* argv[]) {
  for (const auto& view : splitToViews("1, 2, 3", ", ")) {
    cout << view << endl;
  }
  return 0;
}
