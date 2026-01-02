/**
 * @file EllersGenerator.cpp
 * @brief Implementation of Eller's maze generation algorithm.
 */

#include "generators/EllersGenerator.hpp"
#include <algorithm>
#include <map>


void EllersGenerator::initialize(size_t rows, size_t cols) {
    rows_ = rows;
    cols_ = cols;
    grid_.assign(rows * cols, Cell());
    sets_.assign(cols, 0); // 0 means unassigned
    next_set_id_ = 1;
    current_row_ = 0;
    done_ = (rows == 0 || cols == 0);
    initialized_ = true;
    state_ = State::HORIZONTAL;
    rng_ = std::mt19937(std::random_device{}());
    current_row_cells_.clear();
}

auto EllersGenerator::step() -> bool {
    if (done_ || !initialized_) return false;

    current_row_cells_.clear();
    for (size_t c = 0; c < cols_; ++c) {
        current_row_cells_.emplace_back(static_cast<int>(current_row_), static_cast<int>(c));
    }

    if (current_row_ == rows_ - 1) {
        assign_sets();
        finalize_last_row();
        done_ = true;
        
        // Ensure starting and ending points are open
        grid_[MazeUtils::get_1d_index(0, 0, cols_)].top = false;
        grid_[MazeUtils::get_1d_index(rows_ - 1, cols_ - 1, cols_)].bottom = false;
        
        return false;
    }

    assign_sets();
    horizontal_connections();
    vertical_connections();
    prepare_next_row();

    current_row_++;
    return true;
}

void EllersGenerator::assign_sets() {
    for (size_t c = 0; c < cols_; ++c) {
        if (sets_[c] == 0) {
            sets_[c] = next_set_id_++;
        }
    }
}

void EllersGenerator::horizontal_connections() {
    std::uniform_int_distribution<int> dist(0, 1);
    for (size_t c = 0; c < cols_ - 1; ++c) {
        // If they are in the same set, we MUST NOT join them (would create loop)
        // If they are in different sets, we randomly join them
        if (sets_[c] != sets_[c + 1]) {
            if (dist(rng_) == 1) {
                // Join them
                int old_set = sets_[c + 1];
                int new_set = sets_[c];
                for (size_t i = 0; i < cols_; ++i) {
                    if (sets_[i] == old_set) sets_[i] = new_set;
                }
                
                size_t idx1 = MazeUtils::get_1d_index(current_row_, c, cols_);
                size_t idx2 = MazeUtils::get_1d_index(current_row_, c + 1, cols_);
                grid_[idx1].right = false;
                grid_[idx2].left = false;
            }
        }
    }
}

void EllersGenerator::vertical_connections() {
    std::uniform_int_distribution<int> dist(0, 1);
    
    // Group columns by their set IDs
    std::map<int, std::vector<size_t>> set_members;
    for (size_t c = 0; c < cols_; ++c) {
        set_members[sets_[c]].push_back(c);
    }

    std::vector<int> next_row_sets(cols_, 0);

    for (auto const& [set_id, members] : set_members) {
        // Each set must have at least one vertical connection
        // We ensure this by picking at least one member randomly
        std::vector<size_t> shuffled_members = members;
        std::shuffle(shuffled_members.begin(), shuffled_members.end(), rng_);

        bool any_connection = false;
        for (size_t c : shuffled_members) {
            // Randomly decide to create a vertical connection
            // But if it's the last member and no connection has been made, we MUST make it.
            if (dist(rng_) == 1 || (!any_connection && c == shuffled_members.back())) {
                any_connection = true;
                
                size_t idx_curr = MazeUtils::get_1d_index(current_row_, c, cols_);
                size_t idx_next = MazeUtils::get_1d_index(current_row_ + 1, c, cols_);
                
                grid_[idx_curr].bottom = false;
                grid_[idx_next].top = false;
                next_row_sets[c] = set_id;
            }
        }
    }
    
    sets_ = next_row_sets;
}

void EllersGenerator::finalize_last_row() {
    for (size_t c = 0; c < cols_ - 1; ++c) {
        if (sets_[c] != sets_[c + 1]) {
            // Join them
            int old_set = sets_[c + 1];
            int new_set = sets_[c];
            for (size_t i = 0; i < cols_; ++i) {
                if (sets_[i] == old_set) sets_[i] = new_set;
            }
            
            size_t idx1 = MazeUtils::get_1d_index(current_row_, c, cols_);
            size_t idx2 = MazeUtils::get_1d_index(current_row_, c + 1, cols_);
            grid_[idx1].right = false;
            grid_[idx2].left = false;
        }
    }
}

void EllersGenerator::prepare_next_row() {
    // This is handled at the end of vertical_connections() by updating sets_
    // Unassigned cells in the next row will be given new set IDs by assign_sets() in the next step.
}

