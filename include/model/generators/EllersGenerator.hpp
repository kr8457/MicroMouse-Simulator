#pragma once

#include "GeneratorInterface.hpp"
#include <random>
#include <vector>

/**
 * @class EllersGenerator
 * @brief Implements Eller's algorithm for row-by-row maze generation.
 * This algorithm is efficient (O(N) space) and allows for infinitely tall mazes.
 */
class EllersGenerator : public GeneratorInterface {
public:
    EllersGenerator() = default;

    /**
     * @brief Initializes the generator state for a new maze.
     * @param rows Total rows in the maze.
     * @param cols Total columns in the maze.
     */
    void initialize(size_t rows, size_t cols) override;

    /**
     * @brief Advances the algorithm by one step (processing horizontal/vertical connections).
     * @return True if more steps are required, false if the maze is finished.
     */
    auto step() -> bool override;

    /**
     * @brief Checks if the maze generation is complete.
     * @return True if done, false otherwise.
     */
    auto is_done() const -> bool override { return done_; }

    /**
     * @brief Gets a constant reference to the current grid state.
     * @return Reference to the vector of Cells.
     */
    auto get_grid() const -> const std::vector<Cell>& override { return grid_; }

    /**
     * @brief Gets the cells in the current row being processed.
     * @return Reference to a vector of (row, col) pairs.
     */
    auto get_path() const -> const std::vector<std::pair<int, int>>& override { return current_row_cells_; }

private:
    size_t rows_ = 0;
    size_t cols_ = 0;
    std::vector<Cell> grid_;
    bool done_ = true;
    bool initialized_ = false;

    // Algorithm state
    size_t current_row_ = 0;
    std::vector<int> sets_;  // Set ID for each cell in the current row
    int next_set_id_ = 1;
    std::mt19937 rng_;

    enum class State {
        HORIZONTAL,
        VERTICAL,
        FINAL_ROW,
        DONE
    };
    State state_ = State::HORIZONTAL;

    // For visualization
    std::vector<std::pair<int, int>> current_row_cells_;

    void assign_sets();
    void horizontal_connections();
    void vertical_connections();
    void finalize_last_row();
    void prepare_next_row();
};
