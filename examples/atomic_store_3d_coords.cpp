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
  double z{};
};

std::atomic<Coords> coords;

double process() [[clang::nonblocking]] {
  coords.store({4.1, 4.4, 5.2});
  return coords.load().x;
}

int main() {
  std::cout << "I should fail!\n";
  process();
  return 0;
}
