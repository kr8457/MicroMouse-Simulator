/**
 * @file MazeSolver.cpp
 * @brief Implementation of BFS, DFS, A*, Flood Fill, and Wall Follower
 * algorithms.
 */

#include "MazeSolver.hpp"
#include <algorithm>
#include <cmath>
#include <functional>
#include <queue>
#include <stack>
#include <utility>
#include <vector>

/**
 * @brief Converts 2D grid coordinates to a 1D index for linear layout.
 */
auto MazeSolver::get_1d_index(size_t row, size_t col, size_t columns)
    -> size_t {
    return (row * columns) + col;
}

/**
 * @brief Converts a 1D index back to 2D grid coordinates.
 */
auto MazeSolver::get_2d_coords(size_t node_idx, size_t columns)
    -> std::pair<size_t, size_t> {
    return {node_idx / columns, node_idx % columns};
}

/**
 * @brief Transforms the Cell grid into a Graph adjacency list.
 */
auto MazeSolver::convert_to_graph(const std::vector<Cell> &grid, size_t rows,
                                  size_t columns) -> Graph {
    Graph graph(rows * columns);

    for (size_t row_idx = 0; row_idx < rows; ++row_idx) {
        for (size_t col_idx = 0; col_idx < columns; ++col_idx) {
            const size_t U_IDX = get_1d_index(row_idx, col_idx, columns);
            const auto  &cell  = grid[U_IDX];

            // Add edges if no wall exists between cells.
            if (!cell.top && row_idx > 0) {
                const size_t V_IDX =
                    get_1d_index(row_idx - 1, col_idx, columns);
                graph[U_IDX].push_back(V_IDX);
            }
            if (!cell.bottom && row_idx < rows - 1) {
                const size_t V_IDX =
                    get_1d_index(row_idx + 1, col_idx, columns);
                graph[U_IDX].push_back(V_IDX);
            }
            if (!cell.left && col_idx > 0) {
                const size_t V_IDX =
                    get_1d_index(row_idx, col_idx - 1, columns);
                graph[U_IDX].push_back(V_IDX);
            }
            if (!cell.right && col_idx < columns - 1) {
                const size_t V_IDX =
                    get_1d_index(row_idx, col_idx + 1, columns);
                graph[U_IDX].push_back(V_IDX);
            }
        }
    }
    return graph;
}

/**
 * @brief Finds the shortest path in an unweighted graph using Breadth-First
 * Search.
 */
auto MazeSolver::bfs_solve(const Graph &graph, size_t start_node_idx,
                           size_t end_node_idx, size_t total_nodes)
    -> std::vector<size_t> {
    if (start_node_idx == end_node_idx) {
        return {start_node_idx};
    }

    std::queue<size_t>  work_queue;
    std::vector<bool>   visited(total_nodes, false);
    std::vector<size_t> predecessors(total_nodes, static_cast<size_t>(-1));

    visited[start_node_idx] = true;
    work_queue.push(start_node_idx);

    bool found = false;
    while (!work_queue.empty()) {
        const size_t U_NODE = work_queue.front();
        work_queue.pop();

        if (U_NODE == end_node_idx) {
            found = true;
            break;
        }

        for (const size_t V_NODE : graph[U_NODE]) {
            if (!visited[V_NODE]) {
                visited[V_NODE]      = true;
                predecessors[V_NODE] = U_NODE;
                work_queue.push(V_NODE);
            }
        }
    }

    if (!found) {
        return {};
    }

    // Backtrack from end to start to reconstruct path
    std::vector<size_t> path;
    for (size_t curr = end_node_idx; curr != static_cast<size_t>(-1);
         curr        = predecessors[curr]) {
        path.push_back(curr);
    }
    std::reverse(path.begin(), path.end());
    return path;
}

/**
 * @brief Finds a path using Depth-First Search. Note: Not necessarily the
 * shortest.
 */
auto MazeSolver::dfs_solve(const Graph &graph, size_t start_node_idx,
                           size_t end_node_idx, size_t total_nodes)
    -> std::vector<size_t> {
    if (start_node_idx == end_node_idx) {
        return {start_node_idx};
    }

    std::stack<size_t>  dfs_stack;
    std::vector<size_t> predecessors(total_nodes, static_cast<size_t>(-1));
    std::vector<bool>   visited(total_nodes, false);

    dfs_stack.push(start_node_idx);
    visited[start_node_idx] = true;

    while (!dfs_stack.empty()) {
        const size_t CURRENT_NODE = dfs_stack.top();

        if (CURRENT_NODE == end_node_idx) {
            break;  // Destination reached
        }

        bool found_unvisited_neighbor = false;
        for (const size_t NEIGHBOR : graph[CURRENT_NODE]) {
            if (!visited[NEIGHBOR]) {
                visited[NEIGHBOR]      = true;
                predecessors[NEIGHBOR] = CURRENT_NODE;
                dfs_stack.push(NEIGHBOR);
                found_unvisited_neighbor = true;
                break;  // Explore immediately
            }
        }

        if (!found_unvisited_neighbor) {
            dfs_stack.pop();  // Backtrack if no unvisited options
        }
    }

    if (predecessors[end_node_idx] == static_cast<size_t>(-1)) {
        return {};
    }

    std::vector<size_t> path;
    size_t              current_path_node = end_node_idx;
    while (current_path_node != static_cast<size_t>(-1)) {
        path.push_back(current_path_node);
        if (predecessors[current_path_node] == current_path_node) {
            break;
        }
        current_path_node = predecessors[current_path_node];
    }
    std::reverse(path.begin(), path.end());
    return path;
}

/**
 * @brief Implements the Left-Hand rule (Wall Follower) algorithm.
 */
auto MazeSolver::wall_follower_solve(const std::vector<Cell> &grid, size_t rows,
                                     size_t columns, size_t start_node_idx,
                                     size_t end_node_idx)
    -> std::vector<size_t> {

    std::vector<size_t> path;
    size_t              curr   = start_node_idx;
    int                 facing = 1;  // 0: Up, 1: Right, 2: Down, 3: Left

    path.push_back(curr);

    static constexpr size_t K_MAX_ROTATIONS = 4;
    const size_t            MAX_STEPS =
        rows * columns * K_MAX_ROTATIONS;  // Prevent infinite loops
    size_t steps = 0;

    while (curr != end_node_idx && steps < MAX_STEPS) {
        auto [r, c]      = get_2d_coords(curr, columns);
        const auto &cell = grid[curr];

        // Left-hand rule logic
        // Check and turn: Priority 1: Left, Priority 2: Front, Priority 3:
        // Right, Priority 4: Back
        bool moved = false;
        for (int i = -1; i <= 2; ++i) {
            int  check_dir = (facing + i + K_MAX_ROTATIONS) % 4;
            bool wall      = true;
            if (check_dir == 0) {
                wall = cell.top;
            } else if (check_dir == 1) {
                wall = cell.right;
            } else if (check_dir == 2) {
                wall = cell.bottom;
            } else if (check_dir == 3) {
                wall = cell.left;
            }

            if (!wall) {
                facing = check_dir;
                if (facing == 0) {
                    r--;
                } else if (facing == 1) {
                    c++;
                } else if (facing == 2) {
                    r++;
                } else if (facing == 3) {
                    c--;
                }

                curr = get_1d_index(r, c, columns);
                path.push_back(curr);
                moved = true;
                break;
            }
        }

        if (!moved) {
            break;
        }
        steps++;
    }

    return path;
}

/**
 * @brief Finds the shortest path using A* search with Manhattan distance
 * heuristic.
 */
auto MazeSolver::astar_solve(const Graph &graph, size_t start_node_idx,
                             size_t end_node_idx, size_t total_nodes,
                             size_t columns) -> std::vector<size_t> {

    auto heuristic = [&](size_t node_idx) -> double {
        auto [r1, c1] = get_2d_coords(node_idx, columns);
        auto [r2, c2] = get_2d_coords(end_node_idx, columns);
        return static_cast<double>(
            std::abs(static_cast<int>(r1) - static_cast<int>(r2)) +
            std::abs(static_cast<int>(c1) - static_cast<int>(c2)));
    };

    using NodeDist = std::pair<double, size_t>;
    std::priority_queue<NodeDist, std::vector<NodeDist>, std::greater<>>
        priority_q;

    std::vector<double> g_score(total_nodes,
                                std::numeric_limits<double>::infinity());
    std::vector<size_t> predecessors(total_nodes, static_cast<size_t>(-1));

    g_score[start_node_idx] = 0;
    priority_q.emplace(heuristic(start_node_idx), start_node_idx);

    while (!priority_q.empty()) {
        const size_t U_NODE = priority_q.top().second;
        priority_q.pop();

        if (U_NODE == end_node_idx) {
            break;
        }

        for (const size_t V_NODE : graph[U_NODE]) {
            const double TENTATIVE_G = g_score[U_NODE] + 1.0;
            if (TENTATIVE_G < g_score[V_NODE]) {
                predecessors[V_NODE] = U_NODE;
                g_score[V_NODE]      = TENTATIVE_G;
                priority_q.emplace(TENTATIVE_G + heuristic(V_NODE), V_NODE);
            }
        }
    }

    if (predecessors[end_node_idx] == static_cast<size_t>(-1)) {
        return {};
    }

    std::vector<size_t> path;
    for (size_t curr_node = end_node_idx; curr_node != static_cast<size_t>(-1);
         curr_node        = predecessors[curr_node]) {
        path.push_back(curr_node);
    }
    std::reverse(path.begin(), path.end());
    return path;
}

/**
 * @brief Flood Fill algorithm to calculate distances to target and find path.
 */
auto MazeSolver::flood_fill_solve(const std::vector<Cell> &grid, size_t rows,
                                  size_t columns, size_t start_node_idx,
                                  size_t end_node_idx) -> std::vector<size_t> {
    const size_t     total_nodes = rows * columns;
    std::vector<int> distances(total_nodes, static_cast<int>(total_nodes));
    Graph            graph = convert_to_graph(grid, rows, columns);

    std::queue<size_t> work_queue;
    distances[end_node_idx] = 0;
    work_queue.push(end_node_idx);

    while (!work_queue.empty()) {
        const size_t U_NODE = work_queue.front();
        work_queue.pop();

        for (const size_t V_NODE : graph[U_NODE]) {
            if (distances[V_NODE] > distances[U_NODE] + 1) {
                distances[V_NODE] = distances[U_NODE] + 1;
                work_queue.push(V_NODE);
            }
        }
    }

    if (distances[start_node_idx] == static_cast<int>(total_nodes)) {
        return {};
    }

    std::vector<size_t> path;
    size_t              curr = start_node_idx;
    path.push_back(curr);

    while (curr != end_node_idx) {
        size_t next_v = curr;
        int    min_d  = distances[curr];
        for (const size_t V_NODE : graph[curr]) {
            if (distances[V_NODE] < min_d) {
                min_d  = distances[V_NODE];
                next_v = V_NODE;
            }
        }
        if (next_v == curr) {
            break;
        }
        curr = next_v;
        path.push_back(curr);
    }
    return path;
}
