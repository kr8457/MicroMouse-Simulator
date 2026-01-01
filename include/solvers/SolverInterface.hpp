#ifndef MICRO_MOUSE_SOLVER_INTERFACE_HPP
#define MICRO_MOUSE_SOLVER_INTERFACE_HPP

#include "MazeGen.hpp" // For Cell
#include <vector>
#include <cstddef>

// Forward decl or typedef for Graph if strictly needed, 
// but usually it's std::vector<std::vector<size_t>>
using Graph = std::vector<std::vector<size_t>>;

class SolverInterface {
public:
    virtual ~SolverInterface() = default;

    /**
     * @brief Initialize the solver state.
     */
    virtual void initialize(const std::vector<Cell>& grid, 
                            const Graph& graph, 
                            size_t start, 
                            size_t end, 
                            size_t rows, 
                            size_t cols) = 0;

    /**
     * @brief Advance the algorithm by one step.
     * @return true if the algorithm is still running, false if finished (found path or failed).
     */
    virtual bool step() = 0;

    /**
     * @brief Run to completion immediately.
     */
    virtual void solve_instant() {
        while(step());
    }

    /**
     * @brief Get the path found so far (or final path if finished).
     */
    virtual std::vector<size_t> get_path() const = 0;

    /**
     * @brief Get the list of all visited nodes in order (for visuals).
     */
    virtual std::vector<size_t> get_visited_order() const = 0;

    /**
     * @brief Check if the target has been reached.
     */
    virtual bool is_solved() const = 0;

    /**
     * @brief Get grid values for visualization (e.g. flood fill distances).
     * @return Vector of integers mapping one-to-one with grid cells. Empty if not applicable.
     */
    virtual std::vector<int> get_grid_values() const { return {}; }

    /**
     * @brief Get the current heading of the solver agent.
     * @return 0: Up, 1: Right, 2: Down, 3: Left, -1: Undefined/None.
     */
    virtual int get_current_heading() const { return -1; }
};

#endif // MICRO_MOUSE_SOLVER_INTERFACE_HPP
