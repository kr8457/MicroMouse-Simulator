#pragma once

#include "SolverInterface.hpp"
#include <vector>
#include <queue>
#include <functional>
#include <limits>

class AStarSolver : public SolverInterface {
public:
    /**
     * @brief Initializes the A* solver state.
     * @param grid Maze grid.
     * @param graph Adjacency list for pathfinding.
     * @param start 1D start index.
     * @param end 1D goal index.
     * @param rows Number of rows.
     * @param cols Number of columns.
     */
    void initialize(const std::vector<Cell>& grid, 
                    const Graph& graph, 
                    size_t start, 
                    size_t end, 
                    size_t rows, 
                    size_t cols) override;

    /**
     * @brief Performs one iteration of the A* algorithm (extracting and expanding one node).
     * @return True if search is ongoing, false if target found or queue empty.
     */
    bool step() override;

    /**
     * @brief Reconstructs and returns the optimal path found by A*.
     * @return Vector of node indices.
     */
    std::vector<size_t> get_path() const override;

    /**
     * @brief Returns the order of nodes explored during search.
     * @return Vector of node indices.
     */
    std::vector<size_t> get_visited_order() const override;

    /**
     * @brief Checks if the target cell was reached.
     * @return True if path found, false otherwise.
     */
    bool is_solved() const override;

private:
    const Graph* graph_ = nullptr;
    size_t start_node_ = 0;
    size_t end_node_ = 0;
    size_t rows_ = 0;
    size_t cols_ = 0;
    size_t total_nodes_ = 0;

    using NodeDist = std::pair<double, size_t>;
    std::priority_queue<NodeDist, std::vector<NodeDist>, std::greater<>> priority_q_;
    
    std::vector<double> g_score_;
    std::vector<size_t> predecessors_;
    
    std::vector<size_t> visited_order_;
    bool solved_ = false;
    bool finished_ = false;
};

