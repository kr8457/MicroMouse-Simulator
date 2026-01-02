/**
 * @file RecursiveBacktracker.cpp
 * @brief Implementation of Recursive Backtracking maze generation algorithm.
 */

#include "generators/RecursiveBacktracker.hpp"
#include <random>
#include <sstream>
#include <vector>

auto RecursiveBacktracker::initialize(size_t rows, size_t columns) -> void {
    rows_ = rows;
    cols_ = columns;
    grid_.clear();
    grid_.resize(rows * columns);

    // Reset cells
    std::fill(grid_.begin(), grid_.end(), Cell());

    // Reset path stack
    path_stack_.clear();

    // Reset RNG
    rng_ = std::mt19937(std::random_device{}());

    if (rows > 0 && columns > 0) {
        // Start position: top-left cell (0, 0)
        const size_t START_IDX   = MazeUtils::get_1d_index(0, 0, cols_);
        grid_[START_IDX].visited = true;
        path_stack_.emplace_back(0, 0);
        done_        = false;
        initialized_ = true;
    } else {
        done_        = true;
        initialized_ = false;
    }
}

auto RecursiveBacktracker::step() -> bool {
    if (done_ || !initialized_ || path_stack_.empty()) {
        if (!done_ && initialized_ && path_stack_.empty()) {
            // Just finished processing. Finalize entrance/exit.
            // Create entrance at (0,0) and exit at (rows-1, columns-1)
            const size_t START_IDX = MazeUtils::get_1d_index(0, 0, cols_);
            const size_t END_IDX =
                MazeUtils::get_1d_index(rows_ - 1, cols_ - 1, cols_);
            grid_[START_IDX].top  = false;
            grid_[END_IDX].bottom = false;
            done_                 = true;
            return false;
        }
        done_ = true;
        return false;
    }

    // Get current cell from end of path
    const int ROW = path_stack_.back().first;
    const int COL = path_stack_.back().second;

    // Get all unvisited neighbors
    auto neighbors = get_neighbors(ROW, COL);

    if (neighbors.empty()) {
        path_stack_.pop_back();  // Backtrack
    } else {
        // Select a random unvisited neighbor
        std::uniform_int_distribution<size_t> dist(0, neighbors.size() - 1);
        const size_t                          RANDOM_INDEX = dist(rng_);

        // Get coordinates of the chosen random neighbor
        const int NROW = neighbors[RANDOM_INDEX].first;
        const int NCOL = neighbors[RANDOM_INDEX].second;

        // Remove the wall between the current cell and the chosen neighbor
        remove_walls(ROW, COL, NROW, NCOL);

        // Mark the chosen neighbor as visited and push it onto the path
        const size_t N_IDX = MazeUtils::get_1d_index(
            static_cast<size_t>(NROW), static_cast<size_t>(NCOL), cols_);
        grid_[N_IDX].visited = true;
        path_stack_.emplace_back(NROW, NCOL); //push on to the "stack"
    }

    // Check if finished after this step
    if (path_stack_.empty()) {
        // Finalize immediately
        const size_t START_IDX = MazeUtils::get_1d_index(0, 0, cols_);
        const size_t END_IDX =
            MazeUtils::get_1d_index(rows_ - 1, cols_ - 1, cols_);
        
        grid_[START_IDX].top  = false;
        grid_[END_IDX].bottom = false;

        done_                 = true;
        return false;  // Done (base condition of recursion)
    }

    return true;  // Still running
}

auto RecursiveBacktracker::is_done() const -> bool { return done_; }

auto RecursiveBacktracker::get_neighbors(int row, int col) const
    -> std::vector<std::pair<int, int>> {
    std::vector<std::pair<int, int>> neighbors;

    // Check if top neighbor is visited
    if (row > 0 &&
        !grid_[MazeUtils::get_1d_index(static_cast<size_t>(row) - 1,
                                        static_cast<size_t>(col), cols_)]
             .visited) {
        neighbors.emplace_back(row - 1, col);
    }

    // Check if bottom neighbor is visited
    if (row + 1 < static_cast<int>(rows_) &&
        !grid_[MazeUtils::get_1d_index(static_cast<size_t>(row) + 1,
                                        static_cast<size_t>(col), cols_)]
             .visited) {
        neighbors.emplace_back(row + 1, col);
    }

    // Check if left neighbor is visited
    if (col > 0 &&
        !grid_[MazeUtils::get_1d_index(static_cast<size_t>(row),
                                        static_cast<size_t>(col) - 1, cols_)]
             .visited) {
        neighbors.emplace_back(row, col - 1);
    }

    // Check if right neighbor is visited
    if (col + 1 < static_cast<int>(cols_) &&
        !grid_[MazeUtils::get_1d_index(static_cast<size_t>(row),
                                        static_cast<size_t>(col) + 1, cols_)]
             .visited) {
        neighbors.emplace_back(row, col + 1);
    }

    return neighbors;
}


auto RecursiveBacktracker::remove_walls(int row, int col, int nrow, int ncol) -> void {
    const size_t CURR_IDX = MazeUtils::get_1d_index(
        static_cast<size_t>(row), static_cast<size_t>(col), cols_);
    const size_t NEXT_IDX = MazeUtils::get_1d_index(
        static_cast<size_t>(nrow), static_cast<size_t>(ncol), cols_);

    if (nrow == row - 1) {  // Up
        grid_[CURR_IDX].top    = false;
        grid_[NEXT_IDX].bottom = false;
    } else if (nrow == row + 1) {  // Down
        grid_[CURR_IDX].bottom = false;
        grid_[NEXT_IDX].top    = false;
    } else if (ncol == col - 1) {  // Left
        grid_[CURR_IDX].left  = false;
        grid_[NEXT_IDX].right = false;
    } else if (ncol == col + 1) {  // Right
        grid_[CURR_IDX].right = false;
        grid_[NEXT_IDX].left  = false;
    }
}

auto RecursiveBacktracker::generate(size_t rows, size_t columns) -> std::vector<Cell> {
    RecursiveBacktracker gen;
    gen.initialize(rows, columns);
    while (gen.step()) {
        // Loop until done
    }
    return gen.get_grid();
}

auto RecursiveBacktracker::render_ascii(const std::vector<Cell> &grid, size_t rows,
                           size_t columns) -> std::string {
    std::ostringstream out;

    for (size_t r_idx = 0; r_idx < rows; ++r_idx) {
        for (size_t c_idx = 0; c_idx < columns; ++c_idx) {
            out << "+"
                << (grid[MazeUtils::get_1d_index(r_idx, c_idx, columns)].top
                        ? "---"
                        : "   ");
        }
        out << "+\n";

        for (size_t c_idx = 0; c_idx < columns; ++c_idx) {
            out << (grid[MazeUtils::get_1d_index(r_idx, c_idx, columns)].left
                        ? "|"
                        : " ");
            out << "   ";
        }
        out << "|\n";
    }

    for (size_t c_idx = 0; c_idx < columns; ++c_idx) {
        out << "+"
            << (grid[MazeUtils::get_1d_index(rows - 1, c_idx, columns)].bottom
                    ? "---"
                    : "   ");
    }
    out << "+\n";

    return out.str();
}

