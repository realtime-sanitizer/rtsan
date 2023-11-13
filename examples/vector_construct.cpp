#include <vector>
#include <iostream>

[[clang::realtime]] float process() {
  auto vec = std::vector<float>(3);
  return vec[1];
}

int main() {
  std::cout << "I should fail!\n";
  return process();
}
