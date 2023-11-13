/**
    This file is part of the RealtimeSanitizer (RADSan) project.
    https://github.com/realtime-sanitizer/radsan

    Copyright 2023 David Trevelyan & Alistair Barker
    Subject to GNU General Public License (GPL) v3.0
*/

#include <atomic>
#include <iostream>

struct Coords {
  double x{};
  double y{};
};

[[clang::realtime]] double process() {
  std::atomic<Coords> coords;
  coords.store({4.1, 4.4});
  return coords.load().x;
}

int main() {
  std::cout << "I should pass!\n";
  process();
  return 0;
}
