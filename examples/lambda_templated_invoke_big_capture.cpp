#include <functional>
#include <iostream>

template <typename Func> float invoke(Func &&func) { return func(); }

[[clang::realtime]] float process() {
  auto data = std::array<float, 8>{};
  auto func = [data]() { return data[3]; };

  return invoke(func);
}

int main() {
  std::cout << "I should pass!\n";

  return process();
}