/**
 * @file Simulator.cpp
 * @brief Implementation of the Simulator class for the MicroMouse simulator.
 */

#include "Simulator.hpp"
#include <chrono>
#include <iostream>

// ==========================================================
//                  CONSTRUCTOR & INITIALIZATION
// ==========================================================

Simulator::Simulator() = default;

auto Simulator::initialize() -> bool {
    if (!ui_.load_resources()) {
        std::cerr << "CRITICAL: Could not load any system font. App cannot "
                     "start.\n";
        return false;
    }

    // Generate initial maze
    start_generation(false);
    return true;
}

// ==========================================================
//                  EVENT HANDLING
// ==========================================================

auto Simulator::handle_event(sf::RenderWindow &window, const sf::Event &event)
    -> void {
    // Let UI handle sidebar clicks
    const int prev_solver        = static_cast<int>(solver_type_);
    int       current_solver_idx = static_cast<int>(solver_type_);

    ui_.handle_event(
        window, event, maze_rows_, maze_cols_, display_rows_, display_cols_,
        is_generating_, is_solving_, is_paused_, current_solver_idx,
        anim_speed_,
        [this](bool step_by_step) { start_generation(step_by_step); },
        [this]() { run_solver(); },
        []() { /* Single Step logic could be here */ },
        [this]() { reset_simulator(); });

    solver_type_ = static_cast<SolverType>(current_solver_idx);

    // If solver changed while not solving, clear screen status
    if (static_cast<int>(solver_type_) != prev_solver) {
        is_solving_ = false;
        solved_path_.clear();
    }

    // Handle Instant Solve (Skip Animation)
    if (event.type == sf::Event::MouseButtonPressed) {
        const sf::Vector2f mpos_val =
            window.mapPixelToCoords(sf::Mouse::getPosition(window));
        handle_instant_solve_click(mpos_val);
    }
}

// ==========================================================
//                  UPDATE & ANIMATION
// ==========================================================

auto Simulator::update(float delta_time) -> void {
    if (is_paused_) {
        time_accumulator_ = 0.0F;
        return;
    }

    const float threshold  = 1.0F / static_cast<float>(anim_speed_);
    time_accumulator_     += delta_time;

    if (time_accumulator_ >= threshold) {
        time_accumulator_ = 0.0F;

        if (is_generating_) {
            const bool still_running = maze_gen_.step();
            // Update grid after each step so UI can display changes
            maze_grid_ = maze_gen_.get_grid();

            if (!still_running) {
                maze_graph_ = MazeSolver::convert_to_graph(
                    maze_grid_, maze_rows_, maze_cols_);
                is_generating_ = false;
            }
        } else if (is_solving_) {
            if (anim_step_idx_ < solved_path_.size()) {
                anim_step_idx_++;
            } else {
                is_solving_ = false;
            }
        }
    }
}

// ==========================================================
//                  RENDERING
// ==========================================================

auto Simulator::render(sf::RenderWindow &window) -> void {
    ui_.draw(window, maze_rows_, maze_cols_, display_rows_, display_cols_,
             maze_grid_, solved_path_, anim_step_idx_, is_generating_,
             is_solving_, is_paused_, static_cast<int>(solver_type_),
             last_solve_ms_, anim_speed_, maze_gen_, mouse_pos_);
}

auto Simulator::update_mouse_position(sf::RenderWindow &window) -> void {
    mouse_pos_ = window.mapPixelToCoords(sf::Mouse::getPosition(window));
}

// ==========================================================
//                  PRIVATE METHODS
// ==========================================================

auto Simulator::run_solver() -> void {
    const size_t start_node = 0;
    const size_t end_node   = (maze_rows_ * maze_cols_) - 1;

    const auto t_start = std::chrono::high_resolution_clock::now();

    // Execute the chosen algorithm
    switch (solver_type_) {
    case SolverType::BFS:
        solved_path_ = MazeSolver::bfs_solve(maze_graph_, start_node, end_node,
                                             maze_rows_ * maze_cols_);
        break;
    case SolverType::DFS:
        solved_path_ = MazeSolver::dfs_solve(maze_graph_, start_node, end_node,
                                             maze_rows_ * maze_cols_);
        break;
    case SolverType::ASTAR:
        solved_path_ =
            MazeSolver::astar_solve(maze_graph_, start_node, end_node,
                                    maze_rows_ * maze_cols_, maze_cols_);
        break;
    case SolverType::FLOODFILL:
        solved_path_ = MazeSolver::flood_fill_solve(
            maze_grid_, maze_rows_, maze_cols_, start_node, end_node);
        break;
    case SolverType::WALL:
        solved_path_ = MazeSolver::wall_follower_solve(
            maze_grid_, maze_rows_, maze_cols_, start_node, end_node);
        break;
    }

    const auto t_end = std::chrono::high_resolution_clock::now();
    last_solve_ms_ =
        std::chrono::duration<double, std::milli>(t_end - t_start).count();

    // Prepare for animation
    anim_step_idx_ = 0;
    is_solving_    = true;
    is_paused_     = false;
}

auto Simulator::start_generation(bool step_by_step) -> void {
    is_solving_ = false;
    solved_path_.clear();
    maze_gen_.initialize(maze_rows_, maze_cols_);

    // FIX: Update grid immediately after initialization so the first state is
    // visible
    maze_grid_ = maze_gen_.get_grid();

    is_generating_ = true;
    is_paused_     = !step_by_step;

    if (!step_by_step) {
        // Instant generation
        while (maze_gen_.step()) { }
        maze_grid_ = maze_gen_.get_grid();
        maze_graph_ =
            MazeSolver::convert_to_graph(maze_grid_, maze_rows_, maze_cols_);
        is_generating_ = false;
    }
}

auto Simulator::reset_simulator() -> void {
    maze_rows_     = K_DEFAULT_ROWS;
    maze_cols_     = K_DEFAULT_COLS;
    display_rows_  = K_DEFAULT_ROWS;
    display_cols_  = K_DEFAULT_COLS;
    is_generating_ = false;
    is_solving_    = false;
    is_paused_     = true;
    solved_path_.clear();
    last_solve_ms_ = 0.0;
    start_generation(false);
}

auto Simulator::handle_instant_solve_click(const sf::Vector2f &mouse_pos)
    -> void {
    if (ui_.get_layout().solve_inst_btn.contains(mouse_pos)) {
        if (!solved_path_.empty()) {
            anim_step_idx_ = solved_path_.size();
            is_paused_     = true;
        }
    }
}
