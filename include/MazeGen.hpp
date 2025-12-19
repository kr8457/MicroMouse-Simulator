#pragma once
#include <string>

constexpr unsigned __int64 EXAMPLE_RESULT = 42;

class MazeGen {
  public:
    // Generate an ASCII maze with the recursive backtracker algorithm.
    // The returned string contains multiple lines; each cell is rendered
    // using "+---+" for walls and "|   |" for vertical walls. The
    // entrance is at the top-left (opening on the top) and the exit is
    // at the bottom-right (opening on the bottom).
    static std::string generate(size_t rows, size_t columns);
    static unsigned __int64 example_func();
};
