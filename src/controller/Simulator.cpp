/**
 * @file Simulator.cpp
 * @brief Implementation of the Simulator class for the MicroMouse simulator.
 */
#include "Simulator.hpp"
#include "MazeUtils.hpp"
#include "generators/EllersGenerator.hpp"
#include "generators/RecursiveBacktracker.hpp"
#include "solvers/AStarSolver.hpp"
#include "solvers/BFSSolver.hpp"
#include "solvers/DFSSolver.hpp"
#include "solvers/FloodFillSolver.hpp"
#include "solvers/WallFollowerSolver.hpp"
#include <iostream>
#include <fstream>
#ifdef _WIN32
#include <windows.h>
#include <commdlg.h>
#endif

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
    const int PREV_SOLVER        = static_cast<int>(solver_type_);
    int       current_solver_idx = static_cast<int>(solver_type_);
    int       current_gen_idx    = static_cast<int>(gen_type_);

    ui_.handle_event(
        window, event, maze_rows_, maze_cols_, display_rows_, display_cols_,
        is_generating_, is_solving_, is_paused_, current_solver_idx,
        current_gen_idx, anim_speed_,
        [this](bool step_by_step) { start_generation(step_by_step); },
        [this]() { run_solver(); }, [this]() { step_solver_animation(); },
        [this]() { reset_simulator(); },
        [this](size_t r, size_t c, int s) { toggle_wall(r, c, s); },
        [this]() { save_maze(); }, [this]() { load_maze(); }, zoom_factor_,
        camera_offset_);

    solver_type_ = static_cast<SolverType>(current_solver_idx);
    gen_type_    = static_cast<GeneratorType>(current_gen_idx);

    // If solver changed while not solving, clear screen status
    if (static_cast<int>(solver_type_) != PREV_SOLVER) {
        is_solving_ = false;
        solved_path_.clear();
        exploration_path_.clear();
        solver_instance_.reset();
    }

    // Handle Instant Solve (Skip Animation)
    if (event.type == sf::Event::MouseButtonPressed) {
        const sf::Vector2f MPOS_VAL =
            window.mapPixelToCoords(sf::Mouse::getPosition(window));
        handle_instant_solve_click(MPOS_VAL);
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

    const float THRESHOLD  = 1.0F / static_cast<float>(anim_speed_);
    time_accumulator_     += delta_time;

    if (time_accumulator_ >= THRESHOLD) {
        time_accumulator_ = 0.0F;

        if (is_generating_ && maze_gen_) {
            const bool STILL_RUNNING = maze_gen_->step();
            // Update grid after each step so UI can display changes
            maze_grid_ = maze_gen_->get_grid();

            if (!STILL_RUNNING) {
                maze_graph_ = MazeUtils::convert_to_graph(
                    maze_grid_, maze_rows_, maze_cols_);
                // Clear visited flag so the generation "orange" highlight disappears
                for (auto& cell : maze_grid_) {
                    cell.visited = false;
                }
                is_generating_ = false;
            }
        } else if (is_solving_) {
            if (solver_instance_) {
                bool running      = solver_instance_->step();
                exploration_path_ = solver_instance_->get_visited_order();

                // Update the "Ribbon" path live for Wall Follower so the user
                // sees the trail
                if (solver_type_ == SolverType::WALL) {
                    solved_path_ = solver_instance_->get_path();
                }

                if (!running) {
                    is_solving_  = false;
                    solved_path_ = solver_instance_->get_path();
                }
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
    std::vector<int> grid_values;
    int              heading = -1;

    if (solver_instance_) {
        grid_values = solver_instance_->get_grid_values();
        heading     = solver_instance_->get_current_heading();
    }

    ui_.draw(window, maze_rows_, maze_cols_, display_rows_, display_cols_,
             maze_grid_, exploration_path_, solved_path_, is_generating_,
             is_solving_, is_paused_, static_cast<int>(solver_type_),
             static_cast<int>(gen_type_), anim_speed_, *maze_gen_, 
             mouse_pos_, zoom_factor_, camera_offset_,
             grid_values, heading);
}

auto Simulator::update_mouse_position(sf::RenderWindow &window) -> void {
    mouse_pos_ = window.mapPixelToCoords(sf::Mouse::getPosition(window));
}

// ==========================================================
//                  PRIVATE METHODS
// ==========================================================

auto Simulator::run_solver() -> void {
    if (is_generating_ || is_solving_) return;

    const size_t START_NODE = 0;
    const size_t END_NODE   = (maze_rows_ * maze_cols_) - 1;

    // Reset UI state
    solved_path_.clear();
    exploration_path_.clear();
    anim_step_idx_ = 0;

    // Instantiate Solver
    switch (solver_type_) {
    case SolverType::BFS:
        solver_instance_ = std::make_unique<BFSSolver>();
        break;
    case SolverType::DFS:
        solver_instance_ = std::make_unique<DFSSolver>();
        break;
    case SolverType::ASTAR:
        solver_instance_ = std::make_unique<AStarSolver>();
        break;
    case SolverType::FLOODFILL:
        solver_instance_ = std::make_unique<FloodFillSolver>();
        break;
    case SolverType::WALL:
        solver_instance_ = std::make_unique<WallFollowerSolver>();
        break;
    }

    if (solver_instance_) {
        solver_instance_->initialize(maze_grid_, maze_graph_, START_NODE,
                                     END_NODE, maze_rows_, maze_cols_);
        // FIX: Update exploration path immediately so the start node (and head)
        // are visible even before the first simulation step occurs.
        exploration_path_ = solver_instance_->get_visited_order();
    }

    is_solving_    = true;
    is_paused_     = false;
    anim_step_idx_ = 0;
}

auto Simulator::start_generation(bool step_by_step) -> void {
    is_solving_ = false;
    solved_path_.clear();
    exploration_path_.clear();
    solver_instance_.reset();

    // Select generator based on gen_type_
    if (gen_type_ == GeneratorType::RECURSIVE) {
        maze_gen_ = std::make_unique<RecursiveBacktracker>();
    } else {
        maze_gen_ = std::make_unique<EllersGenerator>();
    }
    maze_gen_->initialize(maze_rows_, maze_cols_);

    // FIX: Update grid immediately after initialization so the first state is
    // visible
    maze_grid_ = maze_gen_->get_grid();

    is_generating_ = true;
    is_paused_     = !step_by_step;

    if (!step_by_step) {
        // Instant generation
        while (maze_gen_->step()) { }
        maze_grid_ = maze_gen_->get_grid();
        // Clear visited flag for instant generation too
        for (auto& cell : maze_grid_) {
            cell.visited = false;
        }
        maze_graph_ =
            MazeUtils::convert_to_graph(maze_grid_, maze_rows_, maze_cols_);
        is_generating_ = false;
    }
}

auto Simulator::reset_simulator() -> void {
    maze_rows_     = DEFAULT_ROWS;
    maze_cols_     = DEFAULT_COLS;
    display_rows_  = DEFAULT_ROWS;
    display_cols_  = DEFAULT_COLS;
    is_generating_ = false;
    is_solving_    = false;
    is_paused_     = true;
    zoom_factor_   = 1.0f;
    camera_offset_ = {0.0f, 0.0f};
    solved_path_.clear();
    exploration_path_.clear();
    solver_instance_.reset();

    start_generation(false);
}

auto Simulator::handle_instant_solve_click(const sf::Vector2f &mouse_pos)
    -> void {
    if (ui_.get_layout().solve_inst_btn.contains(mouse_pos)) {
        if (solver_instance_ && is_solving_) {
            solver_instance_->solve_instant();
            exploration_path_ = solver_instance_->get_visited_order();
            solved_path_      = solver_instance_->get_path();
            is_solving_       = false;
            is_paused_        = true;
        }
    }
}

auto Simulator::step_solver_animation() -> void {
    if (!is_solving_ || !solver_instance_) return;

    bool running      = solver_instance_->step();
    exploration_path_ = solver_instance_->get_visited_order();

    if (!running) {
        is_solving_  = false;
        solved_path_ = solver_instance_->get_path();
    }
    is_paused_ = true;
}

auto Simulator::toggle_wall(size_t row, size_t col, int wall_side) -> void {
    if (is_generating_ || is_solving_) return;
    if (row >= maze_rows_ || col >= maze_cols_) return;

    const size_t IDX       = MazeUtils::get_1d_index(row, col, maze_cols_);
    bool         new_state = false;

    // Toggle the selected wall and its neighbor
    switch (wall_side) {
    case 0:  // Top
        maze_grid_[IDX].top = !maze_grid_[IDX].top;
        new_state           = maze_grid_[IDX].top;
        if (row > 0)
            maze_grid_[MazeUtils::get_1d_index(row - 1, col, maze_cols_)]
                .bottom = new_state;
        break;
    case 1:  // Right
        maze_grid_[IDX].right = !maze_grid_[IDX].right;
        new_state             = maze_grid_[IDX].right;
        if (col < maze_cols_ - 1) maze_grid_[IDX + 1].left = new_state;
        break;
    case 2:  // Bottom
        maze_grid_[IDX].bottom = !maze_grid_[IDX].bottom;
        new_state              = maze_grid_[IDX].bottom;
        if (row < maze_rows_ - 1)
            maze_grid_[MazeUtils::get_1d_index(row + 1, col, maze_cols_)].top =
                new_state;
        break;
    case 3:  // Left
        maze_grid_[IDX].left = !maze_grid_[IDX].left;
        new_state            = maze_grid_[IDX].left;
        if (col > 0) maze_grid_[IDX - 1].right = new_state;
        break;
    }

    maze_graph_ =
        MazeUtils::convert_to_graph(maze_grid_, maze_rows_, maze_cols_);

    // Clear any previous paths
    solved_path_.clear();
    exploration_path_.clear();
}

auto Simulator::save_maze() -> void {
#ifdef _WIN32
    OPENFILENAME ofn;
    char         szFile[260] = {0};

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize     = sizeof(ofn);
    ofn.hwndOwner       = NULL;
    ofn.lpstrFile       = szFile;
    ofn.nMaxFile        = sizeof(szFile);
    ofn.lpstrFilter     = "Maze Files\0*.maze\0All Files\0*.*\0";
    ofn.nFilterIndex    = 1;
    ofn.lpstrFileTitle  = NULL;
    ofn.nMaxFileTitle   = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT;

    if (GetSaveFileName(&ofn) == TRUE) {
        std::string path = szFile;
        // Append .maze if not present
        if (path.find(".maze") == std::string::npos) {
            path += ".maze";
        }

        std::ofstream file(path);
        if (file.is_open()) {
            file << maze_rows_ << " " << maze_cols_ << "\n";
            for (const auto &cell : maze_grid_) {
                int mask = 0;
                if (cell.top) mask |= 1;
                if (cell.right) mask |= 2;
                if (cell.bottom) mask |= 4;
                if (cell.left) mask |= 8;
                file << mask << " ";
            }
            std::cout << "Maze saved to: " << path << "\n";
        }
    }
#else
    std::cerr << "Save dialog only supported on Windows.\n";
#endif
}

auto Simulator::load_maze() -> void {
#ifdef _WIN32
    OPENFILENAME ofn;
    char         szFile[260] = {0};

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize     = sizeof(ofn);
    ofn.hwndOwner       = NULL;
    ofn.lpstrFile       = szFile;
    ofn.nMaxFile        = sizeof(szFile);
    ofn.lpstrFilter     = "Maze Files\0*.maze\0All Files\0*.*\0";
    ofn.nFilterIndex    = 1;
    ofn.lpstrFileTitle  = NULL;
    ofn.nMaxFileTitle   = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags           = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn) == TRUE) {
        std::ifstream file(szFile);
        if (file.is_open()) {
            size_t r, c;
            if (file >> r >> c) {
                maze_rows_    = r;
                maze_cols_    = c;
                display_rows_ = r;
                display_cols_ = c;
                maze_grid_.clear();
                maze_grid_.resize(r * c);

                for (size_t i = 0; i < r * c; ++i) {
                    int mask;
                    if (file >> mask) {
                        maze_grid_[i].top     = (mask & 1);
                        maze_grid_[i].right   = (mask & 2);
                        maze_grid_[i].bottom  = (mask & 4);
                        maze_grid_[i].left    = (mask & 8);
                        maze_grid_[i].visited = false;
                    }
                }

                maze_graph_ = MazeUtils::convert_to_graph(
                    maze_grid_, maze_rows_, maze_cols_);
                is_solving_ = false;
                solved_path_.clear();
                exploration_path_.clear();
                solver_instance_.reset();

                std::cout << "Maze loaded from: " << szFile << "\n";
            }
        }
    }
#else
    std::cerr << "Load dialog only supported on Windows.\n";
#endif
}
