#include "MazeGen.hpp"
#include "MazeSolver.hpp"
#include <random>
#include <sstream>
#include <stack>
#include <tuple>
#include <vector>

// ==========================================================
//                  MAZE GENERATION IMPLEMENTATION
// ==========================================================

void MazeGen::initialize(size_t rows, size_t columns) {
    rows_ = rows;
    cols_ = columns;
    grid_.clear();
    grid_.resize(rows * columns);

    // Reset cells
    for (size_t i = 0; i < grid_.size(); ++i) {
        grid_[i] = Cell();  // Reset to default (walls everywhere, unvisited)
    }

    // Reset stack
    while (!stack_.empty())
        stack_.pop();

    // Reset RNG
    rng_ = std::mt19937(std::random_device{}());

    if (rows > 0 && columns > 0) {
        // Start position: top-left cell (0, 0)
        size_t start_idx         = MazeSolver::get_1d_index(0, 0, cols_);
        grid_[start_idx].visited = true;
        stack_.push({0, 0});
        done_        = false;
        initialized_ = true;
    } else {
        done_        = true;
        initialized_ = false;
    }
}

bool MazeGen::step() {
    if (done_ || !initialized_ || stack_.empty()) {
        if (!done_ && initialized_ && stack_.empty()) {
            // Just finished processing. Finalize entrance/exit.
            // Create entrance at (0,0) and exit at (rows-1, columns-1)
            size_t start_idx = MazeSolver::get_1d_index(0, 0, cols_);
            size_t end_idx =
                MazeSolver::get_1d_index(rows_ - 1, cols_ - 1, cols_);
            grid_[start_idx].top  = false;
            grid_[end_idx].bottom = false;
            done_                 = true;
            return false;
        }
        done_ = true;
        return false;
    }

    // Get current cell from top of stack
    int row = stack_.top().first;
    int col = stack_.top().second;

    // Get all unvisited neighbors
    auto neighbors = get_neighbors(row, col);

    if (neighbors.empty()) {
        stack_.pop();  // Backtrack
    } else {
        // Select a random unvisited neighbor
        std::uniform_int_distribution<size_t> dist(0, neighbors.size() - 1);
        size_t                                random_index = dist(rng_);

        // Get coordinates of the chosen random neighbor
        int nrow = neighbors[random_index].first;
        int ncol = neighbors[random_index].second;

        // Remove the wall between the current cell and the chosen neighbor
        remove_walls(row, col, nrow, ncol);

        // Mark the chosen neighbor as visited and push it onto the stack
        size_t n_idx         = MazeSolver::get_1d_index(nrow, ncol, cols_);
        grid_[n_idx].visited = true;
        stack_.push({nrow, ncol});
    }

    // Check if finished after this step
    if (stack_.empty()) {
        // Finalize immediately or wait for next call?
        size_t start_idx = MazeSolver::get_1d_index(0, 0, cols_);
        size_t end_idx = MazeSolver::get_1d_index(rows_ - 1, cols_ - 1, cols_);
        grid_[start_idx].top  = false;
        grid_[end_idx].bottom = false;
        done_                 = true;
        return false;  // Done
    }

    return true;  // Still running
}

bool MazeGen::is_done() const { return done_; }

// Collect all unvisited neighbors
std::vector<std::pair<int, int>> MazeGen::get_neighbors(int row,
                                                        int col) const {
    std::vector<std::pair<int, int>> neighbors;

    if (row > 0 &&
        !grid_[MazeSolver::get_1d_index(row - 1, col, cols_)].visited) {
        neighbors.emplace_back(row - 1, col);
    }

    if (row + 1 < static_cast<int>(rows_) &&
        !grid_[MazeSolver::get_1d_index(row + 1, col, cols_)].visited) {
        neighbors.emplace_back(row + 1, col);
    }

    if (col > 0 &&
        !grid_[MazeSolver::get_1d_index(row, col - 1, cols_)].visited) {
        neighbors.emplace_back(row, col - 1);
    }

    if (col + 1 < static_cast<int>(cols_) &&
        !grid_[MazeSolver::get_1d_index(row, col + 1, cols_)].visited) {
        neighbors.emplace_back(row, col + 1);
    }

    return neighbors;
}

// Remove walls between two adjacent cells
void MazeGen::remove_walls(int row, int col, int nrow, int ncol) {
    size_t curr_idx = MazeSolver::get_1d_index(row, col, cols_);
    size_t next_idx = MazeSolver::get_1d_index(nrow, ncol, cols_);

    if (nrow == row - 1) {  // Up
        grid_[curr_idx].top    = false;
        grid_[next_idx].bottom = false;
    } else if (nrow == row + 1) {  // Down
        grid_[curr_idx].bottom = false;
        grid_[next_idx].top    = false;
    } else if (ncol == col - 1) {  // Left
        grid_[curr_idx].left  = false;
        grid_[next_idx].right = false;
    } else {  // Right
        grid_[curr_idx].right = false;
        grid_[next_idx].left  = false;
    }
}

// Static helper
std::vector<Cell> MazeGen::generate(size_t rows, size_t columns) {
    MazeGen gen;
    gen.initialize(rows, columns);
    while (gen.step()) {
        // Loop until done
    }
    return gen.get_grid();
}

// NEW: MazeGen::render_ascii is now a public static method of MazeGen,
// and uses MazeSolver::get_1d_index.
std::string MazeGen::render_ascii(const std::vector<Cell> &grid, size_t rows,
                                  size_t columns) {
    std::ostringstream out;

    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < columns; ++col) {
            out << "+"
                << (grid[MazeSolver::get_1d_index(row, col, columns)].top
                        ? "---"
                        : "   ");
        }
        out << "+\n";

        for (size_t col = 0; col < columns; ++col) {
            out << (grid[MazeSolver::get_1d_index(row, col, columns)].left
                        ? "|"
                        : " ");
            out << "   ";
        }
        out << "|\n";
    }

    for (size_t col = 0; col < columns; ++col) {
        out << "+"
            << (grid[MazeSolver::get_1d_index(rows - 1, col, columns)].bottom
                    ? "---"
                    : "   ");
    }
    out << "+\n";

    return out.str();
}

unsigned __int64 MazeGen::example_func() { return 42; }
