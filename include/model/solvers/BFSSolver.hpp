#pragma once

#include "SolverInterface.hpp"
#include <queue>
#include <vector>

class BFSSolver : public SolverInterface {
public:
    /**
     * @brief Initializes the BFS solver with the maze graph.
     * @param grid Maze grid.
     * @param graph Adjacency list.
     * @param start 1D start index.
     * @param end 1D goal index.
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
     * @brief Performs one step of the Breadth-First Search.
     * @return True if search continues, false if target found or queue empty.
     */
    bool step() override;

    /**
     * @brief Reconstructs the shortest path found by BFS.
     * @return Vector of node indices.
     */
    std::vector<size_t> get_path() const override;

    /**
     * @brief Returns the order in which nodes were visited during BFS.
     * @return Vector of node indices.
     */
    std::vector<size_t> get_visited_order() const override;

    /**
     * @brief Checks if the target has been reached.
     * @return True if solved, false otherwise.
     */
    bool is_solved() const override;

private:
    const Graph* graph_ = nullptr;
    size_t start_node_ = 0;
    size_t end_node_ = 0;
    size_t total_nodes_ = 0;

    std::queue<size_t> work_queue_;
    std::vector<bool> visited_;
    std::vector<size_t> predecessors_;
    
    // For visualization
    std::vector<size_t> visited_order_;
    
    bool solved_ = false;
    bool finished_ = false; // True if queue empty or target found
};

