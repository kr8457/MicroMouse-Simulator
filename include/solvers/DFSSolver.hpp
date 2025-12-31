#ifndef MICRO_MOUSE_DFS_SOLVER_HPP
#define MICRO_MOUSE_DFS_SOLVER_HPP

#include "SolverInterface.hpp"
#include <stack>
#include <vector>

class DFSSolver : public SolverInterface {
public:
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

#endif // MICRO_MOUSE_DFS_SOLVER_HPP
