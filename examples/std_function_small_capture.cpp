/*
    This file is part of the RealtimeSanitizer (RTSan) project, under the
    Apache v2.0 license.

    https://github.com/realtime-sanitizer/rtsan
*/

#include <functional>
#include <iostream>

float invoke(std::function<float()> &&func) { return func(); }

float process() [[clang::nonblocking]] {
  auto data = std::array<float, 3>{};
  return invoke([data]() { return data[2]; });
}

int main() {
  std::cout << "I should pass!\n";
  return process();
}
