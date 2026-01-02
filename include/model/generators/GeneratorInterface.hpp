#pragma once

#include <vector>
#include <utility>
#include <cstddef>
#include "MazeUtils.hpp"

/**
 * @class GeneratorInterface
 * @brief Pure virtual interface for all maze generation algorithms.
 */
class GeneratorInterface {
public:
    virtual ~GeneratorInterface() = default;

    /**
     * @brief Initializes the maze generator state for a new grid.
     * 
     * Resets all internal structures and prepares the generator for a new run.
     * @param rows The number of rows in the maze grid.
     * @param cols The number of columns in the maze grid.
     */
    virtual void initialize(size_t rows, size_t cols) = 0;

    /**
     * @brief Performs a single step of the generation algorithm.
     * 
     * Used for real-time visualization of the generation process.
     * @return True if generation is still in progress, false if completed.
     */
    virtual auto step() -> bool = 0;

    /**
     * @brief Checks if the maze generation process has finished.
     * @return True if the generation is complete, false otherwise.
     */
    virtual auto is_done() const -> bool = 0;

    /**
     * @brief Gets the current state of the maze cells.
     * @return A constant reference to the grid of Cells.
     */
    virtual auto get_grid() const -> const std::vector<Cell>& = 0;

    /**
     * @brief Gets the current algorithm path or stack for visualization.
     * 
     * For Recursive Backtracker, this is the current recursion stack.
     * For Eller's, this might include the currently processed row's cells.
     * @return A vector of (row, col) pairs representing the active path.
     */
    virtual auto get_path() const -> const std::vector<std::pair<int, int>>& = 0;
};

