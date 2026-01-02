/**
 * @file BFSSolver.cpp
 * @brief Implementation of the Breadth-First Search (BFS) algorithm for maze solving.
 */

#include "solvers/BFSSolver.hpp"
#include <algorithm>

void BFSSolver::initialize(const std::vector<Cell>& /*grid*/, 
                           const Graph& graph, 
                           size_t start, 
                           size_t end, 
                           size_t rows, 
                           size_t cols) {
    graph_ = &graph;
    start_node_ = start;
    end_node_ = end;
    total_nodes_ = rows * cols;

    // Reset state
    std::queue<size_t> empty;
    std::swap(work_queue_, empty);
    visited_.assign(total_nodes_, false);
    predecessors_.assign(total_nodes_, static_cast<size_t>(-1));
    visited_order_.clear();
    solved_ = false;
    finished_ = false;

    // Initial setup
    if (total_nodes_ > 0) {
        visited_[start_node_] = true;
        work_queue_.push(start_node_);
        visited_order_.push_back(start_node_);
        
        // Edge case: start == end
        if (start_node_ == end_node_) {
            solved_ = true;
            finished_ = true;
        }
    } else {
        finished_ = true;
    }
}

bool BFSSolver::step() {
    if (finished_) return false;
    if (work_queue_.empty()) {
        finished_ = true;
        return false;
    }

    // Process one node from the queue
    const size_t U_NODE = work_queue_.front();
    work_queue_.pop();

    if (U_NODE == end_node_) {
        solved_ = true;
        finished_ = true;
        return false;
    }

    for (const size_t V_NODE : (*graph_)[U_NODE]) {
        if (!visited_[V_NODE]) {
            visited_[V_NODE] = true;
            predecessors_[V_NODE] = U_NODE;
            work_queue_.push(V_NODE);
            visited_order_.push_back(V_NODE);
        }
    }

    return true; // Still have work to do
}

std::vector<size_t> BFSSolver::get_path() const {
    if (!solved_) return {};

    std::vector<size_t> path;
    for (size_t curr = end_node_; curr != static_cast<size_t>(-1);
         curr = predecessors_[curr]) {
        path.push_back(curr);
    }
    std::reverse(path.begin(), path.end());
    return path;
}

std::vector<size_t> BFSSolver::get_visited_order() const {
    return visited_order_;
}

bool BFSSolver::is_solved() const {
    return solved_;
}


