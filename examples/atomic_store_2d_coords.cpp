#include <atomic>
#include <iostream>

struct Coords {
  double x{};
  double y{};
};

[[clang::realtime]] float process() {
  std::atomic<Coords> coords;

  coords.store({4.1f, 4.4f});
  return coords.load().x;
}

int main() {
  std::cout << "I should pass!\n";
  return process();
}