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
 * @brief Implementation of Sunset Theme colors.
 */
namespace Theme {
// Deep Twilight/Purple base
const sf::Color Background = sf::Color(30, 27, 46);   ///< Deep Midnight Purple
const sf::Color Sidebar    = sf::Color(45, 35, 66);   ///< Muted Plum Sidebar
const sf::Color Surface    = sf::Color(68, 56, 100);  ///< Lighter Purple Surface

// The "Sun" Colors (Warm Tones)
const sf::Color Primary      = sf::Color(251, 146, 60);  ///< Bright Sunset Orange
const sf::Color PrimaryHover = sf::Color(249, 115, 22);  ///< Deep Burnt Orange
const sf::Color Accent       = sf::Color(244, 63, 112);  ///< Vivid Rose/Pink

// Typography
const sf::Color TextMain = sf::Color(255, 247, 237);  ///< Warm Cream White
const sf::Color TextDim  = sf::Color(167, 139, 192);  ///< Soft Lavender Gray

// State Indicators
const sf::Color Success   = sf::Color(52, 211, 153);  ///< Seafoam Green
const sf::Color PathColor = sf::Color(253, 224, 71);  ///< Golden Hour Yellow
const sf::Color ExplorationColor = sf::Color(56, 189, 248);  ///< Light Sky Blue
const sf::Color DotColor  = sf::Color(255, 255, 255); ///< Pure White
const sf::Color WallColor = sf::Color(91, 76, 125);   ///< Dusty Purple Walls

// Visited cells use a subtle warm glow
const sf::Color VisitedCell = sf::Color(251, 146, 60, 40); ///< Transparent Orange Glow
}  // namespace Theme

UI::UI() : row_input_buffer_("16"), col_input_buffer_("16"), speed_input_buffer_("100") {}

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

    const sf::Vector2f MPOS =
        window.mapPixelToCoords(sf::Mouse::getPosition(window));
    const auto &L = layout_;

    static constexpr size_t K_MIN_MAZE_SIZE  = 5;
    static constexpr size_t K_MAX_MAZE_SIZE  = 100;
    static constexpr int    K_SPEED_STEP     = 5;
    static constexpr int    K_MIN_ANIM_SPEED = 1;
    static constexpr int    K_MAX_ANIM_SPEED = 200;

    // Synchronize buffers if not focused (e.g. after reset or increment/decrement if we still had those)
    if (!row_focused_) row_input_buffer_ = std::to_string(display_rows);
    if (!col_focused_) col_input_buffer_ = std::to_string(display_cols);
    if (!speed_focused_) speed_input_buffer_ = std::to_string(animation_speed);

    auto commit_dimensions = [&]() {
        try {
            if (!row_input_buffer_.empty()) display_rows = std::stoul(row_input_buffer_);
            if (!col_input_buffer_.empty()) display_cols = std::stoul(col_input_buffer_);
        } catch (...) {}

        display_rows = std::clamp(display_rows, K_MIN_MAZE_SIZE, K_MAX_MAZE_SIZE);
        display_cols = std::clamp(display_cols, K_MIN_MAZE_SIZE, K_MAX_MAZE_SIZE);
        
        row_input_buffer_ = std::to_string(display_rows);
        col_input_buffer_ = std::to_string(display_cols);
        row_focused_ = false;
        col_focused_ = false;

        maze_rows = display_rows;
        maze_cols = display_cols;
        start_gen(false);
    };

    auto commit_speed = [&]() {
        try {
            if (!speed_input_buffer_.empty()) animation_speed = std::stoi(speed_input_buffer_);
        } catch (...) {}
        animation_speed = std::clamp(animation_speed, K_MIN_ANIM_SPEED, K_MAX_ANIM_SPEED);
        speed_input_buffer_ = std::to_string(animation_speed);
        speed_focused_ = false;
    };

    // Handle clicks for focus and buttons
    if (event.type == sf::Event::MouseButtonPressed) {
        const bool clicked_row = L.row_input_box.contains(MPOS);
        const bool clicked_col = L.col_input_box.contains(MPOS);
        const bool clicked_speed = L.speed_input_box.contains(MPOS);

        // Commit speed if focus is lost
        if (speed_focused_ && !clicked_speed) {
            commit_speed();
        }

        row_focused_ = clicked_row;
        col_focused_ = clicked_col;
        speed_focused_ = clicked_speed;

        if (L.apply_btn.contains(MPOS)) {
            commit_dimensions();
        } else if (L.gen_inst_btn.contains(MPOS)) start_gen(false);
        else if (L.gen_step_btn.contains(MPOS)) start_gen(true);
        else if (L.bfs_btn.contains(MPOS)) solver_type = 0;
        else if (L.dfs_btn.contains(MPOS)) solver_type = 1;
        else if (L.astar_btn.contains(MPOS)) solver_type = 2;
        else if (L.flood_btn.contains(MPOS)) solver_type = 3;
        else if (L.wall_btn.contains(MPOS)) solver_type = 4;
        else if (L.solve_btn.contains(MPOS)) start_sol();
        else if (L.solve_inst_btn.contains(MPOS)) start_sol();
        else if (L.play_btn.contains(MPOS)) is_paused = !is_paused;
        else if (L.step_btn.contains(MPOS)) { is_paused = true; step_fn(); }
        else if (L.reset_btn.contains(MPOS)) reset_fn();
    }

    // Handle character input
    if (event.type == sf::Event::TextEntered) {
        if (row_focused_ || col_focused_ || speed_focused_) {
            if (event.text.unicode == 13 || event.text.unicode == 10) { // Enter
                if (speed_focused_) commit_speed();
                else commit_dimensions();
            } else if (event.text.unicode == 8) { // Backspace
                std::string &buffer = row_focused_ ? row_input_buffer_ : (col_focused_ ? col_input_buffer_ : speed_input_buffer_);
                if (!buffer.empty()) buffer.pop_back();
            } else if (event.text.unicode >= 48 && event.text.unicode <= 57) { // 0-9
                std::string &buffer = row_focused_ ? row_input_buffer_ : (col_focused_ ? col_input_buffer_ : speed_input_buffer_);
                if (buffer.length() < 3) buffer += static_cast<char>(event.text.unicode);
            }
        }
    }
}

auto UI::draw(sf::RenderWindow &window, size_t maze_rows, size_t maze_cols,
              size_t display_rows, size_t display_cols,
              const std::vector<Cell> &grid, 
              const std::vector<size_t> &exploration_path,
              const std::vector<size_t> &solved_path,
              bool is_generating, bool is_solving,
              bool is_paused, int solver_type,
              int animation_speed, const MazeGen &maze_gen,
              const sf::Vector2f &mouse_pos) -> void {

    draw_sidebar(window, display_rows, display_cols, is_generating, is_solving,
                 is_paused, solver_type, exploration_path.size(), 
                 solved_path.size(), animation_speed, mouse_pos);

    draw_maze(window, maze_rows, maze_cols, grid, exploration_path, 
              solved_path, is_generating, maze_gen);
}

/**
 * @brief Draws a reusable button element and updates its bounds in the layout.
 */
auto UI::draw_btn(sf::RenderWindow &window, const std::string &label,
                  sf::FloatRect &bounds, float x, float y, float width,
                  bool active, const sf::Vector2f &mouse_pos) -> void {
    // Responsive button height based on window size
    const float window_height = static_cast<float>(window.getSize().y);
    const float btn_height = std::clamp(window_height * 0.05f, 30.0f, 42.0f);
    const float outline_thick = 2.0F;
    const unsigned int font_size = static_cast<unsigned int>(std::clamp(window_height * 0.02f, 12.0f, 16.0f));

    sf::RectangleShape shape(sf::Vector2f(width, btn_height));
    shape.setPosition(x, y);
    bounds = shape.getGlobalBounds();

    const bool HOVERED = bounds.contains(mouse_pos);

    if (active) {
        shape.setFillColor(Theme::Primary);
        shape.setOutlineThickness(outline_thick);
        shape.setOutlineColor(sf::Color::White);
    } else if (HOVERED) {
        shape.setFillColor(Theme::PrimaryHover);
    } else {
        shape.setFillColor(Theme::Surface);
    }

    window.draw(shape);

    sf::Text text(label, font_, font_size);
    text.setFillColor(Theme::TextMain);
    sf::FloatRect tb = text.getLocalBounds();
    
    // Auto-scale text if too wide for the button
    const float max_w = width - 12.0f; // 6px padding on each side
    if (tb.width > max_w) {
        float s = max_w / tb.width;
        text.setScale(s, s);
        // Re-measure after scaling for centering if needed, 
        // but origin-based centering is more robust.
    }

    // Origin-based centering is easier for scaled text
    text.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
    text.setPosition(x + width / 2.0f, y + btn_height / 2.0f);
    
    window.draw(text);
}

auto UI::draw_sidebar(sf::RenderWindow &window, size_t display_rows,
                      size_t display_cols, bool is_generating, bool is_solving,
                      bool is_paused, int solver_type,
                      size_t exploration_count, size_t solved_count,
                      int animation_speed,
                      const sf::Vector2f &mouse_pos) -> void {
    // Calculate responsive dimensions
    const float window_width = static_cast<float>(window.getSize().x);
    const float window_height = static_cast<float>(window.getSize().y);
    const float sidebar_width = std::clamp(window_width * 0.2f, 200.0f, 350.0f);
    const float padding = std::clamp(window_width * 0.015f, 12.0f, 24.0f);
    
    // Responsive font sizes
    const unsigned int header_size = static_cast<unsigned int>(std::clamp(window_height * 0.03f, 18.0f, 26.0f));
    const unsigned int section_size = static_cast<unsigned int>(std::clamp(window_height * 0.02f, 12.0f, 16.0f));
    const unsigned int text_size = static_cast<unsigned int>(std::clamp(window_height * 0.018f, 11.0f, 14.0f));
    const unsigned int stats_size = static_cast<unsigned int>(std::clamp(window_height * 0.017f, 10.0f, 13.0f));
    
    sf::RectangleShape sb_bg(
        sf::Vector2f(sidebar_width, window_height));
    sb_bg.setFillColor(Theme::Sidebar);
    window.draw(sb_bg);

    float cx = padding;
    float cy = padding;

    // Header
    sf::Text header("MICROMOUSE", font_, header_size);
    header.setStyle(sf::Text::Bold);
    header.setFillColor(Theme::Primary);
    header.setPosition(cx, cy);
    window.draw(header);
    cy += header_size + padding * 1.5f;

    // Section: Maze Configuration
    sf::Text cfg_title("MAZE CONFIG", font_, section_size);
    cfg_title.setFillColor(Theme::TextDim);
    cfg_title.setPosition(cx, cy);
    window.draw(cfg_title);
    cy += section_size + padding * 0.5f;

    const float input_height = std::clamp(window_height * 0.05f, 32.0f, 40.0f);
    const float input_box_width = sidebar_width - padding * 2.0f;
    
    draw_input_box(window, "Rows", row_input_buffer_, layout_.row_input_box, cx, cy, input_box_width, row_focused_);
    cy += input_height;
    draw_input_box(window, "Cols", col_input_buffer_, layout_.col_input_box, cx, cy, input_box_width, col_focused_);
    cy += input_height + padding * 0.4f;

    draw_btn(window, "APPLY DIMENSIONS", layout_.apply_btn, cx, cy,
             sidebar_width - padding * 2.0f, false, mouse_pos);
    cy += input_height + padding * 0.5f;

    // Section: Speed Control
    sf::Text spd_title("ANIMATION SPEED", font_, section_size);
    spd_title.setFillColor(Theme::TextDim);
    spd_title.setPosition(cx, cy);
    window.draw(spd_title);
    cy += section_size + padding * 0.5f;

    draw_input_box(window, "Speed", speed_input_buffer_, layout_.speed_input_box, cx, cy, input_box_width, speed_focused_);
    cy += input_height + padding;

    // Section: Generation
    sf::Text gen_title("GENERATION", font_, section_size);
    gen_title.setFillColor(Theme::TextDim);
    gen_title.setPosition(cx, cy);
    window.draw(gen_title);
    cy += section_size + padding * 0.5f;
    const float btn_spacing = padding * 0.5f;
    const float btn_width = (sidebar_width - padding * 2.0f - btn_spacing) / 2.0F;
    const float btn_height_spacing = std::clamp(window_height * 0.062f, 38.0f, 50.0f);
    draw_btn(window, "Quick Gen", layout_.gen_inst_btn, cx, cy,
             btn_width, false, mouse_pos);
    draw_btn(window, "Step Gen", layout_.gen_step_btn,
             cx + btn_width + btn_spacing, cy,
             btn_width, false, mouse_pos);
    cy += btn_height_spacing;

    // Section: Solver Algorithms
    sf::Text sol_title("ALGORITHMS", font_, section_size);
    sol_title.setFillColor(Theme::TextDim);
    sol_title.setPosition(cx, cy);
    window.draw(sol_title);
    cy += section_size + padding * 0.5f;
    const float algo_btn_width = (sidebar_width - padding * 2.0f - btn_spacing) / 2.0F;
    draw_btn(window, "BFS", layout_.bfs_btn, cx, cy, algo_btn_width, solver_type == 0,
             mouse_pos);
    draw_btn(window, "DFS", layout_.dfs_btn, cx + algo_btn_width + btn_spacing, cy, algo_btn_width,
             solver_type == 1, mouse_pos);
    cy += btn_height_spacing - padding * 0.2f;
    draw_btn(window, "A*", layout_.astar_btn, cx, cy, algo_btn_width, solver_type == 2,
             mouse_pos);
    draw_btn(window, "Flood Fill", layout_.flood_btn, cx + algo_btn_width + btn_spacing, cy,
             algo_btn_width, solver_type == 3, mouse_pos);
    cy += btn_height_spacing - padding * 0.2f;
    draw_btn(window, "Wall Follow", layout_.wall_btn, cx, cy,
             sidebar_width - padding * 2.0f, solver_type == 4, mouse_pos);
    cy += btn_height_spacing;
    draw_btn(window, "ANIMATE SOLVE", layout_.solve_btn, cx, cy, algo_btn_width, false,
             mouse_pos);
    draw_btn(window, "INSTANT SOLVE", layout_.solve_inst_btn,
             cx + algo_btn_width + btn_spacing, cy, algo_btn_width, false, mouse_pos);
    cy += btn_height_spacing;

    // Section: Global Controls
    std::string pl = is_paused ? "RESUME" : "PAUSE";
    if (!is_generating && !is_solving) pl = "START";
    draw_btn(window, pl, layout_.play_btn, cx, cy, algo_btn_width,
             !is_paused && (is_generating || is_solving), mouse_pos);
    draw_btn(window, "STEP", layout_.step_btn, cx + algo_btn_width + btn_spacing, cy, algo_btn_width,
             false, mouse_pos);
    cy += btn_height_spacing - padding * 0.2f;
    draw_btn(window, "RESET SIMULATOR", layout_.reset_btn, cx, cy,
             sidebar_width - padding * 2.0f, false, mouse_pos);
    cy += btn_height_spacing;

    // Section: Stats Card
    // Section: Stats Card
    if (exploration_count > 0 || solved_count > 0) {
        const float card_height = std::clamp(window_height * 0.08f, 50.0f, 70.0f);
        sf::RectangleShape card(sf::Vector2f(sidebar_width - padding * 2.0f, card_height));
        card.setPosition(cx, cy);
        card.setFillColor(sf::Color(15, 23, 42, 180));
        card.setOutlineThickness(1.0F);
        card.setOutlineColor(Theme::Surface);
        window.draw(card);

        std::stringstream ss;
        ss << "Explored: " << exploration_count << "\n";
        if (solver_type == 4) { // Wall Follower
             ss << "Actual Path: " << solved_count;
        } else {
             ss << "Final Path: " << solved_count;
        }
        
        sf::Text st(ss.str(), font_, stats_size);
        st.setFillColor(Theme::TextMain);
        st.setPosition(cx + 12.0F, cy + 12.0F);
        window.draw(st);
    }
}

auto UI::draw_maze(sf::RenderWindow &window, size_t rows, size_t cols,
                   const std::vector<Cell>   &grid,
                   const std::vector<size_t> &exploration_path,
                   const std::vector<size_t> &solved_path,
                   bool is_generating,
                   const MazeGen &maze_gen) -> void {
    if (grid.empty()) return;

    // Calculate responsive dimensions
    const float window_width = static_cast<float>(window.getSize().x);
    const float window_height = static_cast<float>(window.getSize().y);
    const float sidebar_width = std::clamp(window_width * 0.2f, 200.0f, 350.0f);
    const float available_width = window_width - sidebar_width - 40.0f;
    const float available_height = window_height - 40.0f;
    const float cell_size_w = available_width / static_cast<float>(cols);
    const float cell_size_h = available_height / static_cast<float>(rows);
    const float cell_size = std::min({cell_size_w, cell_size_h, 40.0f});
    const float wall_thickness = std::max(1.0f, cell_size * 0.08f);
    const float MAZE_W = static_cast<float>(cols) * cell_size;
    const float MAZE_H = static_cast<float>(rows) * cell_size;
    const float OFFSET_X = sidebar_width + (window_width - sidebar_width - MAZE_W) / 2.0F;
    const float OFFSET_Y = (window_height - MAZE_H) / 2.0F;

    // 1. Draw Grid Cells
    sf::RectangleShape cs(sf::Vector2f(cell_size, cell_size));
    for (size_t r_idx = 0; r_idx < rows; ++r_idx) {
        for (size_t c_idx = 0; c_idx < cols; ++c_idx) {
            const size_t idx = (r_idx * cols) + c_idx;
            cs.setPosition(OFFSET_X + static_cast<float>(c_idx) * cell_size,
                           OFFSET_Y + static_cast<float>(r_idx) * cell_size);
            if (grid[idx].visited) {
                cs.setFillColor(Theme::VisitedCell);
            } else {
                cs.setFillColor(sf::Color(30, 41, 59, 40));
            }
            if (idx == 0) cs.setFillColor(sf::Color(34, 197, 94));
            else if (idx == (rows * cols - 1)) cs.setFillColor(Theme::Accent);
            window.draw(cs);
        }
    }

    // 2. Draw Maze Generation Animation Path
    if (is_generating) {
        const auto &gp = maze_gen.get_path();
        if (gp.size() > 1) {
            for (size_t i = 0; i < gp.size() - 1; ++i) {
                sf::Vertex line[] = {
                    sf::Vertex(sf::Vector2f(OFFSET_X + gp[i].second * cell_size + cell_size / 2.0F,
                                            OFFSET_Y + gp[i].first * cell_size + cell_size / 2.0F),
                               Theme::Primary),
                    sf::Vertex(sf::Vector2f(OFFSET_X + gp[i+1].second * cell_size + cell_size / 2.0F,
                                            OFFSET_Y + gp[i+1].first * cell_size + cell_size / 2.0F),
                               Theme::Primary)};
                window.draw(line, 2, sf::Lines);
            }
            sf::CircleShape h(cell_size * 0.35F);
            h.setOrigin(h.getRadius(), h.getRadius());
            h.setFillColor(sf::Color::White);
            h.setPosition(OFFSET_X + gp.back().second * cell_size + cell_size / 2.0F,
                          OFFSET_Y + gp.back().first * cell_size + cell_size / 2.0F);
            window.draw(h);
        }
    }

    // 3. Draw Exploration Data as Dots
    if (!exploration_path.empty()) {
        const float dot_radius = std::max(2.0f, cell_size * 0.22f);
        sf::CircleShape dot(dot_radius);
        dot.setOrigin(dot_radius, dot_radius);
        dot.setFillColor(Theme::ExplorationColor);
        for (const auto node_idx : exploration_path) {
            auto [r, c] = MazeSolver::get_2d_coords(node_idx, cols);
            dot.setPosition(OFFSET_X + c * cell_size + cell_size / 2.0F,
                            OFFSET_Y + r * cell_size + cell_size / 2.0F);
            window.draw(dot);
        }

        // Highlight head
        size_t head_node = exploration_path.back();
        auto [hr, hc] = MazeSolver::get_2d_coords(head_node, cols);
        sf::RectangleShape head(sf::Vector2f(cell_size, cell_size));
        head.setPosition(OFFSET_X + hc * cell_size, OFFSET_Y + hr * cell_size);
        head.setFillColor(sf::Color(255, 255, 255, 100));
        window.draw(head);
    }

    // 4. Overlay Solution Path as Ribbon
    if (!solved_path.empty()) {
        const float ribbon_thick = std::max(3.0f, cell_size * 0.25f);
        for (size_t i = 0; i < solved_path.size() - 1; ++i) {
            auto [r1, c1] = MazeSolver::get_2d_coords(solved_path[i], cols);
            auto [r2, c2] = MazeSolver::get_2d_coords(solved_path[i+1], cols);
            float x1 = OFFSET_X + c1 * cell_size + cell_size / 2.0F, y1 = OFFSET_Y + r1 * cell_size + cell_size / 2.0F;
            float x2 = OFFSET_X + c2 * cell_size + cell_size / 2.0F, y2 = OFFSET_Y + r2 * cell_size + cell_size / 2.0F;
            float dx = x2 - x1, dy = y2 - y1;
            float length = std::sqrt(dx*dx + dy*dy);
            float angle = std::atan2(dy, dx) * 180.0F / 3.14159F;
            sf::RectangleShape seg(sf::Vector2f(length, ribbon_thick));
            seg.setOrigin(0, ribbon_thick / 2.0F);
            seg.setPosition(x1, y1);
            seg.setRotation(angle);
            seg.setFillColor(Theme::PathColor);
            window.draw(seg);
        }
    }

    // 5. Draw Walls
    sf::RectangleShape wh(sf::Vector2f(cell_size + wall_thickness, wall_thickness));
    sf::RectangleShape wv(sf::Vector2f(wall_thickness, cell_size + wall_thickness));
    wh.setFillColor(Theme::WallColor);
    wv.setFillColor(Theme::WallColor);

    for (size_t r = 0; r < rows; ++r) {
        for (size_t c = 0; c < cols; ++c) {
            const auto &cell = grid[r * cols + c];
            float px = OFFSET_X + c * cell_size, py = OFFSET_Y + r * cell_size;
            if (cell.top) { wh.setPosition(px - wall_thickness/2.0f, py - wall_thickness/2.0f); window.draw(wh); }
            if (cell.bottom) { wh.setPosition(px - wall_thickness/2.0f, py + cell_size - wall_thickness/2.0f); window.draw(wh); }
            if (cell.left) { wv.setPosition(px - wall_thickness/2.0f, py - wall_thickness/2.0f); window.draw(wv); }
            if (cell.right) { wv.setPosition(px + cell_size - wall_thickness/2.0f, py - wall_thickness/2.0f); window.draw(wv); }
        }
    }
}

auto UI::draw_input_box(sf::RenderWindow &window, const std::string &label,
                        const std::string &value, sf::FloatRect &bounds,
                        float x, float y, float width, bool focused) -> void {
    const float label_width = 55.0f;
    const float padding = 8.0f;
    const float height = std::clamp(static_cast<float>(window.getSize().y) * 0.045f, 28.0f, 36.0f);

    sf::Text t(label + ":", font_, 14);
    t.setFillColor(Theme::TextDim);
    t.setPosition(x, y + (height - 20.0f) / 2.0f);
    window.draw(t);

    sf::RectangleShape rect(sf::Vector2f(width - label_width, height));
    rect.setPosition(x + label_width, y);
    rect.setFillColor(sf::Color(15, 23, 42));
    rect.setOutlineThickness(focused ? 2.0f : 1.0f);
    rect.setOutlineColor(focused ? Theme::Primary : Theme::Surface);
    window.draw(rect);
    bounds = rect.getGlobalBounds();

    sf::Text val(value + (focused ? "_" : ""), font_, 14);
    val.setFillColor(Theme::TextMain);
    val.setPosition(x + label_width + padding, y + (height - 20.0f) / 2.0f);
    window.draw(val);
}
