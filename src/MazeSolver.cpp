#include "MazeSolver.hpp"

#include <algorithm>
#include <iostream>
#include <queue>
#include <stack>
#include <vector>

size_t MazeSolver::get_1d_index(size_t row, size_t col, size_t columns) {
    return row * columns + col;
}

std::pair<size_t, size_t> MazeSolver::get_2d_coords(size_t node_idx,
                                                    size_t columns) {
    return {node_idx / columns, node_idx % columns};
}

Graph MazeSolver::convert_to_graph(const std::vector<Cell> &grid, size_t rows,
                                   size_t columns) {
    Graph graph(rows * columns);

    for (size_t r = 0; r < rows; ++r) {
        for (size_t c = 0; c < columns; ++c) {
            size_t      u_idx = get_1d_index(r, c, columns);
            const auto &cell  = grid[u_idx];

            // Check neighbors.
            if (!cell.top && r > 0) {
                size_t v_idx = get_1d_index(r - 1, c, columns);
                graph[u_idx].push_back(v_idx);
            }
            if (!cell.bottom && r < rows - 1) {
                size_t v_idx = get_1d_index(r + 1, c, columns);
                graph[u_idx].push_back(v_idx);
            }
            if (!cell.left && c > 0) {
                size_t v_idx = get_1d_index(r, c - 1, columns);
                graph[u_idx].push_back(v_idx);
            }
            if (!cell.right && c < columns - 1) {
                size_t v_idx = get_1d_index(r, c + 1, columns);
                graph[u_idx].push_back(v_idx);
            }
        }
    }
    return graph;
}

std::vector<size_t> MazeSolver::bfs_solve(const Graph &graph,
                                          size_t       start_node_idx,
                                          size_t       end_node_idx,
                                          size_t       total_nodes) {
    if (start_node_idx == end_node_idx) {
        return {start_node_idx};
    }

    std::queue<size_t>  q;
    std::vector<bool>   visited(total_nodes, false);
    std::vector<size_t> predecessors(total_nodes, static_cast<size_t>(-1));

    visited[start_node_idx] = true;
    q.push(start_node_idx);

    bool found = false;
    while (!q.empty()) {
        size_t u = q.front();
        q.pop();

        if (u == end_node_idx) {
            found = true;
            break;
        }

        for (size_t v : graph[u]) {
            if (!visited[v]) {
                visited[v]      = true;
                predecessors[v] = u;
                q.push(v);
            }
        }
    }

    if (!found) {
        return {};
    }

    // Reconstruct path
    std::vector<size_t> path;
    for (size_t curr = end_node_idx; curr != static_cast<size_t>(-1);
         curr        = predecessors[curr]) {
        path.push_back(curr);
    }
    std::reverse(path.begin(), path.end());
    return path;
}

std::vector<size_t> MazeSolver::dfs_solve(const Graph &graph,
                                          size_t       start_node_idx,
                                          size_t       end_node_idx,
                                          size_t       total_nodes) {
    if (start_node_idx == end_node_idx) {
        return {start_node_idx};
    }

    std::stack<size_t>  dfs_stack;
    std::vector<size_t> predecessors(total_nodes, static_cast<size_t>(-1));
    std::vector<bool>   visited(total_nodes, false);

    dfs_stack.push(start_node_idx);
    visited[start_node_idx] = true;

    while (!dfs_stack.empty()) {
        size_t current_node = dfs_stack.top();

        if (current_node == end_node_idx) {
            break;  // Destination found
        }

        // Use std::find_if to satisfy cppcheck? No, simple loop is better for
        // logic here. We need to keep this loop to find the *first* unvisited
        // neighbor. cppcheck-suppress useStlAlgorithm
        bool found_unvisited_neighbor = false;

        for (size_t neighbor : graph[current_node]) {
            if (!visited[neighbor]) {
                visited[neighbor]      = true;
                predecessors[neighbor] = current_node;
                dfs_stack.push(neighbor);
                found_unvisited_neighbor = true;
                break;
            }
        }

        if (!found_unvisited_neighbor) {
            dfs_stack.pop();  // Backtrack
        }
    }

    // Reconstruct path
    std::vector<size_t> path;
    if (predecessors[end_node_idx] == static_cast<size_t>(-1)) {
        return {};  // No path found
    }

    size_t current_path_node = end_node_idx;
    while (current_path_node != static_cast<size_t>(-1)) {
        path.push_back(current_path_node);
        if (predecessors[current_path_node] == current_path_node) break;
        current_path_node = predecessors[current_path_node];
    }
    std::reverse(path.begin(), path.end());
    return path;
}

std::vector<size_t>
MazeSolver::wall_follower_solve(const std::vector<Cell> &grid, size_t rows,
                                size_t cols, size_t start_node_idx,
                                size_t end_node_idx) {

    std::vector<size_t> path;
    size_t              curr = start_node_idx;
    path.push_back(curr);

    Direction facing = Direction::EAST;

    size_t max_steps = rows * cols * 4;
    size_t steps     = 0;

    while (curr != end_node_idx && steps < max_steps) {
        auto [r, c]      = get_2d_coords(curr, cols);
        const auto &cell = grid[curr];

        bool wall_front = false;
        bool wall_right = false;

        switch (facing) {
        case Direction::NORTH:
            wall_front = cell.top;
            wall_right = cell.right;
            break;
        case Direction::EAST:
            wall_front = cell.right;
            wall_right = cell.bottom;
            break;
        case Direction::SOUTH:
            wall_front = cell.bottom;
            wall_right = cell.left;
            break;
        case Direction::WEST:
            wall_front = cell.left;
            wall_right = cell.top;
            break;
        }

        if (!wall_right) {
            // Turn Right
            switch (facing) {
            case Direction::NORTH: facing = Direction::EAST; break;
            case Direction::EAST: facing = Direction::SOUTH; break;
            case Direction::SOUTH: facing = Direction::WEST; break;
            case Direction::WEST: facing = Direction::NORTH; break;
            }
        } else if (wall_front) {
            // Wall on right AND front. Turn Left.
            switch (facing) {
            case Direction::NORTH: facing = Direction::WEST; break;
            case Direction::EAST: facing = Direction::NORTH; break;
            case Direction::SOUTH: facing = Direction::EAST; break;
            case Direction::WEST: facing = Direction::SOUTH; break;
            }
            steps++;
            continue;
        }

        // Move
        int nr = static_cast<int>(r);
        int nc = static_cast<int>(c);

        switch (facing) {
        case Direction::NORTH: nr--; break;
        case Direction::EAST: nc++; break;
        case Direction::SOUTH: nr++; break;
        case Direction::WEST: nc--; break;
        }

        if (nr >= 0 &&
            nr < static_cast<int>(rows) &&
            nc >= 0 &&
            nc < static_cast<int>(cols)) {
            curr = get_1d_index(nr, nc, cols);
            path.push_back(curr);
        }

        steps++;
    }

    if (curr == end_node_idx) {
        return path;
    }
    return {};
}
