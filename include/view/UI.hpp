/**
 * @file UI.hpp
 * @brief Header for the UI class, managing rendering and user interaction for
 * the MicroMouse simulator.
 */

#pragma once

#include "generators/GeneratorInterface.hpp"
#include "view/UITheme.hpp"
#include "view/UIComponents.hpp"
#include <SFML/Graphics.hpp>
#include <functional>
#include <string>
#include <vector>

/**
 * @struct UILayout
 * @brief Stores the bounding boxes (sf::FloatRect) for all interactive UI
 * elements. Used to detect clicks and hover states.
 */
struct UILayout {
    sf::FloatRect gen_inst_btn;    ///< Quick Generate button
    sf::FloatRect gen_step_btn;    ///< Step-by-step Generate button
    sf::FloatRect recursive_gen_btn; ///< Recursive Backtracker selection
    sf::FloatRect ellers_gen_btn;    ///< Eller's algorithm selection
    sf::FloatRect play_btn;        ///< Resume/Pause button
    sf::FloatRect step_btn;        ///< Simulation step button
    sf::FloatRect bfs_btn;         ///< BFS solver selection
    sf::FloatRect dfs_btn;         ///< DFS solver selection
    sf::FloatRect astar_btn;       ///< A* solver selection
    sf::FloatRect flood_btn;       ///< Flood Fill solver selection
    sf::FloatRect wall_btn;        ///< Wall Follower solver selection
    sf::FloatRect solve_btn;       ///< Animate Solve button
    sf::FloatRect solve_inst_btn;  ///< Instant Solve button
    sf::FloatRect reset_btn;       ///< Reset Simulator button
    sf::FloatRect save_btn;        ///< Save Maze button
    sf::FloatRect load_btn;        ///< Load Maze button

    sf::FloatRect row_input_box; ///< Input box for Rows
    sf::FloatRect col_input_box; ///< Input box for Columns
    sf::FloatRect speed_input_box; ///< Input box for Speed
    sf::FloatRect apply_btn;     ///< Apply Dimensions button
};

/**
 * @class UI
 * @brief Orchestrates the simulation's GUI, including the sidebar, maze
 * rendering, and event dispatching.
 */
class UI {
  public:
    /** @brief Default constructor. */
    UI();

    /**
     * @brief Loads required assets (fonts) from system or local paths.
     * @return True if at least one font was loaded, false otherwise.
     */
    auto load_resources() -> bool;

    /**
     * @brief Renders the entire application interface.
     * 
     * Orchestrates the drawing of the sidebar and the maze area.
     * @param window The render window to draw to.
     * @param maze_rows Total rows in the maze grid.
     * @param maze_cols Total columns in the maze grid.
     * @param display_rows Rows to display in dimension inputs.
     * @param display_cols Columns to display in dimension inputs.
     * @param grid The collection of maze cells to render.
     * @param exploration_path Sequence of cells explored by the solver.
     * @param solved_path Final path found by the solver.
     * @param is_generating True if a generation animation is active.
     * @param is_solving True if a solver animation is active.
     * @param is_paused True if the simulation is currently paused.
     * @param solver_type Enum value of the active solver.
     * @param gen_type Enum value of the active generator.
     * @param animation_speed Current speed of the simulation in ms.
     * @param maze_gen Reference to the active generator for step-by-step visuals.
     * @param mouse_pos Current mouse position for hover effects.
     * @param zoom_factor Current camera zoom level.
     * @param camera_offset Current camera pan offset.
     * @param grid_values Optional numeric values to display in cells (e.g., for Flood Fill).
     * @param heading Optional direction for the mouse/robot (0=N, 1=E, 2=S, 3=W).
     */
    auto draw(sf::RenderWindow &window, size_t maze_rows, size_t maze_cols,
              size_t display_rows, size_t display_cols,
              const std::vector<Cell> &grid, 
              const std::vector<size_t> &exploration_path,
              const std::vector<size_t> &solved_path,
              bool is_generating, bool is_solving,
              bool is_paused, int solver_type, int gen_type,
              int animation_speed, const GeneratorInterface &maze_gen,
              const sf::Vector2f &mouse_pos,
              float zoom_factor, const sf::Vector2f &camera_offset,
              const std::vector<int>& grid_values = {},
              int heading = -1) -> void;

    /**
     * @brief Handles SFML events and dispatches simulation state updates.
     * 
     * Manages text input focus, button clicks, and camera manipulation.
     * @param window The render window.
     * @param event The event to process.
     * @param maze_rows [in,out] Current maze rows.
     * @param maze_cols [in,out] Current maze columns.
     * @param display_rows [in,out] Maze rows shown in UI text box.
     * @param display_cols [in,out] Maze columns shown in UI text box.
     * @param is_generating [in,out] Generation state.
     * @param is_solving [in,out] Solver state.
     * @param is_paused [in,out] Pause state.
     * @param solver_type [in,out] Selected solver index.
     * @param gen_type [in,out] Selected generator index.
     * @param animation_speed [in,out] Animation speed in ms.
     * @param start_gen Callback to initiate maze generation.
     * @param start_sol Callback to initiate maze solving.
     * @param step_fn Callback for simulation stepping.
     * @param reset_fn Callback to reset simulation.
     * @param toggle_wall_fn Callback to manually toggle cell walls.
     * @param save_fn Callback to save maze.
     * @param load_fn Callback to load maze.
     * @param zoom_factor [in,out] Camera zoom factor.
     * @param camera_offset [in,out] Camera pan offset.
     */
    auto handle_event(sf::RenderWindow &window, const sf::Event &event,
                      size_t &maze_rows, size_t &maze_cols,
                      size_t &display_rows, size_t &display_cols,
                      bool &is_generating, bool &is_solving, bool &is_paused,
                      int &solver_type, int &gen_type, int &animation_speed,
                      const std::function<void(bool)> &start_gen,
                      const std::function<void()>     &start_sol,
                      const std::function<void()>     &step_fn,
                      const std::function<void()>     &reset_fn,
                      const std::function<void(size_t, size_t, int)> &toggle_wall_fn,
                      const std::function<void()>     &save_fn,
                      const std::function<void()>     &load_fn,
                      float &zoom_factor, sf::Vector2f &camera_offset) -> void;

    /** @brief Gets the current UI layout bounding boxes. */
    auto get_layout() const -> const UILayout & { return layout_; }

  private:
    sf::Font font_;    ///< The font used for all UI text elements.
    UILayout layout_;  ///< Stores bounding boxes for all interactive UI elements.

    std::string row_input_buffer_; ///< Text buffer for the row dimension input field.
    std::string col_input_buffer_; ///< Text buffer for the column dimension input field.
    std::string speed_input_buffer_; ///< Text buffer for the animation speed input field.
    bool row_focused_ = false;     ///< Whether the row input field has focus.
    bool col_focused_ = false;     ///< Whether the column input field has focus.
    bool speed_focused_ = false;   ///< Whether the speed input field has focus.

    /**
     * @brief Renders the UI sidebar containing controls and statistics.
     * @param window The render window.
     * @param display_rows Current rows value in text box.
     * @param display_cols Current columns value in text box.
     * @param is_generating If generation is active.
     * @param is_solving If solving is active.
     * @param is_paused If simulation is paused.
     * @param solver_type Index of current solver.
     * @param gen_type Index of current generator.
     * @param exploration_count Number of nodes explored.
     * @param solved_count Number of nodes in final path.
     * @param animation_speed Current speed in ms.
     * @param mouse_pos Current mouse position.
     */
    auto draw_sidebar(sf::RenderWindow &window, size_t display_rows,
                      size_t display_cols, bool is_generating, bool is_solving,
                      bool is_paused, int solver_type, int gen_type,
                      size_t exploration_count, size_t solved_count,
                      int animation_speed,
                      const sf::Vector2f &mouse_pos) -> void;

    /**
     * @brief Renders the 2D maze grid with optional paths and values.
     * 
     * Handles centering, zoom, and panning logic for the maze viewport.
     * @param window The render window.
     * @param rows Total grid rows.
     * @param cols Total grid columns.
     * @param grid Grid of cells to draw.
     * @param exploration_path Path segments explored by the solver.
     * @param solved_path Final path segments found by the solver.
     * @param is_generating If generation is active.
     * @param maze_gen Reference to active generator for specific status.
     * @param grid_values Numeric values to display for certain solvers.
     * @param heading Direction indicator.
     * @param zoom_factor Current camera scale.
     * @param camera_offset Current camera pan.
     */
    auto draw_maze(sf::RenderWindow &window, size_t rows, size_t cols,
                   const std::vector<Cell>   &grid,
                   const std::vector<size_t> &exploration_path,
                   const std::vector<size_t> &solved_path,
                   bool is_generating,
                   const GeneratorInterface &maze_gen,
                   const std::vector<int>& grid_values,
                   int heading,
                   float zoom_factor, const sf::Vector2f &camera_offset) -> void;
};

