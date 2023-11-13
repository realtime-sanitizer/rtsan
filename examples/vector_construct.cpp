/**
    This file is part of the RealtimeSanitizer (RADSan) project.
    https://github.com/realtime-sanitizer/radsan

    Copyright 2023 David Trevelyan & Alistair Barker
    Subject to GNU General Public License (GPL) v3.0
*/

#include <vector>
#include <iostream>

[[clang::realtime]] float process() {
  auto vec = std::vector<float>(3);
  return vec[1];
}

int main() {
  std::cout << "I should fail!\n";
  return process();
}
