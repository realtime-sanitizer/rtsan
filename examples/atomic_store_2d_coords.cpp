/*
    This file is part of the RealtimeSanitizer (RTSan) project, under the
    Apache v2.0 license.

    https://github.com/realtime-sanitizer/rtsan
*/

#include <atomic>
#include <iostream>

struct Coords {
  double x{};
  double y{};
};

double process() [[clang::nonblocking]] {
  std::atomic<Coords> coords;
  coords.store({4.1, 4.4});
  return coords.load().x;
}

int main() {
  std::cout << "I should pass!\n";
  process();
  return 0;
}
