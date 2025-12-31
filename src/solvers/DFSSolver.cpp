#include "solvers/DFSSolver.hpp"
#include <algorithm>

void DFSSolver::initialize(const std::vector<Cell>& /*grid*/, 
                           const Graph& graph, 
                           size_t start, 
                           size_t end, 
                           size_t rows, 
                           size_t cols) {
    graph_ = &graph;
    start_node_ = start;
    end_node_ = end;
    total_nodes_ = rows * cols;

    while(!work_stack_.empty()) work_stack_.pop();
    visited_.assign(total_nodes_, false);
    predecessors_.assign(total_nodes_, static_cast<size_t>(-1));
    visited_order_.clear();
    solved_ = false;
    finished_ = false;

    if (total_nodes_ > 0) {
        work_stack_.push(start_node_);
        visited_[start_node_] = true;
        visited_order_.push_back(start_node_);
        
        if (start_node_ == end_node_) {
            solved_ = true;
            finished_ = true;
        }
    } else {
        finished_ = true;
    }
}

bool DFSSolver::step() {
    if (finished_) return false;
    if (work_stack_.empty()) {
        finished_ = true;
        return false;
    }

    // DFS processes the top of stack
    const size_t CURRENT_NODE = work_stack_.top();

    if (CURRENT_NODE == end_node_) {
        solved_ = true;
        finished_ = true;
        return false;
    }

    // Find one unvisited neighbor
    bool found_unvisited_neighbor = false;
    for (const size_t NEIGHBOR : (*graph_)[CURRENT_NODE]) {
        if (!visited_[NEIGHBOR]) {
            visited_[NEIGHBOR] = true;
            predecessors_[NEIGHBOR] = CURRENT_NODE;
            work_stack_.push(NEIGHBOR);
            visited_order_.push_back(NEIGHBOR);
            found_unvisited_neighbor = true;
            break; // Depth first: explore this immediately
        }
    }

    if (!found_unvisited_neighbor) {
        work_stack_.pop(); // Backtrack
    }

    return true;
}

std::vector<size_t> DFSSolver::get_path() const {
    if (!solved_) return {};

    // Standard path reconstruction logic
    std::vector<size_t> path;
    size_t current_path_node = end_node_;
    
    // Safety check for loops or disconnected components (though predecessors shouldn't have loops)
    size_t safety_ctr = 0;
    while (current_path_node != static_cast<size_t>(-1) && safety_ctr++ < total_nodes_) {
        path.push_back(current_path_node);
        if (predecessors_[current_path_node] == current_path_node) break; // Should not happen with -1 init
        current_path_node = predecessors_[current_path_node];
    }
    std::reverse(path.begin(), path.end());
    return path;
}

std::vector<size_t> DFSSolver::get_visited_order() const {
    return visited_order_;
}

bool DFSSolver::is_solved() const {
    return solved_;
}
