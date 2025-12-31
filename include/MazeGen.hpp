#ifndef MICRO_MOUSE_MAZE_GEN_HPP
#define MICRO_MOUSE_MAZE_GEN_HPP

#include <cstddef>
#include <random>
#include <stack>
#include <string>
#include <utility>
#include <vector>

/**
 * @brief Represents a single cell in the maze grid.
 *        Stores wall states and visited status.
 */
struct Cell {
    bool top     = true;
    bool right   = true;
    bool bottom  = true;
    bool left    = true;
    bool visited = false;
};

/**
 * @class MazeGen
 * @brief Implements the Recursive Backtracker algorithm for perfect maze
 * generation. Supports both instant generation and step-by-step execution for
 * visualization.
 */
class MazeGen {
  public:
    MazeGen() = default; // ? is this needed?

    /**
     * @brief Initializes the maze generator state.
     * @param rows Number of rows in the maze.
     * @param columns Number of columns in the maze.
     */
    void initialize(size_t rows, size_t columns);

    /**
     * @brief Performs a single step of the maze generation algorithm.
     * @return True if the generation is still in progress, False if completed.
     */
    auto step() -> bool;

    /**
     * @brief Checks if the maze generation is finished.
     * @return True if finished, False otherwise.
     */
    auto is_done() const -> bool; // ? why is it needed when step() returns false when maze is completed

    /**
     * @brief Gets the constant reference to the current grid state.
     * @return Reference to the vector of Cells.
     */
    auto get_grid() const -> const std::vector<Cell> & { return grid_; }

    /**
     * @brief Gets the constant reference to the generation path (stack).
     * @return Reference to the vector of (row, col) pairs representing the
     * current path.
     */
    auto get_path() const -> const std::vector<std::pair<int, int>> & {
        return path_stack_;
    }

    /**
     * @brief Static helper to generate a full maze instantly.
     */
    static auto generate(size_t rows, size_t columns) -> std::vector<Cell>;

    static auto render_ascii(const std::vector<Cell> &grid, size_t rows,
                             size_t columns) -> std::string; // ? why is rows and columns needed when vector.size exists?

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

#endif  // MICRO_MOUSE_MAZE_GEN_HPP
