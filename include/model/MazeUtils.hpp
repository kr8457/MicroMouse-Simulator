#pragma once

#include <cstddef>
#include <utility>
#include <vector>

/**
 * @struct Cell
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
 * @brief Adjacency list representation of the maze graph.
 */
using Graph = std::vector<std::vector<size_t>>;

/**
 * @class MazeUtils
 * @brief Provides static utility methods for coordinate conversion and graph transformation.
 */
class MazeUtils {
public:
    /**
     * @brief Converts 2D (row, col) coordinates to a 1D index for linear storage.
     * 
     * Uses the formula: (row * columns) + col.
     * @param row The row index (0-indexed).
     * @param col The column index (0-indexed).
     * @param columns The total number of columns in the grid.
     * @return The 1D index corresponding to the 2D coordinates.
     */
    static inline auto get_1d_index(size_t row, size_t col, size_t columns) -> size_t {
        return (row * columns) + col;
    }

    /**
     * @brief Converts a 1D index back to 2D (row, col) coordinates.
     * @param node_idx The linear index to convert.
     * @param columns The total number of columns in the grid.
     * @return A pair containing {row, col}.
     */
    static inline auto get_2d_coords(size_t node_idx, size_t columns) -> std::pair<size_t, size_t> {
        return {node_idx / columns, node_idx % columns};
    }

    /**
     * @brief Converts the 2D maze grid (vector of Cells) into an adjacency list graph.
     * 
     * Analyzes wall boundaries for each cell to determine traversable neighbors.
     * @param grid The linear vector of maze cells.
     * @param rows Total number of rows in the maze.
     * @param columns Total number of columns in the maze.
     * @return An adjacency list where each entry contains a list of reachable neighbor indices.
     */
    static auto convert_to_graph(const std::vector<Cell>& grid, size_t rows, size_t columns) -> Graph;
};

