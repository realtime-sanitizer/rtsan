#include <memory>
#include <iostream>

std::shared_ptr<int> ptr_to_int = std::make_shared<int>(3);

[[clang::realtime]] int process() {
  auto new_ptr = ptr_to_int;
  (*new_ptr)++;
  return *new_ptr;
}

int main() {
  std::cout << "I should pass!\n";
  return process();
}
