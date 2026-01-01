#ifndef MICRO_MOUSE_FLOOD_FILL_SOLVER_HPP
#define MICRO_MOUSE_FLOOD_FILL_SOLVER_HPP

#include "SolverInterface.hpp"
#include <vector>
#include <queue>

class FloodFillSolver : public SolverInterface {
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

#endif // MICRO_MOUSE_FLOOD_FILL_SOLVER_HPP
