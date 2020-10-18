#include <iostream>
#include <optional>
#include <string>
using namespace std;

inline std::optional<int> strToInt(const std::string& s) {
  try {
    return std::stoi(s);
  } catch (const std::exception& e) {
    std::cerr << "strToInt(\"" << s << "\") failed: " << e.what() << std::endl;
    return {};
  }
}

inline void callStrToInt(const std::string& s) {
  // 使用 if 判断 optional 是否非空
  if (auto optInteger = strToInt(s)) {
    cout << optInteger.value() << endl;
  }
}

int main(int argc, char* argv[]) {
  callStrToInt("1");  // 1
  callStrToInt(
      "12345678901");   // strToInt("1234567890") failed: stoi: out of range
  callStrToInt("xyz");  // strToInt("xyz") failed: stoi: no conversion

  // 出错时采用默认值
  auto optInteger =
      strToInt("xyz");  // strToInt("xyz") failed: stoi: no conversion
  cout << optInteger.value_or(-1) << endl;  // -1
  return 0;
}
