/**
 * @file Main.cpp
 * @brief Entry point for the MicroMouse Simulator Pro.
 * @details Initializes the SFML environment, handles the main simulation loop,
 * and manages transitions between generation and solving states.
 */

#include "MazeGen.hpp"
#include "MazeSolver.hpp"
#include "UI.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

// Simulation constants
static constexpr int    K_WINDOW_WIDTH       = 1280;
static constexpr int    K_WINDOW_HEIGHT      = 720;
static constexpr size_t K_DEFAULT_ROWS       = 16;
static constexpr size_t K_DEFAULT_COLS       = 16;
static constexpr int    K_DEFAULT_ANIM_SPEED = 100;

/**
 * @enum SolverType
 * @brief Identifies the available maze-solving algorithms.
 */
enum class SolverType : std::uint8_t {
    BFS       = 0,
    DFS       = 1,
    ASTAR     = 2,
    FLOODFILL = 3,
    WALL      = 4
};

/**
 * @struct SimulationState
 * @brief Encapsulates all global simulation variables to respect
 *        cppcoreguidelines-avoid-non-const-global-variables.
 */
namespace {
struct SimulationState {
    size_t              maze_rows    = K_DEFAULT_ROWS;
    size_t              maze_cols    = K_DEFAULT_COLS;
    size_t              display_rows = K_DEFAULT_ROWS;
    size_t              display_cols = K_DEFAULT_COLS;
    MazeGen             maze_gen;
    std::vector<Cell>   maze_grid;
    Graph               maze_graph;
    std::vector<size_t> solved_path;
    bool                is_generating = false;
    bool                is_solving    = false;
    bool                is_paused     = true;
    size_t              anim_step_idx = 0;
    double              last_solve_ms = 0.0;
    int                 anim_speed    = K_DEFAULT_ANIM_SPEED;
    SolverType          solver_type   = SolverType::BFS;
    sf::Clock           anim_clock;
    sf::Vector2f        mouse_pos;
    UI                  ui;
};

SimulationState G_STATE;
}  // namespace

/**
 * @brief Core logic to run the selected solving algorithm.
 * @details Measures execution time and stores the resulting path in the state.
 */
auto run_solver() -> void {
    const size_t START_NODE = 0;
    const size_t END_NODE   = (G_STATE.maze_rows * G_STATE.maze_cols) - 1;

    const auto T_START = std::chrono::high_resolution_clock::now();

    // Execute the chosen algorithm
    switch (G_STATE.solver_type) {
    case SolverType::BFS:
        G_STATE.solved_path =
            MazeSolver::bfs_solve(G_STATE.maze_graph, START_NODE, END_NODE,
                                  G_STATE.maze_rows * G_STATE.maze_cols);
        break;
    case SolverType::DFS:
        G_STATE.solved_path =
            MazeSolver::dfs_solve(G_STATE.maze_graph, START_NODE, END_NODE,
                                  G_STATE.maze_rows * G_STATE.maze_cols);
        break;
    case SolverType::ASTAR:
        G_STATE.solved_path = MazeSolver::astar_solve(
            G_STATE.maze_graph, START_NODE, END_NODE,
            G_STATE.maze_rows * G_STATE.maze_cols, G_STATE.maze_cols);
        break;
    case SolverType::FLOODFILL:
        G_STATE.solved_path = MazeSolver::flood_fill_solve(
            G_STATE.maze_grid, G_STATE.maze_rows, G_STATE.maze_cols, START_NODE,
            END_NODE);
        break;
    case SolverType::WALL:
        G_STATE.solved_path = MazeSolver::wall_follower_solve(
            G_STATE.maze_grid, G_STATE.maze_rows, G_STATE.maze_cols, START_NODE,
            END_NODE);
        break;
    }

    const auto T_END = std::chrono::high_resolution_clock::now();
    G_STATE.last_solve_ms =
        std::chrono::duration<double, std::milli>(T_END - T_START).count();

    // Prepare for animation
    G_STATE.anim_step_idx = 0;
    G_STATE.is_solving    = true;
    G_STATE.is_paused     = false;
}

/**
 * @brief Prepares a new maze generation session.
 * @param step_by_step If true, enables visual stepping of the algorithm.
 */
auto start_generation(bool step_by_step) -> void {
    G_STATE.is_solving = false;
    G_STATE.solved_path.clear();
    G_STATE.maze_gen.initialize(G_STATE.maze_rows, G_STATE.maze_cols);
    G_STATE.is_generating = true;
    G_STATE.is_paused     = !step_by_step;

    if (!step_by_step) {
        while (G_STATE.maze_gen.step()) { }
        G_STATE.maze_grid  = G_STATE.maze_gen.get_grid();
        G_STATE.maze_graph = MazeSolver::convert_to_graph(
            G_STATE.maze_grid, G_STATE.maze_rows, G_STATE.maze_cols);
        G_STATE.is_generating = false;
    }
}

/**
 * @brief Resets the simulator to its initial state with defaults.
 */
auto reset_simulator() -> void {
    G_STATE.maze_rows     = K_DEFAULT_ROWS;
    G_STATE.maze_cols     = K_DEFAULT_COLS;
    G_STATE.display_rows  = K_DEFAULT_ROWS;
    G_STATE.display_cols  = K_DEFAULT_COLS;
    G_STATE.is_generating = false;
    G_STATE.is_solving    = false;
    G_STATE.is_paused     = true;
    G_STATE.solved_path.clear();
    G_STATE.last_solve_ms = 0.0;
    start_generation(false);
}

/**
 * @brief Application entry point.
 * @return 0 on successful exit.
 * @note Possible exceptions from SFML or std::cerr are caught by top-level
 * handler if present.
 */
auto main() -> int {
    sf::RenderWindow window(sf::VideoMode(K_WINDOW_WIDTH, K_WINDOW_HEIGHT),
                            "MicroMouse Simulator Pro",
                            sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    try {
        if (!G_STATE.ui.load_resources()) {
            std::cerr << "CRITICAL: Could not load any system font. App cannot "
                         "start.\n";
            return 1;
        }

        // Initial Maze
        start_generation(false);

        while (window.isOpen()) {
            sf::Event event{};
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }

                // Let UI handle sidebar clicks
                const int PREV_SOLVER  = static_cast<int>(G_STATE.solver_type);
                int current_solver_idx = static_cast<int>(G_STATE.solver_type);

                G_STATE.ui.handle_event(
                    window, event, G_STATE.maze_rows, G_STATE.maze_cols,
                    G_STATE.display_rows, G_STATE.display_cols,
                    G_STATE.is_generating, G_STATE.is_solving,
                    G_STATE.is_paused, current_solver_idx, G_STATE.anim_speed,
                    start_generation, run_solver,
                    []() { /* Single Step logic could be here */ },
                    reset_simulator);

                G_STATE.solver_type =
                    static_cast<SolverType>(current_solver_idx);

                // If solver changed while not solving, clear screen status
                if (static_cast<int>(G_STATE.solver_type) != PREV_SOLVER) {
                    G_STATE.is_solving = false;
                    G_STATE.solved_path.clear();
                }

                // Handle Instant Solve (Skip Animation)
                if (event.type == sf::Event::MouseButtonPressed) {
                    const sf::Vector2f MPOS_VAL =
                        window.mapPixelToCoords(sf::Mouse::getPosition(window));
                    if (G_STATE.ui.get_layout().solve_inst_btn.contains(
                            MPOS_VAL)) {
                        if (!G_STATE.solved_path.empty()) {
                            G_STATE.anim_step_idx = G_STATE.solved_path.size();
                            G_STATE.is_paused     = true;
                        }
                    }
                }
            }

            G_STATE.mouse_pos =
                window.mapPixelToCoords(sf::Mouse::getPosition(window));

            // Background logic for animation
            if (!G_STATE.is_paused) {
                float dt        = G_STATE.anim_clock.restart().asSeconds();
                float threshold = 1.0F / static_cast<float>(G_STATE.anim_speed);

                static float ACCUMULATOR  = 0.0F;
                ACCUMULATOR              += dt;

                if (ACCUMULATOR >= threshold) {
                    ACCUMULATOR = 0.0F;

                    if (G_STATE.is_generating) {
                        if (!G_STATE.maze_gen.step()) {
                            G_STATE.maze_grid  = G_STATE.maze_gen.get_grid();
                            G_STATE.maze_graph = MazeSolver::convert_to_graph(
                                G_STATE.maze_grid, G_STATE.maze_rows,
                                G_STATE.maze_cols);
                            G_STATE.is_generating = false;
                        }
                    } else if (G_STATE.is_solving) {
                        if (G_STATE.anim_step_idx <
                            G_STATE.solved_path.size()) {
                            G_STATE.anim_step_idx++;
                        } else {
                            G_STATE.is_solving = false;
                        }
                    }
                }
            } else {
                G_STATE.anim_clock.restart();
            }

            window.clear(Theme::Background);

            G_STATE.ui.draw(
                window, G_STATE.maze_rows, G_STATE.maze_cols,
                G_STATE.display_rows, G_STATE.display_cols, G_STATE.maze_grid,
                G_STATE.solved_path, G_STATE.anim_step_idx,
                G_STATE.is_generating, G_STATE.is_solving, G_STATE.is_paused,
                static_cast<int>(G_STATE.solver_type), G_STATE.last_solve_ms,
                G_STATE.anim_speed, G_STATE.maze_gen, G_STATE.mouse_pos);

            window.display();
        }
    } catch (const std::exception &e) {
        std::cerr << "EXCEPTION: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "UNKNOWN EXCEPTION CAUGHT\n";
        return 1;
    }

    return 0;
}
