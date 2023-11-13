#include <atomic>
#include <iostream>
#include <memory>

std::shared_ptr<int> ptr_to_int = std::make_shared<int>(3);

[[clang::realtime]] int process() {
  auto new_ptr = std::atomic_load(&ptr_to_int);
  (*new_ptr)++;
  return *new_ptr;
}

int main() {
  std::cout << "I should fail!\n";
  return process();
}
