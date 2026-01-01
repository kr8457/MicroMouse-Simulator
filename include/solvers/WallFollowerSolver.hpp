#ifndef MICRO_MOUSE_WALL_FOLLOWER_SOLVER_HPP
#define MICRO_MOUSE_WALL_FOLLOWER_SOLVER_HPP

#include "SolverInterface.hpp"
#include <vector>

class WallFollowerSolver : public SolverInterface {
public:
    virtual void solve_instant() override;

    void initialize(const std::vector<Cell>& grid, 
                    const Graph& graph, 
                    size_t start, 
                    size_t end, 
                    size_t rows, 
                    size_t cols) override;

    bool step() override;
    std::vector<size_t> get_path() const override;
    std::vector<size_t> get_visited_order() const override;
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

#endif // MICRO_MOUSE_WALL_FOLLOWER_SOLVER_HPP
