#include <MazeGen.hpp>

std::string MazeGen::generate(size_t rows, size_t columns) {
  std::string maze = "";
  for (size_t row = 0; row < rows; row++) {
    maze += "|";
    for (size_t column = 0; column < columns; column++) {
      maze += "-";
    }
    maze += "|\n";
  }
  return maze;
}