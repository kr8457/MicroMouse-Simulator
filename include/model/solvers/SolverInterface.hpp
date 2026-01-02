#pragma once

#include "MazeUtils.hpp"

class SolverInterface {
public:
    virtual ~SolverInterface() = default;

    /**
     * @brief Initializes the solver state with necessary data.
     * 
     * @param grid The collection of maze cells (wall configuration).
     * @param graph Adjacency list representation of traversable paths.
     * @param start 1D index of the starting cell.
     * @param end 1D index of the target destination cell.
     * @param rows Number of rows in the maze.
     * @param cols Number of columns in the maze.
     */
    virtual void initialize(const std::vector<Cell>& grid, 
                            const Graph& graph, 
                            size_t start, 
                            size_t end, 
                            size_t rows, 
                            size_t cols) = 0;

    /**
     * @brief Advances the solving algorithm by exactly one step.
     * 
     * This method is intended for visualization of the search process.
     * @return True if the algorithm is still actively searching, 
     *         false if it has finished (either found the path or exhausted all options).
     */
    virtual bool step() = 0;

    /**
     * @brief Runs the algorithm to completion immediately without visualization.
     */
    virtual void solve_instant() {
        while(step());
    }

    /**
     * @brief Retrieves the path found by the solver.
     * 
     * If the solver is still running, this returns the path found so far.
     * @return Vector of 1D node indices representing the path.
     */
    virtual std::vector<size_t> get_path() const = 0;

    /**
     * @brief Retrieves the chronological order of nodes explored by the solver.
     * @return Vector of 1D node indices in the order they were visited.
     */
    virtual std::vector<size_t> get_visited_order() const = 0;

    /**
     * @brief Checks if the target cell has been successfully reached.
     * @return True if a path to the goal exists and was found.
     */
    virtual bool is_solved() const = 0;

    /**
     * @brief Provides algorithm-specific values for cell visualization.
     * 
     * For example, Flood Fill uses this to return distances from the goal.
     * @return Vector of integers mapped one-to-one with grid cells. Empty if not used.
     */
    virtual std::vector<int> get_grid_values() const { return {}; }

    /**
     * @brief Gets the current direction the solver agent is facing.
     * @return An integer representing direction (0: North, 1: East, 2: South, 3: West, -1: Undefined).
     */
    virtual int get_current_heading() const { return -1; }
};

