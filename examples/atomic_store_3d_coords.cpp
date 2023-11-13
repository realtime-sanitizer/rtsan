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
  double z{};
};

std::atomic<Coords> coords;

[[clang::realtime]] double process() {
  coords.store({4.1, 4.4, 5.2});
  return coords.load().x;
}

int main() {
  std::cout << "I should fail!\n";
  process();
  return 0;
}
