/**
 * @file MazeUtils.cpp
 * @brief Implementation of utility functions for maze operations.
 */

#include "MazeUtils.hpp"

auto MazeUtils::convert_to_graph(const std::vector<Cell>& grid, size_t rows, size_t columns) -> Graph {
    Graph graph(rows * columns);

    for (size_t row_idx = 0; row_idx < rows; ++row_idx) {
        for (size_t col_idx = 0; col_idx < columns; ++col_idx) {
            const size_t U_IDX = get_1d_index(row_idx, col_idx, columns);
            const auto& cell = grid[U_IDX];

            // Add edges if no wall exists between cells.
            if (!cell.top && row_idx > 0) {
                const size_t V_IDX = get_1d_index(row_idx - 1, col_idx, columns);
                graph[U_IDX].push_back(V_IDX);
            }
            if (!cell.bottom && row_idx < rows - 1) {
                const size_t V_IDX = get_1d_index(row_idx + 1, col_idx, columns);
                graph[U_IDX].push_back(V_IDX);
            }
            if (!cell.left && col_idx > 0) {
                const size_t V_IDX = get_1d_index(row_idx, col_idx - 1, columns);
                graph[U_IDX].push_back(V_IDX);
            }
            if (!cell.right && col_idx < columns - 1) {
                const size_t V_IDX = get_1d_index(row_idx, col_idx + 1, columns);
                graph[U_IDX].push_back(V_IDX);
            }
        }
    }
    return graph;
}

