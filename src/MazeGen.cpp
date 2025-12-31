/**
 * @file MazeGen.cpp
 * @brief Implementation of Recursive Backtracking maze generation algorithm.
 */

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

auto MazeGen::initialize(size_t rows, size_t columns) -> void {
    rows_ = rows;
    cols_ = columns;
    grid_.clear();
    grid_.resize(rows * columns);

    // Reset cells
    std::fill(grid_.begin(), grid_.end(), Cell()); // ? why not just reset the cells rather than the whole vector. What are the tradeoffs?

    // Reset path stack
    path_stack_.clear();

    // Reset RNG
    rng_ = std::mt19937(std::random_device{}());

    if (rows > 0 && columns > 0) { // ? why is this needed when rows and columsn are of type unsigned long
        // Start position: top-left cell (0, 0)
        const size_t start_idx   = MazeSolver::get_1d_index(0, 0, cols_);
        grid_[start_idx].visited = true;
        path_stack_.emplace_back(0, 0);
        done_        = false;
        initialized_ = true;
    } else {
        done_        = true;
        initialized_ = false;
    }
}

auto MazeGen::step() -> bool {
    if (done_ || !initialized_ || path_stack_.empty()) { // ? why need path_stack_.empty() condition when done_ does the same thing
        if (!done_ && initialized_ && path_stack_.empty()) {
            // Just finished processing. Finalize entrance/exit.
            // Create entrance at (0,0) and exit at (rows-1, columns-1)
            const size_t start_idx = MazeSolver::get_1d_index(0, 0, cols_); // ? why not just assign 0
            const size_t end_idx =
                MazeSolver::get_1d_index(rows_ - 1, cols_ - 1, cols_);
            grid_[start_idx].top  = false;
            grid_[end_idx].bottom = false;
            done_                 = true;
            return false;
        }
        done_ = true;
        return false;
    }

    // Get current cell from end of path
    const int row = path_stack_.back().first;
    const int col = path_stack_.back().second;

    // Get all unvisited neighbors
    auto neighbors = get_neighbors(row, col);

    if (neighbors.empty()) {
        path_stack_.pop_back();  // Backtrack
    } else {
        // Select a random unvisited neighbor
        std::uniform_int_distribution<size_t> dist(0, neighbors.size() - 1);
        const size_t                          random_index = dist(rng_);

        // Get coordinates of the chosen random neighbor
        const int nrow = neighbors[random_index].first;
        const int ncol = neighbors[random_index].second;

        // Remove the wall between the current cell and the chosen neighbor
        remove_walls(row, col, nrow, ncol);

        // Mark the chosen neighbor as visited and push it onto the path
        const size_t n_idx = MazeSolver::get_1d_index( // ? why does get_1d_index belong to MazeSolver class? It should either belong to MazeGen or some utility class
            static_cast<size_t>(nrow), static_cast<size_t>(ncol), cols_);
        grid_[n_idx].visited = true;
        path_stack_.emplace_back(nrow, ncol); //push on to the "stack"
    }

    // Check if finished after this step
    if (path_stack_.empty()) {
        // Finalize immediately or wait for next call?
        const size_t start_idx = MazeSolver::get_1d_index(0, 0, cols_); // ? why not just assign 0
        const size_t end_idx =
            MazeSolver::get_1d_index(rows_ - 1, cols_ - 1, cols_);
        
        // ? why
        grid_[start_idx].top  = false;
        grid_[end_idx].bottom = false;

        done_                 = true;
        return false;  // Done (base condition of recursion)
    }

    return true;  // Still running
}

// cppcheck-suppress unusedFunction
auto MazeGen::is_done() const -> bool { return done_; } // ? is this ever used

// Collect all unvisited neighbors
auto MazeGen::get_neighbors(int row, int col) const
    -> std::vector<std::pair<int, int>> {
    std::vector<std::pair<int, int>> neighbors;


    // ? Khalid's idea: Can this be implemented using switch

    // Check if top neighbor is visited
    if (row > 0 && // ? row is size_t, it can't be negative
        !grid_[MazeSolver::get_1d_index(static_cast<size_t>(row) - 1,
                                        static_cast<size_t>(col), cols_)]
             .visited) {
        neighbors.emplace_back(row - 1, col);
    }

    // Check if bottom neighbor is visited
    if (row + 1 < static_cast<int>(rows_) &&
        !grid_[MazeSolver::get_1d_index(static_cast<size_t>(row) + 1,
                                        static_cast<size_t>(col), cols_)]
             .visited) {
        neighbors.emplace_back(row + 1, col);
    }

    // Check if left neighbor is visited
    if (col > 0 &&
        !grid_[MazeSolver::get_1d_index(static_cast<size_t>(row),
                                        static_cast<size_t>(col) - 1, cols_)]
             .visited) {
        neighbors.emplace_back(row, col - 1);
    }

    // Check if right neighbor is visited
    if (col + 1 < static_cast<int>(cols_) &&
        !grid_[MazeSolver::get_1d_index(static_cast<size_t>(row),
                                        static_cast<size_t>(col) + 1, cols_)]
             .visited) {
        neighbors.emplace_back(row, col + 1);
    }

    return neighbors;
}


// Remove walls between two adjacent cells
auto MazeGen::remove_walls(int row, int col, int nrow, int ncol) -> void {
    const size_t curr_idx = MazeSolver::get_1d_index(
        static_cast<size_t>(row), static_cast<size_t>(col), cols_);
    const size_t next_idx = MazeSolver::get_1d_index(
        static_cast<size_t>(nrow), static_cast<size_t>(ncol), cols_);

    if (nrow == row - 1) {  // Up
        grid_[curr_idx].top    = false;
        grid_[next_idx].bottom = false;
    } else if (nrow == row + 1) {  // Down
        grid_[curr_idx].bottom = false;
        grid_[next_idx].top    = false;
    } else if (ncol == col - 1) {  // Left
        grid_[curr_idx].left  = false;
        grid_[next_idx].right = false;
    } else if (ncol == col + 1) {  // Right
        grid_[curr_idx].right = false;
        grid_[next_idx].left  = false;
    }
}

// Static helper
// cppcheck-suppress unusedFunction
auto MazeGen::generate(size_t rows, size_t columns) -> std::vector<Cell> {
    MazeGen gen;
    gen.initialize(rows, columns);
    while (gen.step()) {
        // Loop until done
    }
    return gen.get_grid();
}

// NEW: MazeGen::render_ascii is now a public static method of MazeGen,
// and uses MazeSolver::get_1d_index.
// cppcheck-suppress unusedFunction
auto MazeGen::render_ascii(const std::vector<Cell> &grid, size_t rows,
                           size_t columns) -> std::string {
    std::ostringstream out;

    for (size_t r_idx = 0; r_idx < rows; ++r_idx) {
        for (size_t c_idx = 0; c_idx < columns; ++c_idx) {
            out << "+"
                << (grid[MazeSolver::get_1d_index(r_idx, c_idx, columns)].top
                        ? "---"
                        : "   ");
        }
        out << "+\n";

        for (size_t c_idx = 0; c_idx < columns; ++c_idx) {
            out << (grid[MazeSolver::get_1d_index(r_idx, c_idx, columns)].left
                        ? "|"
                        : " ");
            out << "   ";
        }
        out << "|\n";
    }

    for (size_t c_idx = 0; c_idx < columns; ++c_idx) {
        out << "+"
            << (grid[MazeSolver::get_1d_index(rows - 1, c_idx, columns)].bottom
                    ? "---"
                    : "   ");
    }
    out << "+\n";

    return out.str();
}
