/**
 * @file Simulator.hpp
 * @brief Header for the Simulator class, managing the MicroMouse simulation
 * state and logic.
 */

#pragma once

#include "generators/GeneratorInterface.hpp"
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
 * @enum GeneratorType
 * @brief Identifies the available maze-generation algorithms.
 */
enum class GeneratorType : std::uint8_t {
    RECURSIVE = 0,
    ELLERS    = 1
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
     * @param event The event to process (mouse clicks, key presses, etc.).
     */
    auto handle_event(sf::RenderWindow &window, const sf::Event &event) -> void;

    /**
     * @brief Updates the simulation state based on elapsed time.
     * 
     * Handles animation timing for both maze generation and solving.
     * @param delta_time Time elapsed since last update in seconds.
     */
    auto update(float delta_time) -> void;

    /**
     * @brief Renders the current simulation state to the window.
     * 
     * Draws the maze grid, paths, and UI sidebar.
     * @param window The render window to draw to.
     */
    auto render(sf::RenderWindow &window) -> void;

    /**
     * @brief Updates the stored mouse position relative to the render window.
     * @param window The render window.
     */
    auto update_mouse_position(sf::RenderWindow &window) -> void;

  private:
    // Simulation constants
    static constexpr size_t DEFAULT_ROWS       = 16;   ///< Default number of rows for a new maze.
    static constexpr size_t DEFAULT_COLS       = 16;   ///< Default number of columns for a new maze.
    static constexpr int    DEFAULT_ANIM_SPEED = 100;  ///< Default animation speed in milliseconds per step.

    // Simulation state
    size_t              maze_rows_    = DEFAULT_ROWS;  ///< Current number of maze rows.
    size_t              maze_cols_    = DEFAULT_COLS;  ///< Current number of maze columns.
    size_t              display_rows_ = DEFAULT_ROWS;  ///< Target number of rows for the next generation.
    size_t              display_cols_ = DEFAULT_COLS;  ///< Target number of columns for the next generation.
    
    std::unique_ptr<GeneratorInterface> maze_gen_;     ///< Instance of the current maze generator.
    std::vector<Cell>   maze_grid_;                    ///< The 2D grid of maze cells.
    Graph               maze_graph_;                   ///< Graph representation of the maze for solving.
    std::vector<size_t> solved_path_;                  ///< Path found by the solver.
    std::vector<size_t> exploration_path_;             ///< List of nodes explored by the solver.
    
    bool                is_generating_ = false;        ///< Whether a maze is currently being generated.
    bool                is_solving_    = false;        ///< Whether a solver is currently running.
    bool                is_paused_     = true;         ///< Whether the solving animation is paused.
    size_t              anim_step_idx_ = 0;            ///< Current step in the solver animation.
    int                 anim_speed_    = DEFAULT_ANIM_SPEED; ///< Current animation speed.
    
    SolverType          solver_type_   = SolverType::BFS;       ///< Currently selected solver algorithm.
    GeneratorType       gen_type_      = GeneratorType::RECURSIVE; ///< Currently selected generator algorithm.
    std::unique_ptr<SolverInterface> solver_instance_; ///< Instance of the current maze solver.
    
    sf::Clock           anim_clock_;                   ///< Clock used for animation timing.
    sf::Vector2f        mouse_pos_;                    ///< Current mouse position in window coordinates.
    UI                  ui_;                           ///< UI manager instance.
    float               time_accumulator_ = 0.0F;      ///< Accumulates time for animation stepping.

    // Zoom and Pan state
    float               zoom_factor_   = 1.0f;         ///< Current camera zoom level.
    sf::Vector2f        camera_offset_ = {0.0f, 0.0f}; ///< Current camera pan offset.
    bool                is_panning_    = false;        ///< Whether the user is currently panning the camera.
    sf::Vector2i        last_mouse_pos_;               ///< Last recorded mouse position during panning.

    /**
     * @brief Initiates the selected maze-solving algorithm.
     */
    auto run_solver() -> void;

    /**
     * @brief Initiates the selected maze-generation algorithm.
     * @param step_by_step If true, enables animated generation.
     */
    auto start_generation(bool step_by_step) -> void;

    /**
     * @brief Resets the simulation to its initial state.
     */
    auto reset_simulator() -> void;

    /**
     * @brief Handles mouse clicks for instant maze solving.
     * @param mouse_pos The mouse position in world coordinates.
     */
    auto handle_instant_solve_click(const sf::Vector2f &mouse_pos) -> void;

    /**
     * @brief Performs a single step in the solver animation.
     */
    auto step_solver_animation() -> void;

    /**
     * @brief Toggles a wall at the specified cell boundary.
     * @param row Cell row index.
     * @param col Cell column index.
     * @param wall_side The side of the cell where the wall is located.
     */
    auto toggle_wall(size_t row, size_t col, int wall_side) -> void;

    /**
     * @brief Saves the current maze configuration to a file.
     */
    auto save_maze() -> void;

    /**
     * @brief Loads a maze configuration from a file.
     */
    auto load_maze() -> void;
};

