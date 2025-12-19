#include "MazeGen.hpp"
#include <cstddef>
#include <iostream>

int main() {
    constexpr std::size_t ROWS = 10;
    constexpr std::size_t COLS = 15;

    auto maze = MazeGen::generate(ROWS, COLS);
    std::cout << "Hello Maze!\n";
    std::cout << maze << "\n";
}
