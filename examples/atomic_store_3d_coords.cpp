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
