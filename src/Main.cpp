#include "MazeGen.hpp"
#include <iostream>

int main() {
  MazeGen gen;
  auto maze = gen.generate(5, 10);
  std::cout << "Hello Maze!\n";
  std::cout << maze << "\n";
}