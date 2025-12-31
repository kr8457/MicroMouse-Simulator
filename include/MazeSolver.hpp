#ifndef MICRO_MOUSE_MAZE_SOLVER_HPP
#define MICRO_MOUSE_MAZE_SOLVER_HPP

#include "MazeGen.hpp"  // For Cell struct

#include <cstddef>
#include <cstdint>
#include <utility>  // For std::pair
#include <vector>

// Define a type for our graph representation (adjacency list)
// Each element in the outer vector corresponds to a node (cell index),
// and the inner vector contains the 1D indices of its direct neighbors.

using Graph = std::vector<std::vector<size_t>>;

/**
 * @namespace MazeSolver
 * @brief Provides static methods for converting a maze grid into a graph and
 * solving it using various pathfinding algorithms.
 */
class MazeSolver {
  public:
    /**
     * @brief Converts 2D (row,col) to 1D index.
     * @param row The row coordinate.
     * @param col The column coordinate.
     * @param columns The total number of columns in the grid.
     * @return The 1D index corresponding to the given 2D coordinates.
     */
    static auto get_1d_index(size_t row, size_t col, size_t columns) -> size_t; // ? no need for this to be here (not the MazeSolver's responsibility)

    /**
     * @brief Converts a 1D node index back to 2D (row, col) coordinates.
     * @param node_idx The 1D node index.
     * @param columns The number of columns in the maze.
     * @return A pair of size_t (row, col) coordinates.
     */
    static auto get_2d_coords(size_t node_idx, size_t columns)
        -> std::pair<size_t, size_t>;

    /**
     * @brief Converts the 2D maze grid (vector of Cells) into an adjacency list
     * graph. This graph represents the open paths between cells.
     * @param grid The 1D vector of Cell objects representing the maze.
     * @param rows The number of rows in the maze.
     * @param columns The number of columns in the maze.
     * @return An adjacency list (Graph) representation of the maze.
     */
    static auto convert_to_graph(const std::vector<Cell> &grid, size_t rows,
                                 size_t columns) -> Graph;

    /**
     * @brief Solves the maze using Breadth-First Search (BFS).
     *        Guaranteed to find the shortest path in terms of steps.
     * @param graph The adjacency list representation of the maze.
     * @param start_node_idx The 1D index of the starting cell.
     * @param end_node_idx The 1D index of the ending cell.
     * @param total_nodes The total number of nodes (cells) in the graph.
     * @return A vector of 1D node indices representing the shortest path from
     * start to end. Returns an empty vector if no path is found.
     */
    static auto bfs_solve(const Graph &graph, size_t start_node_idx,
                          size_t end_node_idx, size_t total_nodes)
        -> std::vector<size_t>;

    /**
     * @brief Solves the maze using BFS and also returns the exploration order.
     *        This shows all nodes visited during the search process.
     * @param graph The adjacency list representation of the maze.
     * @param start_node_idx The 1D index of the starting cell.
     * @param end_node_idx The 1D index of the ending cell.
     * @param total_nodes The total number of nodes (cells) in the graph.
     * @param exploration_order Output parameter - filled with nodes in visit order.
     * @return A vector of 1D node indices representing the shortest path.
     */
    static auto bfs_solve_with_exploration(const Graph &graph, 
                                           size_t start_node_idx,
                                           size_t end_node_idx, 
                                           size_t total_nodes,
                                           std::vector<size_t> &exploration_order)
        -> std::vector<size_t>;

    /**
     * @brief Solves the maze using Depth-First Search (DFS).
     *        Finds *a* path, but not necessarily the shortest.
     * @param graph The adjacency list representation of the maze.
     * @param start_node_idx The 1D index of the starting cell.
     * @param end_node_idx The 1D index of the ending cell.
     * @param total_nodes The total number of nodes (cells) in the graph.
     * @return A vector of 1D node indices representing a path from start to
     * end. Returns an empty vector if no path is found.
     */
    static auto dfs_solve(const Graph &graph, size_t start_node_idx,
                          size_t end_node_idx, size_t total_nodes)
        -> std::vector<size_t>;

    /**
     * @brief Solves the maze using the A* search algorithm.
     * @param graph The adjacency list representation of the maze.
     * @param start_node_idx The 1D index of the starting cell.
     * @param end_node_idx The 1D index of the ending cell.
     * @param total_nodes The total number of nodes in the graph.
     * @param columns The number of columns (for heuristic).
     * @return A vector of 1D node indices representing the shortest path.
     */
    static auto astar_solve(const Graph &graph, size_t start_node_idx,
                            size_t end_node_idx, size_t total_nodes,
                            size_t columns) -> std::vector<size_t>;

    /**
     * @brief Solves the maze using the Flood Fill algorithm (distance
     * transformed BFS).
     * @param grid The Cell grid representation.
     * @param rows Total rows.
     * @param columns Total columns.
     * @param start_node_idx The 1D index of the starting cell.
     * @param end_node_idx The 1D index of the ending cell.
     * @return A vector of 1D node indices representing the path.
     */
    static auto flood_fill_solve(const std::vector<Cell> &grid, size_t rows,
                                 size_t columns, size_t start_node_idx,
                                 size_t end_node_idx) -> std::vector<size_t>;

    /**
     * @brief Solves the maze using the Left-Hand Rule (Wall Follower
     * algorithm).
     * @param grid The 1D vector of Cell objects representing the maze.
     * @param rows The number of rows in the maze.
     * @param columns The number of columns in the maze.
     * @param start_node_idx The 1D index of the starting cell.
     * @param end_node_idx The 1D index of the ending cell.
     * @return A vector of 1D node indices representing the path taken.
     */
    static auto wall_follower_solve(const std::vector<Cell> &grid, size_t rows,
                                    size_t columns, size_t start_node_idx,
                                    size_t end_node_idx) -> std::vector<size_t>;
};

#endif  // MICRO_MOUSE_MAZE_SOLVER_HPP
