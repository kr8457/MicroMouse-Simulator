#pragma once
#include <iostream>
#include <string>
#include <vector>

class MazeGen {
public:
  std::string generate(size_t rows, size_t columns);
  static size_t exampleFunc();
};