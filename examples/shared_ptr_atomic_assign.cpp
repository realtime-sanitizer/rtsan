/**
    This file is part of the RealtimeSanitizer (RADSan) project.
    https://github.com/realtime-sanitizer/radsan

    Copyright 2023 David Trevelyan & Alistair Barker
    Subject to GNU General Public License (GPL) v3.0
*/

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
