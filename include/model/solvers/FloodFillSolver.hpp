#pragma once

#include "SolverInterface.hpp"
#include <vector>
#include <queue>

class FloodFillSolver : public SolverInterface {
public:
    /**
     * @brief Initializes the Flood Fill solver state.
     * @param grid Maze grid.
     * @param graph Adjacency list.
     * @param start 1D target start node (where the agent starts).
     * @param end 1D goal node (distance 0).
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
     * @brief Advances the solver (either flooding distances or following the gradient).
     * @return True if solver animation should continue.
     */
    bool step() override;

    /**
     * @brief Returns the gradient path from start to end.
     * @return Vector of node indices.
     */
    std::vector<size_t> get_path() const override;

    /**
     * @brief Returns visited order during the flooding phase.
     * @return Vector of node indices.
     */
    std::vector<size_t> get_visited_order() const override;

    /**
     * @brief Checks if the agent reached the goal.
     * @return True if solved.
     */
    bool is_solved() const override;

    /**
     * @brief Returns the calculated distance field for visualization.
     * @return Vector of integers mapping one-to-one with grid cells.
     */
    std::vector<int> get_grid_values() const override { return distances_; }

private:
    const std::vector<Cell>* grid_ = nullptr;
    const Graph* graph_ = nullptr;
    size_t start_node_ = 0;
    size_t end_node_ = 0;
    size_t rows_ = 0;
    size_t cols_ = 0;
    size_t total_nodes_ = 0;

    std::queue<size_t> work_queue_;
    std::vector<int> distances_; 
    
    std::vector<size_t> visited_order_;
    bool solved_ = false;
    bool finished_ = false;
    bool flooding_phase_ = true;
};

