/*
    This file is part of the RealtimeSanitizer (RTSan) project, under the
    Apache v2.0 license.

    https://github.com/realtime-sanitizer/rtsan
*/

#include <vector>
#include <iostream>

float process() [[clang::nonblocking]] {
  auto vec = std::vector<float>(3);
  return vec[1];
}

int main() {
  std::cout << "I should fail!\n";
  return process();
}
