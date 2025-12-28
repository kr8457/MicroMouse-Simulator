/**
 * @file UI.cpp
 * @brief Implementation of the UI class for the MicroMouse simulator.
 */

#include "UI.hpp"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

/**
 * @namespace Theme
 * @brief Implementation of Theme colors.
 */
namespace Theme {
const sf::Color Background = sf::Color(15, 23, 42);  ///< Dark Slate Background
const sf::Color Sidebar = sf::Color(30, 41, 59);  ///< Slightly lighter Sidebar
const sf::Color Surface = sf::Color(51, 65, 85);  ///< UI Surface color
const sf::Color Primary = sf::Color(99, 102, 241);  ///< Indigo Primary
const sf::Color PrimaryHover =
    sf::Color(79, 70, 229);  ///< Indigo Darker for Hover
const sf::Color Accent    = sf::Color(244, 63, 112);   ///< Rose Accent
const sf::Color TextMain  = sf::Color(248, 250, 252);  ///< Off-white main text
const sf::Color TextDim   = sf::Color(148, 163, 184);  ///< Gray secondary text
const sf::Color Success   = sf::Color(16, 185, 129);   ///< Emerald indicator
const sf::Color PathColor = sf::Color(255, 255, 0);    ///< Electric Yellow Path
const sf::Color DotColor  = sf::Color(255, 255, 255);  ///< Path Head Dot
const sf::Color WallColor = sf::Color(100, 116, 139);  ///< Blue-gray walls
const sf::Color VisitedCell =
    sf::Color(30, 41, 59, 100);  ///< Semi-transparent visited cells
}  // namespace Theme

UI::UI() = default;

/**
 * @brief Attempts to load a font from multiple common system and local paths.
 * @return True if successful.
 */
auto UI::load_resources() -> bool {
    const std::vector<std::string> K_FONT_PATHS = {
        "assets/fonts/arial.ttf",
        "assets/fonts/consola.ttf",
        "../assets/fonts/arial.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/consola.ttf",
        "arial.ttf",
        "consola.ttf"};

    if (std::any_of(
            K_FONT_PATHS.begin(), K_FONT_PATHS.end(),
            [this](const auto &path) { return font_.loadFromFile(path); })) {
        return true;
    }

    // Last resort: Absolute path to Arial
    return font_.loadFromFile(R"(C:\Windows\Fonts\arial.ttf)");
}

/**
 * @brief Handles simulation interaction by checking if the mouse was clicked on
 * UI elements.
 */
auto UI::handle_event(sf::RenderWindow &window, const sf::Event &event,
                      size_t &maze_rows, size_t &maze_cols,
                      size_t &display_rows, size_t &display_cols,
                      bool & /*is_generating*/, bool & /*is_solving*/,
                      bool &is_paused, int &solver_type, int &animation_speed,
                      const std::function<void(bool)> &start_gen,
                      const std::function<void()>     &start_sol,
                      const std::function<void()>     &step_fn,
                      const std::function<void()>     &reset_fn) -> void {

    if (event.type != sf::Event::MouseButtonPressed) return;

    const sf::Vector2f MPOS =
        window.mapPixelToCoords(sf::Mouse::getPosition(window));
    const auto &L = layout_;

    static constexpr size_t K_MIN_MAZE_SIZE  = 5;
    static constexpr size_t K_MAX_MAZE_SIZE  = 100;
    static constexpr int    K_SPEED_STEP     = 5;
    static constexpr int    K_MIN_ANIM_SPEED = 1;
    static constexpr int    K_MAX_ANIM_SPEED = 200;

    // Handle all sidebar buttons using the layout_'s bounding rectangles
    if (L.row_dec_btn.contains(MPOS)) {
        if (display_rows > K_MIN_MAZE_SIZE) display_rows--;
    } else if (L.row_inc_btn.contains(MPOS)) {
        if (display_rows < K_MAX_MAZE_SIZE) display_rows++;
    } else if (L.col_dec_btn.contains(MPOS)) {
        if (display_cols > K_MIN_MAZE_SIZE) display_cols--;
    } else if (L.col_inc_btn.contains(MPOS)) {
        if (display_cols < K_MAX_MAZE_SIZE) display_cols++;
    } else if (L.apply_btn.contains(MPOS)) {
        maze_rows = display_rows;
        maze_cols = display_cols;
        start_gen(false);
    } else if (L.speed_dec_btn.contains(MPOS)) {
        if (animation_speed > K_MIN_ANIM_SPEED) {
            animation_speed -= K_SPEED_STEP;
        }
        if (animation_speed < K_MIN_ANIM_SPEED) {
            animation_speed = K_MIN_ANIM_SPEED;
        }
    } else if (L.speed_inc_btn.contains(MPOS)) {
        if (animation_speed < K_MAX_ANIM_SPEED) {
            animation_speed += K_SPEED_STEP;
        }
    } else if (L.gen_inst_btn.contains(MPOS))
        start_gen(false);
    else if (L.gen_step_btn.contains(MPOS))
        start_gen(true);
    else if (L.bfs_btn.contains(MPOS))
        solver_type = 0;
    else if (L.dfs_btn.contains(MPOS))
        solver_type = 1;
    else if (L.astar_btn.contains(MPOS))
        solver_type = 2;
    else if (L.flood_btn.contains(MPOS))
        solver_type = 3;
    else if (L.wall_btn.contains(MPOS))
        solver_type = 4;
    else if (L.solve_btn.contains(MPOS))
        start_sol();
    else if (L.solve_inst_btn.contains(MPOS)) {
        start_sol();
    } else if (L.play_btn.contains(MPOS))
        is_paused = !is_paused;
    else if (L.step_btn.contains(MPOS)) {
        is_paused = true;
        step_fn();
    } else if (L.reset_btn.contains(MPOS)) {
        reset_fn();
    }
}

auto UI::draw(sf::RenderWindow &window, size_t maze_rows, size_t maze_cols,
              size_t display_rows, size_t display_cols,
              const std::vector<Cell> &grid, const std::vector<size_t> &path,
              size_t highlight_idx, bool is_generating, bool is_solving,
              bool is_paused, int solver_type, double last_solve_ms,
              int animation_speed, const MazeGen &maze_gen,
              const sf::Vector2f &mouse_pos) -> void {

    draw_sidebar(window, display_rows, display_cols, is_generating, is_solving,
                 is_paused, solver_type, last_solve_ms, path.size(),
                 animation_speed, mouse_pos);

    draw_maze(window, maze_rows, maze_cols, grid, path, highlight_idx,
              is_generating, maze_gen);
}

/**
 * @brief Draws a reusable button element and updates its bounds in the layout.
 */
auto UI::draw_btn(sf::RenderWindow &window, const std::string &label,
                  sf::FloatRect &bounds, float x, float y, float width,
                  bool active, const sf::Vector2f &mouse_pos) -> void {
    static constexpr float        K_BTN_HEIGHT    = 36.0F;
    static constexpr float        K_OUTLINE_THICK = 2.0F;
    static constexpr unsigned int K_FONT_SIZE     = 14;

    sf::RectangleShape shape(sf::Vector2f(width, K_BTN_HEIGHT));
    shape.setPosition(x, y);
    bounds = shape.getGlobalBounds();

    const bool HOVERED = bounds.contains(mouse_pos);

    if (active) {
        shape.setFillColor(Theme::Primary);
        shape.setOutlineThickness(K_OUTLINE_THICK);
        shape.setOutlineColor(sf::Color::White);
    } else if (HOVERED) {
        shape.setFillColor(Theme::PrimaryHover);
    } else {
        shape.setFillColor(Theme::Surface);
    }

    window.draw(shape);

    sf::Text text(label, font_, K_FONT_SIZE);
    text.setFillColor(Theme::TextMain);
    const sf::FloatRect TB = text.getLocalBounds();
    text.setPosition(x + (width - TB.width) / 2.0F,
                     y + (K_BTN_HEIGHT - TB.height) / 2.0F - 4.0F);
    window.draw(text);
}

auto UI::draw_sidebar(sf::RenderWindow &window, size_t display_rows,
                      size_t display_cols, bool is_generating, bool is_solving,
                      bool is_paused, int solver_type, double last_solve_ms,
                      size_t path_size, int animation_speed,
                      const sf::Vector2f &mouse_pos) -> void {
    sf::RectangleShape sb_bg(
        sf::Vector2f(K_SIDEBAR_WIDTH, static_cast<float>(window.getSize().y)));
    sb_bg.setFillColor(Theme::Sidebar);
    window.draw(sb_bg);

    float cx = K_PADDING;
    float cy = K_PADDING;

    // Header
    sf::Text header("MICROMOUSE", font_, 22);
    header.setStyle(sf::Text::Bold);
    header.setFillColor(Theme::Primary);
    header.setPosition(cx, cy);
    window.draw(header);
    cy += 50.0F;

    // Section: Maze Configuration
    sf::Text cfg_title("MAZE CONFIG", font_, 14);
    cfg_title.setFillColor(Theme::TextDim);
    cfg_title.setPosition(cx, cy);
    window.draw(cfg_title);
    cy += 25.0F;

    auto draw_spinner = [&](const std::string &l, size_t v, sf::FloatRect &db,
                            sf::FloatRect &ib, float y) {
        sf::Text t(l + ": " + std::to_string(v), font_, 13);
        t.setFillColor(Theme::TextMain);
        t.setPosition(cx, y + 6.0F);
        window.draw(t);
        draw_btn(window, "-", db, cx + 130.0F, y, 32.0F, false, mouse_pos);
        draw_btn(window, "+", ib, cx + 175.0F, y, 32.0F, false, mouse_pos);
    };

    draw_spinner("Rows", display_rows, layout_.row_dec_btn, layout_.row_inc_btn,
                 cy);
    cy += 42.0F;
    draw_spinner("Cols", display_cols, layout_.col_dec_btn, layout_.col_inc_btn,
                 cy);
    cy += 42.0F;
    draw_btn(window, "APPLY DIMENSIONS", layout_.apply_btn, cx, cy,
             K_SIDEBAR_WIDTH - 40.0F, false, mouse_pos);
    cy += 50.0F;

    // Section: Speed Control
    sf::Text spd_title("ANIMATION SPEED", font_, 14);
    spd_title.setFillColor(Theme::TextDim);
    spd_title.setPosition(cx, cy);
    window.draw(spd_title);
    cy += 25.0F;
    draw_spinner("Speed", static_cast<size_t>(animation_speed),
                 layout_.speed_dec_btn, layout_.speed_inc_btn, cy);
    cy += 55.0F;

    // Section: Generation
    sf::Text gen_title("GENERATION", font_, 14);
    gen_title.setFillColor(Theme::TextDim);
    gen_title.setPosition(cx, cy);
    window.draw(gen_title);
    cy += 25.0F;
    draw_btn(window, "Quick Gen", layout_.gen_inst_btn, cx, cy,
             (K_SIDEBAR_WIDTH - 50.0F) / 2.0F, false, mouse_pos);
    draw_btn(window, "Step Gen", layout_.gen_step_btn,
             cx + (K_SIDEBAR_WIDTH - 50.0F) / 2.0F + 10.0F, cy,
             (K_SIDEBAR_WIDTH - 50.0F) / 2.0F, false, mouse_pos);
    cy += 45.0F;

    // Section: Solver Algorithms
    sf::Text sol_title("ALGORITHMS", font_, 14);
    sol_title.setFillColor(Theme::TextDim);
    sol_title.setPosition(cx, cy);
    window.draw(sol_title);
    cy                 += 25.0F;
    const float BW_VAL  = (K_SIDEBAR_WIDTH - 50.0F) / 2.0F;
    draw_btn(window, "BFS", layout_.bfs_btn, cx, cy, BW_VAL, solver_type == 0,
             mouse_pos);
    draw_btn(window, "DFS", layout_.dfs_btn, cx + BW_VAL + 10.0F, cy, BW_VAL,
             solver_type == 1, mouse_pos);
    cy += 40.0F;
    draw_btn(window, "A*", layout_.astar_btn, cx, cy, BW_VAL, solver_type == 2,
             mouse_pos);
    draw_btn(window, "Flood Fill", layout_.flood_btn, cx + BW_VAL + 10.0F, cy,
             BW_VAL, solver_type == 3, mouse_pos);
    cy += 40.0F;
    draw_btn(window, "Wall Follow", layout_.wall_btn, cx, cy,
             K_SIDEBAR_WIDTH - 40.0F, solver_type == 4, mouse_pos);
    cy += 45.0F;
    draw_btn(window, "ANIMATE SOLVE", layout_.solve_btn, cx, cy, BW_VAL, false,
             mouse_pos);
    draw_btn(window, "INSTANT SOLVE", layout_.solve_inst_btn,
             cx + BW_VAL + 10.0F, cy, BW_VAL, false, mouse_pos);
    cy += 50.0F;

    // Section: Global Controls
    std::string pl = is_paused ? "RESUME" : "PAUSE";
    if (!is_generating && !is_solving) pl = "START";
    draw_btn(window, pl, layout_.play_btn, cx, cy, BW_VAL,
             !is_paused && (is_generating || is_solving), mouse_pos);
    draw_btn(window, "STEP", layout_.step_btn, cx + BW_VAL + 10.0F, cy, BW_VAL,
             false, mouse_pos);
    cy += 40.0F;
    draw_btn(window, "RESET SIMULATOR", layout_.reset_btn, cx, cy,
             K_SIDEBAR_WIDTH - 40.0F, false, mouse_pos);
    cy += 50.0F;

    // Section: Stats Card
    if (path_size > 0 || last_solve_ms > 0.0) {
        sf::RectangleShape card(sf::Vector2f(K_SIDEBAR_WIDTH - 40.0F, 70.0F));
        card.setPosition(cx, cy);
        card.setFillColor(sf::Color(15, 23, 42, 180));
        card.setOutlineThickness(1.0F);
        card.setOutlineColor(Theme::Surface);
        window.draw(card);

        std::stringstream ss;
        ss << "Time: " << std::fixed << std::setprecision(2) << last_solve_ms
           << "ms\n";
        ss << "Steps: " << path_size;
        sf::Text st(ss.str(), font_, 12);
        st.setFillColor(Theme::Success);
        st.setPosition(cx + 12.0F, cy + 12.0F);
        window.draw(st);
    }
}

auto UI::draw_maze(sf::RenderWindow &window, size_t rows, size_t cols,
                   const std::vector<Cell>   &grid,
                   const std::vector<size_t> &path, size_t highlight_idx,
                   bool is_generating, const MazeGen &maze_gen) -> void {
    if (grid.empty()) return;

    // Center the maze in the viewport (remaining space after sidebar)
    const float MAZE_W = static_cast<float>(cols) * K_CELL_SIZE;
    const float MAZE_H = static_cast<float>(rows) * K_CELL_SIZE;
    const float OFFSET_X =
        K_SIDEBAR_WIDTH +
        (static_cast<float>(window.getSize().x) - K_SIDEBAR_WIDTH - MAZE_W) /
            2.0F;
    const float OFFSET_Y =
        (static_cast<float>(window.getSize().y) - MAZE_H) / 2.0F;

    // Draw Grid Cells
    sf::RectangleShape cs(sf::Vector2f(K_CELL_SIZE, K_CELL_SIZE));
    for (size_t r_idx = 0; r_idx < rows; ++r_idx) {
        for (size_t c_idx = 0; c_idx < cols; ++c_idx) {
            const size_t idx = MazeSolver::get_1d_index(r_idx, c_idx, cols);
            cs.setPosition(OFFSET_X + static_cast<float>(c_idx) * K_CELL_SIZE,
                           OFFSET_Y + static_cast<float>(r_idx) * K_CELL_SIZE);
            if (grid[idx].visited) {
                cs.setFillColor(Theme::VisitedCell);
            } else {
                cs.setFillColor(sf::Color(30, 41, 59, 40));
            }
            if (idx == 0) {
                cs.setFillColor(sf::Color(34, 197, 94));  // Start
            } else if (idx == (rows * cols - 1)) {
                cs.setFillColor(Theme::Accent);  // End
            }
            window.draw(cs);
        }
    }

    // Draw Maze Generation Animation Path
    if (is_generating) {
        const auto &gp = maze_gen.get_path();
        if (gp.size() > 1) {
            for (size_t i = 0; i < gp.size() - 1; ++i) {
                sf::Vertex line[] = {
                    sf::Vertex(
                        sf::Vector2f(
                            OFFSET_X +
                                static_cast<float>(gp[i].second) * K_CELL_SIZE +
                                K_CELL_SIZE / 2.0F,
                            OFFSET_Y +
                                static_cast<float>(gp[i].first) * K_CELL_SIZE +
                                K_CELL_SIZE / 2.0F),
                        Theme::Primary),
                    sf::Vertex(
                        sf::Vector2f(OFFSET_X +
                                         static_cast<float>(gp[i + 1].second) *
                                             K_CELL_SIZE +
                                         K_CELL_SIZE / 2.0F,
                                     OFFSET_Y +
                                         static_cast<float>(gp[i + 1].first) *
                                             K_CELL_SIZE +
                                         K_CELL_SIZE / 2.0F),
                        Theme::Primary)};
                window.draw(line, 2, sf::Lines);
            }
            // Draw a white circle at the generator head
            sf::CircleShape h(K_CELL_SIZE * 0.35F);
            h.setOrigin(h.getRadius(), h.getRadius());
            h.setFillColor(sf::Color::White);
            h.setPosition(
                OFFSET_X +
                    static_cast<float>(gp.back().second) * K_CELL_SIZE +
                    K_CELL_SIZE / 2.0F,
                OFFSET_Y +
                    static_cast<float>(gp.back().first) * K_CELL_SIZE +
                    K_CELL_SIZE / 2.0F);
            window.draw(h);
        }
    }

    // Draw Solved Path as THICK YELLOW RIBBONS
    if (!path.empty() && highlight_idx > 0) {
        static constexpr float K_RIBBON_THICK = 6.0F;

        for (size_t i = 0; i < highlight_idx && i < path.size() - 1; ++i) {
            const auto  coords1 = MazeSolver::get_2d_coords(path[i], cols);
            const auto  coords2 = MazeSolver::get_2d_coords(path[i + 1], cols);
            const float x1      = OFFSET_X +
                             static_cast<float>(coords1.second) * K_CELL_SIZE +
                             K_CELL_SIZE / 2.0F;
            const float y1 = OFFSET_Y +
                             static_cast<float>(coords1.first) * K_CELL_SIZE +
                             K_CELL_SIZE / 2.0F;
            const float x2 = OFFSET_X +
                             static_cast<float>(coords2.second) * K_CELL_SIZE +
                             K_CELL_SIZE / 2.0F;
            const float y2 = OFFSET_Y +
                             static_cast<float>(coords2.first) * K_CELL_SIZE +
                             K_CELL_SIZE / 2.0F;
            const float dx     = x2 - x1;
            const float dy     = y2 - y1;
            const float length = std::sqrt(dx * dx + dy * dy);
            const float angle  = std::atan2(dy, dx) * 180.0F / 3.14159F;

            sf::RectangleShape segment(sf::Vector2f(length, K_RIBBON_THICK));
            segment.setOrigin(0, K_RIBBON_THICK / 2.0F);
            segment.setPosition(x1, y1);
            segment.setRotation(angle);
            segment.setFillColor(Theme::PathColor);
            window.draw(segment);
        }

        // Draw the leading "Dot" marker
        const size_t head_idx = std::min(highlight_idx, path.size() - 1);
        const auto   head_coords =
            MazeSolver::get_2d_coords(path[head_idx], cols);
        const float hx = OFFSET_X +
                         static_cast<float>(head_coords.second) * K_CELL_SIZE +
                         K_CELL_SIZE / 2.0F;
        const float hy = OFFSET_Y +
                         static_cast<float>(head_coords.first) * K_CELL_SIZE +
                         K_CELL_SIZE / 2.0F;

        static constexpr float K_DOT_RADIUS = 6.0F;
        sf::CircleShape        dot(K_DOT_RADIUS);
        dot.setOrigin(K_DOT_RADIUS, K_DOT_RADIUS);
        dot.setFillColor(Theme::DotColor);
        dot.setOutlineThickness(2.0F);
        dot.setOutlineColor(sf::Color::Black);
        dot.setPosition(hx, hy);
        window.draw(dot);
    }

    // Draw Walls
    sf::RectangleShape wh(
        sf::Vector2f(K_CELL_SIZE + K_WALL_THICKNESS, K_WALL_THICKNESS));
    sf::RectangleShape wv(
        sf::Vector2f(K_WALL_THICKNESS, K_CELL_SIZE + K_WALL_THICKNESS));
    wh.setFillColor(Theme::WallColor);
    wv.setFillColor(Theme::WallColor);

    for (size_t r_idx = 0; r_idx < rows; ++r_idx) {
        for (size_t c_idx = 0; c_idx < cols; ++c_idx) {
            const auto &cell =
                grid[MazeSolver::get_1d_index(r_idx, c_idx, cols)];
            const float px = OFFSET_X + static_cast<float>(c_idx) * K_CELL_SIZE;
            const float py = OFFSET_Y + static_cast<float>(r_idx) * K_CELL_SIZE;
            if (cell.top) {
                wh.setPosition(px, py);
                window.draw(wh);
            }
            if (cell.left) {
                wv.setPosition(px, py);
                window.draw(wv);
            }
            if (c_idx == cols - 1 && cell.right) {
                wv.setPosition(px + K_CELL_SIZE, py);
                window.draw(wv);
            }
            if (r_idx == rows - 1 && cell.bottom) {
                wh.setPosition(px, py + K_CELL_SIZE);
                window.draw(wh);
            }
        }
    }
}
