#include "solvers/AStarSolver.hpp"
#include "MazeSolver.hpp"
#include <cmath>
#include <algorithm>

void AStarSolver::initialize(const std::vector<Cell>& /*grid*/, 
                             const Graph& graph, 
                             size_t start, 
                             size_t end, 
                             size_t rows, 
                             size_t cols) {
    graph_ = &graph;
    start_node_ = start;
    end_node_ = end;
    rows_ = rows;
    cols_ = cols;
    total_nodes_ = rows * cols;

    while(!priority_q_.empty()) priority_q_.pop();
    
    g_score_.assign(total_nodes_, std::numeric_limits<double>::infinity());
    predecessors_.assign(total_nodes_, static_cast<size_t>(-1));
    visited_order_.clear();
    
    solved_ = false;
    finished_ = false;

    if (total_nodes_ > 0) {
        // Heuristic function: Manhattan distance
        auto heuristic = [&](size_t n) -> double {
            auto [r1, c1] = MazeSolver::get_2d_coords(n, cols_);
            auto [r2, c2] = MazeSolver::get_2d_coords(end_node_, cols_);
            return static_cast<double>(
                std::abs(static_cast<int>(r1) - static_cast<int>(r2)) +
                std::abs(static_cast<int>(c1) - static_cast<int>(c2)));
        };

        g_score_[start_node_] = 0.0;
        priority_q_.emplace(heuristic(start_node_), start_node_);
        visited_order_.push_back(start_node_); // Technically strictly visited only when popped?
        // A* "visits" when popping from PQ, but we often visualize "frontier" too.
        // Let's stick to "when popped" for strict consistency with BFS/DFS step.
    } else {
        finished_ = true;
    }
}

bool AStarSolver::step() {
    if (finished_) return false;
    if (priority_q_.empty()) {
        finished_ = true;
        return false;
    }

    const size_t U_NODE = priority_q_.top().second;
    priority_q_.pop();
    
    // We add to visited order here to signify "processing" this node
    // Optimization: avoid duplicates in visualization list if re-visited with lower cost (rare in unweighted consistent heuristic but possible)
    // Actually standard A* with monotonic heuristic doesn't re-pop.
    visited_order_.push_back(U_NODE);

    if (U_NODE == end_node_) {
        solved_ = true;
        finished_ = true;
        return false;
    }

    auto heuristic = [&](size_t n) -> double {
         auto [r1, c1] = MazeSolver::get_2d_coords(n, cols_);
         auto [r2, c2] = MazeSolver::get_2d_coords(end_node_, cols_);
         return static_cast<double>(
             std::abs(static_cast<int>(r1) - static_cast<int>(r2)) +
             std::abs(static_cast<int>(c1) - static_cast<int>(c2)));
    };

    for (const size_t V_NODE : (*graph_)[U_NODE]) {
        const double TENTATIVE_G = g_score_[U_NODE] + 1.0;
        if (TENTATIVE_G < g_score_[V_NODE]) {
            predecessors_[V_NODE] = U_NODE;
            g_score_[V_NODE] = TENTATIVE_G;
            priority_q_.emplace(TENTATIVE_G + heuristic(V_NODE), V_NODE);
        }
    }

    return true;
}

std::vector<size_t> AStarSolver::get_path() const {
    if (!solved_) return {};

    std::vector<size_t> path;
    for (size_t curr = end_node_; curr != static_cast<size_t>(-1);
         curr = predecessors_[curr]) {
        path.push_back(curr);
    }
    std::reverse(path.begin(), path.end());
    return path;
}

std::vector<size_t> AStarSolver::get_visited_order() const {
    return visited_order_;
}

bool AStarSolver::is_solved() const {
    return solved_;
}
