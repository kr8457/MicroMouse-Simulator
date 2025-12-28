/**
 * @file UI.hpp
 * @brief Header for the UI class, managing rendering and user interaction for
 * the MicroMouse simulator.
 */

#ifndef MICRO_MOUSE_UI_HPP
#define MICRO_MOUSE_UI_HPP

#include "MazeGen.hpp"
#include "MazeSolver.hpp"
#include <SFML/Graphics.hpp>
#include <functional>
#include <string>
#include <vector>

/**
 * @namespace Theme
 * @brief Contains color definitions for consistent UI styling.
 */
namespace Theme {
extern const sf::Color Background;  ///< Application background color
extern const sf::Color Sidebar;     ///< Sidebar background color
extern const sf::Color Surface;     ///< UI element base color (buttons, cards)
extern const sf::Color Primary;     ///< Primary action color (Indigo)
extern const sf::Color PrimaryHover;  ///< Hover state for primary actions
extern const sf::Color Accent;        ///< Accent color (Rose)
extern const sf::Color TextMain;      ///< Main text color (White)
extern const sf::Color TextDim;       ///< Dimmed/secondary text color (Slate)
extern const sf::Color Success;       ///< Success/indicator color (Emerald)
extern const sf::Color PathColor;     ///< Color of the solved path ribbon
extern const sf::Color DotColor;      ///< Color of the path head marker
extern const sf::Color WallColor;     ///< Color of the maze walls
extern const sf::Color
    VisitedCell;  ///< Color for cells visited during exploration
}  // namespace Theme

/**
 * @struct UILayout
 * @brief Stores the bounding boxes (sf::FloatRect) for all interactive UI
 * elements. Used to detect clicks and hover states.
 */
struct UILayout {
    sf::FloatRect gen_inst_btn;    ///< Quick Generate button
    sf::FloatRect gen_step_btn;    ///< Step-by-step Generate button
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

    sf::FloatRect row_dec_btn;  ///< Decrease Rows spinner
    sf::FloatRect row_inc_btn;  ///< Increase Rows spinner
    sf::FloatRect col_dec_btn;  ///< Decrease Cols spinner
    sf::FloatRect col_inc_btn;  ///< Increase Cols spinner
    sf::FloatRect apply_btn;    ///< Apply Dimensions button

    sf::FloatRect speed_dec_btn;  ///< Decrease Animation Speed spinner
    sf::FloatRect speed_inc_btn;  ///< Increase Animation Speed spinner
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
     */
    auto draw(sf::RenderWindow &window, size_t maze_rows, size_t maze_cols,
              size_t display_rows, size_t display_cols,
              const std::vector<Cell> &grid, const std::vector<size_t> &path,
              size_t highlight_idx, bool is_generating, bool is_solving,
              bool is_paused, int solver_type, double last_solve_ms,
              int animation_speed, const MazeGen &maze_gen,
              const sf::Vector2f &mouse_pos) -> void;

    /**
     * @brief Handles SFML events and dispatches them to simulation state
     * changes via lambdas.
     */
    auto handle_event(sf::RenderWindow &window, const sf::Event &event,
                      size_t &maze_rows, size_t &maze_cols,
                      size_t &display_rows, size_t &display_cols,
                      bool &is_generating, bool &is_solving, bool &is_paused,
                      int &solver_type, int &animation_speed,
                      const std::function<void(bool)> &start_gen,
                      const std::function<void()>     &start_sol,
                      const std::function<void()>     &step_fn,
                      const std::function<void()>     &reset_fn) -> void;

    /** @brief Gets the current UI layout bounding boxes. */
    auto get_layout() const -> const UILayout & { return layout_; }

    static constexpr float K_SIDEBAR_WIDTH =
        280.0F;                                  ///< Width of the sidebar panel
    static constexpr float K_PADDING   = 20.0F;  ///< Standard UI padding
    static constexpr float K_CELL_SIZE = 22.0F;  ///< Base size for maze cells
    static constexpr float K_WALL_THICKNESS =
        1.5F;  ///< Thickness of maze walls

  private:
    sf::Font font_;    ///< The font used for all UI text
    UILayout layout_;  ///< Current layout bounds

    /** @brief Internal helper to draw the sidebar. */
    auto draw_sidebar(sf::RenderWindow &window, size_t display_rows,
                      size_t display_cols, bool is_generating, bool is_solving,
                      bool is_paused, int solver_type, double last_solve_ms,
                      size_t path_size, int animation_speed,
                      const sf::Vector2f &mouse_pos) -> void;

    /** @brief Internal helper to draw the maze and centering logic. */
    auto draw_maze(sf::RenderWindow &window, size_t rows, size_t cols,
                   const std::vector<Cell>   &grid,
                   const std::vector<size_t> &path, size_t highlight_idx,
                   bool is_generating, const MazeGen &maze_gen) -> void;

    /** @brief Internal helper to draw an interactive button. */
    auto draw_btn(sf::RenderWindow &window, const std::string &label,
                  sf::FloatRect &bounds, float x, float y, float width,
                  bool active, const sf::Vector2f &mouse_pos) -> void;
};

#endif  // MICRO_MOUSE_UI_HPP
