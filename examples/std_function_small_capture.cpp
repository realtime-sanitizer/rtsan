/**
    This file is part of the RealtimeSanitizer (RADSan) project.
    https://github.com/realtime-sanitizer/radsan

    Copyright 2023 David Trevelyan & Alistair Barker
    Subject to GNU General Public License (GPL) v3.0
*/

#include <functional>
#include <iostream>

float invoke(std::function<float()> &&func) { return func(); }

[[clang::realtime]] float process() {
  auto data = std::array<float, 3>{};
  return invoke([data]() { return data[2]; });
}

int main() {
  std::cout << "I should pass!\n";
  return process();
}
