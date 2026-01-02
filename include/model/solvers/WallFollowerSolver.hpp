#pragma once

#include "SolverInterface.hpp"
#include <vector>

class WallFollowerSolver : public SolverInterface {
public:
    /**
     * @brief Solves the maze instantly by following the right-hand rule until the exit is reached.
     */
    virtual void solve_instant() override;

    /**
     * @brief Initializes the solver with the maze grid and start/end points.
     * @param grid Grid of cells for wall checks.
     * @param graph Adjacency list (not primary for wall follower, but kept for interface).
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
     * @brief Performs one step of the wall-following algorithm.
     * @return True if still searching, false if solved or failed.
     */
    bool step() override;

    /**
     * @brief Gets the path taken by the agent.
     * @return Vector of node indices.
     */
    std::vector<size_t> get_path() const override;

    /**
     * @brief Gets the order in which nodes were visited.
     * @return Vector of node indices.
     */
    std::vector<size_t> get_visited_order() const override;

    /**
     * @brief Checks if the solver has reached the target.
     * @return True if solved, false otherwise.
     */
    bool is_solved() const override;

    int get_current_heading() const override { return facing_; }

private:
    const std::vector<Cell>* grid_ = nullptr;
    size_t start_node_ = 0;
    size_t end_node_ = 0;
    size_t rows_ = 0;
    size_t cols_ = 0;

    size_t current_node_ = 0;
    int facing_ = 0; // 0:Up, 1:Right, 2:Down, 3:Left
    
    std::vector<size_t> path_; // Stores the path taken
    std::vector<size_t> visited_order_;
    
    bool solved_ = false;
    bool finished_ = false;

    // Cycle detection: tracks {node, facing} states
    std::vector<bool> visited_states_;
};

