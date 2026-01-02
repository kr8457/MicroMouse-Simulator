/**
 * @file WallFollowerSolver.cpp
 * @brief Implementation of the Wall Follower algorithm for maze solving.
 */

#include "solvers/WallFollowerSolver.hpp"

#include "MazeUtils.hpp"

void WallFollowerSolver::initialize(const std::vector<Cell>& grid, 
                                    const Graph& /*graph*/, 
                                    size_t start, 
                                    size_t end, 
                                    size_t rows, 
                                    size_t cols) {
    grid_ = &grid;
    start_node_ = start;
    end_node_ = end;
    rows_ = rows;
    cols_ = cols;
    
    current_node_ = start;
    facing_ = 1; // Default to Right? Or heuristic based on start pos?
    // If we are at 0,0 (Top Left), facing Right is reasonable.
    
    path_.clear();
    visited_order_.clear();
    solved_ = false;
    finished_ = false;
    visited_states_.assign(rows * cols * 4, false);

    if (rows * cols > 0) {
        path_.push_back(start);
        visited_order_.push_back(start);
        
        if (start == end) {
            solved_ = true;
            finished_ = true;
        }
    } else {
        finished_ = true;
    }
}

bool WallFollowerSolver::step() {
    if (finished_) return false;
    if (current_node_ == end_node_) {
        solved_ = true;
        finished_ = true;
        return false;
    }

    // Cycle detection
    size_t state_idx = current_node_ * 4 + facing_;
    if (visited_states_[state_idx]) {
        finished_ = true; // Loop detected
        return false;
    }
    visited_states_[state_idx] = true;

    // Direction vectors for 0:Up, 1:Right, 2:Down, 3:Left
    // Row, Col deltas
    const int DR[] = {-1, 0, 1, 0};
    const int DC[] = {0, 1, 0, -1};

    // Helper to get next node in a direction
    auto get_next = [&](int dir) -> size_t {
        auto [r, c] = MazeUtils::get_2d_coords(current_node_, cols_);
        int nr = static_cast<int>(r) + DR[dir];
        int nc = static_cast<int>(c) + DC[dir];
        // Bounds check (though walls should prevent OOB)
         if (nr >= 0 && nr < (int)rows_ && nc >= 0 && nc < (int)cols_) {
             return MazeUtils::get_1d_index(nr, nc, cols_);
         }
         return current_node_;
    };

    // Walls: 0=Top, 1=Right, 2=Bottom, 3=Left
    const auto& c = (*grid_)[current_node_];
    bool walls[4];
    walls[0] = c.top;
    walls[1] = c.right;
    walls[2] = c.bottom;
    walls[3] = c.left;

    // Helper to check if a direction is truly open (no wall AND in bounds)
    auto is_open = [&](int dir) -> bool {
        // 1. Check Maze Walls
        if (walls[dir]) return false;

        // 2. Check Map Boundaries explicitly
        // If we are at the edge and facing out, it's blocked even if the cell says "no wall"
        auto [r, c] = MazeUtils::get_2d_coords(current_node_, cols_);
        if (dir == 0 && r == 0) return false;          // Up
        if (dir == 1 && c == cols_ - 1) return false;  // Right
        if (dir == 2 && r == rows_ - 1) return false;  // Down
        if (dir == 3 && c == 0) return false;          // Left
        
        return true;
    };

    // Left Hand Rule
    // 1. Check relative Left
    int left_dir = (facing_ + 3) % 4;
    
    if (is_open(left_dir)) {
        // Prepare to move Left
        facing_ = left_dir;
        current_node_ = get_next(facing_);
        path_.push_back(current_node_);
        visited_order_.push_back(current_node_);
        return true;
    }
    
    // 2. Check Forward
    if (is_open(facing_)) {
        // Move Forward
        current_node_ = get_next(facing_);
        path_.push_back(current_node_);
        visited_order_.push_back(current_node_);
        return true;
    }
    
    // 3. Turn Right (stay in cell)
    facing_ = (facing_ + 1) % 4;
    // We don't advance physical location, but we performed a step.
    // Ideally we should move in the same step if possible to avoid sticking?
    // But strict wall following allows turning.
    
    return true;
}

void WallFollowerSolver::solve_instant() {
    size_t limit = 10000; // Reduced limit, cycle detection handles most cases anyway
    while(step() && limit > 0) {
        limit--;
    }
    if (limit == 0 && !solved_) {
         finished_ = true;
    }
}

std::vector<size_t> WallFollowerSolver::get_path() const {
    return path_;
}

std::vector<size_t> WallFollowerSolver::get_visited_order() const {
    return visited_order_;
}

bool WallFollowerSolver::is_solved() const {
    return solved_;
}
