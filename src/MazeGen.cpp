#include "MazeGen.hpp"
#include <random>
#include <sstream>
#include <stack>
#include <tuple>
#include <vector>

struct Cell {
    bool visited = false;
    bool top     = true;
    bool bottom  = true;
    bool left    = true;
    bool right   = true;
};

namespace {

// Convert 2D (row,col) to 1D index
size_t index(size_t row, size_t col, size_t columns) {
    return (row * columns) + col;
}

// Collect all unvisited neighbors
std::vector<std::pair<int, int>> get_neighbors(int row, int col, size_t rows,
                                               size_t                   columns,
                                               const std::vector<Cell> &grid) {
    std::vector<std::pair<int, int>> neighbors;

    if (row > 0 && !grid[index(row - 1, col, columns)].visited) {
        neighbors.emplace_back(row - 1, col);
    }

    if (row + 1 < static_cast<int>(rows) &&
        !grid[index(row + 1, col, columns)].visited) {
        neighbors.emplace_back(row + 1, col);
    }

    if (col > 0 && !grid[index(row, col - 1, columns)].visited) {
        neighbors.emplace_back(row, col - 1);
    }

    if (col + 1 < static_cast<int>(columns) &&
        !grid[index(row, col + 1, columns)].visited) {
        neighbors.emplace_back(row, col + 1);
    }

    return neighbors;
}

// Remove walls between two adjacent cells
void remove_walls(int row, int col, int nrow, int ncol, size_t columns,
                  std::vector<Cell> &grid) {
    if (nrow == row - 1) {
        grid[index(row, col, columns)].top      = false;
        grid[index(nrow, ncol, columns)].bottom = false;
    } else if (nrow == row + 1) {
        grid[index(row, col, columns)].bottom = false;
        grid[index(nrow, ncol, columns)].top  = false;
    } else if (ncol == col - 1) {
        grid[index(row, col, columns)].left    = false;
        grid[index(nrow, ncol, columns)].right = false;
    } else {
        grid[index(row, col, columns)].right  = false;
        grid[index(nrow, ncol, columns)].left = false;
    }
}

// Render maze as ASCII
std::string render_ascii(const std::vector<Cell> &grid, size_t rows,
                         size_t columns) {
    std::ostringstream out;

    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < columns; ++col) {
            out << "+" << (grid[index(row, col, columns)].top ? "---" : "   ");
        }
        out << "+\n";

        for (size_t col = 0; col < columns; ++col) {
            out << (grid[index(row, col, columns)].left ? "|" : " ");
            out << "   ";
        }
        out << "|\n";
    }

    for (size_t col = 0; col < columns; ++col) {
        out << "+"
            << (grid[index(rows - 1, col, columns)].bottom ? "---" : "   ");
    }
    out << "+\n";

    return out.str();
}

}  // anonymous namespace

// ==========================================================
//                  MAZE GENERATION
// ==========================================================

std::string MazeGen::generate(size_t rows, size_t columns) {
    if (rows == 0 || columns == 0) {
        return {};
    }

    std::vector<Cell>               grid(rows * columns);
    std::stack<std::pair<int, int>> stack;

    std::mt19937 rng(std::random_device{}());

    // start position
    grid[index(0, 0, columns)].visited = true;
    stack.push({0, 0});

    while (!stack.empty()) {
        // Declare and initialize variables separately
        int row            = 0;
        int col            = 0;
        std::tie(row, col) = stack.top();

        auto neighbors = get_neighbors(row, col, rows, columns, grid);

        if (neighbors.empty()) {
            stack.pop();  // backtrack
            continue;
        }

        std::uniform_int_distribution<size_t> dist(0, neighbors.size() - 1);
        size_t                                random_index = dist(rng);

        // Declare and initialize variables separately
        int nrow             = 0;
        int ncol             = 0;
        std::tie(nrow, ncol) = neighbors[random_index];

        remove_walls(row, col, nrow, ncol, columns, grid);

        grid[index(nrow, ncol, columns)].visited = true;
        stack.push({nrow, ncol});
    }

    // Entrance and exit
    grid[index(0, 0, columns)].top                     = false;
    grid[index(rows - 1, columns - 1, columns)].bottom = false;

    return render_ascii(grid, rows, columns);
}

unsigned __int64 MazeGen::example_func() { return EXAMPLE_RESULT; }