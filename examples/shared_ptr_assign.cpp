/*
    This file is part of the RealtimeSanitizer (RTSan) project, under the
    Apache v2.0 license.

    https://github.com/realtime-sanitizer/rtsan
*/

#include <memory>
#include <iostream>

std::shared_ptr<int> ptr_to_int = std::make_shared<int>(3);

int process() [[clang::nonblocking]] {
  auto new_ptr = ptr_to_int;
  (*new_ptr)++;
  return *new_ptr;
}

int main() {
  std::cout << "I should pass!\n";
  return process();
}
