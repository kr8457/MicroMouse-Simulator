#pragma once

#include "SolverInterface.hpp"
#include <stack>
#include <vector>

class DFSSolver : public SolverInterface {
public:
    /**
     * @brief Initializes the DFS solver state.
     * @param grid Maze grid.
     * @param graph Adjacency list.
     * @param start 1D start node.
     * @param end 1D end node.
     * @param rows Total rows.
     * @param cols Total columns.
     */
    void initialize(const std::vector<Cell>& grid, 
                    const Graph& graph, 
                    size_t start, 
                    size_t end, 
                    size_t rows, 
                    size_t cols) override;

    /**
     * @brief Performs one step of Depth-First Search.
     * @return True if continuing, false if finished.
     */
    bool step() override;

    /**
     * @brief Returns the path found by DFS.
     * @return Vector of node indices.
     */
    std::vector<size_t> get_path() const override;

    /**
     * @brief Returns the order of node visits.
     * @return Vector of node indices.
     */
    std::vector<size_t> get_visited_order() const override;

    /**
     * @brief Checks if the target was found.
     * @return True if solved, false otherwise.
     */
    bool is_solved() const override;

private:
    const Graph* graph_ = nullptr;
    size_t start_node_ = 0;
    size_t end_node_ = 0;
    size_t total_nodes_ = 0;

    std::stack<size_t> work_stack_;
    std::vector<bool> visited_;
    std::vector<size_t> predecessors_;
    
    std::vector<size_t> visited_order_;
    
    bool solved_ = false;
    bool finished_ = false;
};

