int main(int argc, char* argv[]) {
  auto less = [](int x, int y) constexpr { return x < y; };
  static_assert(less(1, 2));
  auto sum = [](int x, int y) constexpr { return x + y; };
  static_assert(sum(1, 2) == 3);
  return 0;
}
