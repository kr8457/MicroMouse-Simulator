/**
 * @file FloodFillSolver.cpp
 * @brief Implementation of the Flood Fill algorithm for maze solving.
 */

#include "solvers/FloodFillSolver.hpp"

#include <algorithm>

void FloodFillSolver::initialize(const std::vector<Cell>& grid, 
                                 const Graph& graph, 
                                 size_t start, 
                                 size_t end, 
                                 size_t rows, 
                                 size_t cols) {
    grid_ = &grid;
    graph_ = &graph;
    start_node_ = start;
    end_node_ = end;
    rows_ = rows;
    cols_ = cols;
    total_nodes_ = rows * cols;
    
    std::queue<size_t> empty;
    std::swap(work_queue_, empty);
    distances_.assign(total_nodes_, static_cast<int>(total_nodes_));
    visited_order_.clear();
    solved_ = false;
    finished_ = false;
    flooding_phase_ = true;

    if (total_nodes_ > 0) {
        // Start flood from END node
        distances_[end_node_] = 0;
        work_queue_.push(end_node_);
        // visited_order_.push_back(end_node_); // Optional here, sticking to push-on-pop or push-on-push
    } else {
        finished_ = true;
    }
}

bool FloodFillSolver::step() {
    if (finished_) return false;

    if (work_queue_.empty()) {
        finished_ = true;
        // Check if start is reachable
        if (distances_[start_node_] != static_cast<int>(total_nodes_)) {
            solved_ = true;
        }
        return false;
    }

    const size_t U_NODE = work_queue_.front();
    work_queue_.pop();
    visited_order_.push_back(U_NODE);

    // Flood to neighbors
    for (const size_t V_NODE : (*graph_)[U_NODE]) {
        if (distances_[V_NODE] > distances_[U_NODE] + 1) {
            distances_[V_NODE] = distances_[U_NODE] + 1;
            work_queue_.push(V_NODE);
        }
    }

    // Optimization: If we reached start node, we technically "found" the path.
    // visualizing the full flood is better though.
    
    return true; 
}

std::vector<size_t> FloodFillSolver::get_path() const {
    if (!solved_ && distances_[start_node_] == static_cast<int>(total_nodes_)) return {};

    // Trace path from start to end by following decreasing distances
    // Note: If flood isn't complete but reached start, we can still trace.
    if (distances_[start_node_] == static_cast<int>(total_nodes_)) return {};

    std::vector<size_t> path;
    size_t curr = start_node_;
    path.push_back(curr);

    size_t safety = 0;
    while (curr != end_node_ && safety++ < total_nodes_) {
        size_t next_v = curr;
        int min_d = distances_[curr];
        
        for (const size_t V_NODE : (*graph_)[curr]) {
            if (distances_[V_NODE] < min_d) {
                min_d = distances_[V_NODE];
                next_v = V_NODE;
            }
        }
        
        if (next_v == curr) break; // Stuck?
        curr = next_v;
        path.push_back(curr);
    }
    return path;
}

std::vector<size_t> FloodFillSolver::get_visited_order() const {
    return visited_order_;
}

bool FloodFillSolver::is_solved() const {
    return solved_ || (distances_[start_node_] != static_cast<int>(total_nodes_));
}
