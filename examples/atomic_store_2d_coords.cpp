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
