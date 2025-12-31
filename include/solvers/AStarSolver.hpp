#ifndef MICRO_MOUSE_ASTAR_SOLVER_HPP
#define MICRO_MOUSE_ASTAR_SOLVER_HPP

#include "SolverInterface.hpp"
#include <vector>
#include <queue>
#include <functional>
#include <limits>

class AStarSolver : public SolverInterface {
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

#endif // MICRO_MOUSE_ASTAR_SOLVER_HPP
