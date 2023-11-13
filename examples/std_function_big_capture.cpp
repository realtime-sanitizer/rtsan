#include <functional>
#include <iostream>

float invoke(std::function<float()> func) { return func(); }

[[clang::realtime]] float process() {
  auto data = std::array<float, 8>{};
  auto func = [data]() { return data[3]; };

  return invoke(func);
}

int main() {

  std::cout << "I should fail!\n";
  return process();
}