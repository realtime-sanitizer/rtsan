#include <functional>
#include <iostream>

float invoke(std::function<float()> &&func) { return func(); }

[[clang::realtime]] float process() {
  auto data = std::array<float, 3>{};
  return invoke([data]() { return data[2]; });
}

int main() {
  std::cout << "I should pass!\n";
  return process();
}
