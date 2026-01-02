#pragma once

#include <cstddef>
#include <random>
#include <stack>
#include <string>
#include <utility>
#include <vector>

#include "GeneratorInterface.hpp"
#include "MazeUtils.hpp"

/**
 * @class RecursiveBacktracker
 * @brief Implements the Recursive Backtracker algorithm for perfect maze
 * generation.
 */
class RecursiveBacktracker : public GeneratorInterface {
  public:
    RecursiveBacktracker() = default;

    /**
     * @brief Initializes the maze generator state.
     * @param rows Number of rows in the maze.
     * @param columns Number of columns in the maze.
     */
    void initialize(size_t rows, size_t columns) override;

    /**
     * @brief Performs a single step of the maze generation algorithm.
     * 
     * Uses a stack-based depth-first search to carve paths through the grid.
     * @return True if the generation is still in progress, False if completed.
     */
    auto step() -> bool override;

    /**
     * @brief Checks if the maze generation is finished.
     * @return True if finished, False otherwise.
     */
    auto is_done() const -> bool override;

    /**
     * @brief Gets a constant reference to the current grid state.
     * @return Reference to the vector of Cells.
     */
    auto get_grid() const -> const std::vector<Cell> & override { return grid_; }

    /**
     * @brief Gets a constant reference to the active recursion stack.
     * @return Reference to the vector of (row, col) pairs representing the current path.
     */
    auto get_path() const -> const std::vector<std::pair<int, int>> & override {
        return path_stack_;
    }

    /**
     * @brief Static helper to generate a complete perfect maze instantly.
     * @param rows Number of rows.
     * @param columns Number of columns.
     * @return A vector of Cells representing the finished maze.
     */
    static auto generate(size_t rows, size_t columns) -> std::vector<Cell>;

    /**
     * @brief Utility to render the maze in ASCII format for debugging.
     * @param grid The maze grid to render.
     * @param rows Total rows.
     * @param columns Total columns.
     * @return A string representation of the maze.
     */
    static auto render_ascii(const std::vector<Cell> &grid, size_t rows,
                             size_t columns) -> std::string;

  private:
    size_t                           rows_ = 0;
    size_t                           cols_ = 0;
    std::vector<Cell>                grid_;
    std::vector<std::pair<int, int>> path_stack_;
    bool                             initialized_ = false;
    bool                             done_        = true;
    std::mt19937                     rng_;

    // Helper to collect unvisited neighbors (internal use)
    auto get_neighbors(int row, int col) const
        -> std::vector<std::pair<int, int>>;
    // Helper to remove walls (internal use)
    void remove_walls(int row, int col, int nrow, int ncol);
};

