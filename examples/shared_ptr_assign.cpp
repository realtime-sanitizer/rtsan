#include <memory>
#include <iostream>

std::shared_ptr<int> ptr_to_int = std::make_shared<int> (3);

std::shared_ptr<int> mutate(std::shared_ptr<int> ptr) {
  auto new_ptr = ptr;
  (*new_ptr)++;
  return new_ptr;
}

[[clang::realtime]] float process() {
  mutate(ptr_to_int);
  return *ptr_to_int;
}

int main() {
  std::cout << "I should pass!\n";
  return process();
}