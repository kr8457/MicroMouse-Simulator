/**
 * @file Simulator.hpp
 * @brief Header for the Simulator class, managing the MicroMouse simulation
 * state and logic.
 */

#ifndef MICRO_MOUSE_SIMULATOR_HPP
#define MICRO_MOUSE_SIMULATOR_HPP

#include "MazeGen.hpp"
#include "MazeSolver.hpp"
#include "UI.hpp"
#include "solvers/SolverInterface.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <memory>

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
 * @class Simulator
 * @brief Manages the complete simulation state and orchestrates maze
 * generation, solving, and animation.
 */
class Simulator {
  public:
    /**
     * @brief Constructs a new Simulator with default settings.
     */
    Simulator();

    /**
     * @brief Initializes the simulator and loads required resources.
     * @return True if initialization succeeded, false otherwise.
     */
    auto initialize() -> bool;

    /**
     * @brief Processes SFML events and updates simulation state.
     * @param window The render window to handle events for.
     * @param event The event to process.
     */
    auto handle_event(sf::RenderWindow &window, const sf::Event &event) -> void;

    /**
     * @brief Updates the simulation state (animation stepping).
     * @param delta_time Time elapsed since last update in seconds.
     */
    auto update(float delta_time) -> void;

    /**
     * @brief Renders the current simulation state to the window.
     * @param window The render window to draw to.
     */
    auto render(sf::RenderWindow &window) -> void;

    /**
     * @brief Gets the current mouse position in window coordinates.
     * @param window The render window.
     */
    auto update_mouse_position(sf::RenderWindow &window) -> void;

  private:
    // Simulation constants
    static constexpr size_t K_DEFAULT_ROWS       = 16;
    static constexpr size_t K_DEFAULT_COLS       = 16;
    static constexpr int    K_DEFAULT_ANIM_SPEED = 100;

    // Simulation state
    size_t              maze_rows_    = K_DEFAULT_ROWS;
    size_t              maze_cols_    = K_DEFAULT_COLS;
    size_t              display_rows_ = K_DEFAULT_ROWS;
    size_t              display_cols_ = K_DEFAULT_COLS;
    MazeGen             maze_gen_;
    std::vector<Cell>   maze_grid_;
    Graph               maze_graph_;
    std::vector<size_t> solved_path_;
    std::vector<size_t> exploration_path_;
    bool                is_generating_ = false;
    bool                is_solving_    = false;
    bool                is_paused_     = true;
    size_t              anim_step_idx_ = 0;
    // double last_solve_ms_ = 0.0; // Removed
    int                 anim_speed_    = K_DEFAULT_ANIM_SPEED;
    SolverType          solver_type_   = SolverType::BFS;
    std::unique_ptr<SolverInterface> solver_instance_;
    
    sf::Clock           anim_clock_;
    sf::Vector2f        mouse_pos_;
    UI                  ui_;
    float               time_accumulator_ = 0.0F;

    // Private methods
    auto run_solver() -> void;
    auto start_generation(bool step_by_step) -> void;
    auto reset_simulator() -> void;
    auto handle_instant_solve_click(const sf::Vector2f &mouse_pos) -> void;
    auto step_solver_animation() -> void;
};

#endif  // MICRO_MOUSE_SIMULATOR_HPP
