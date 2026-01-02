/**
 * @file UI.cpp
 * @brief Implementation of the UI class for the MicroMouse simulator.
 */

#include "UI.hpp"
#include "MazeUtils.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <string>
#include <vector>


UI::UI() : row_input_buffer_("16"), col_input_buffer_("16"), speed_input_buffer_("100") {}

auto UI::load_resources() -> bool {
    const std::vector<std::string> K_FONT_PATHS = {
        "assets/fonts/PixelOperatorMono-Bold.ttf",
        "../assets/fonts/PixelOperatorMono-Bold.ttf",
        "assets/fonts/Pixel-Regular.ttf",
        "../assets/fonts/Pixel-Regular.ttf",
        "assets/fonts/pixel.ttf",
        "../assets/fonts/pixel.ttf",
        "pixel.ttf",
        // Fallback: OpenSans if available (found in project root)
        "OpenSans-Regular.ttf",
        "assets/fonts/OpenSans-Regular.ttf",
        // System fallback (Monospace preferred for pixel-like aesthetic)
        "C:/Windows/Fonts/lucon.ttf",
        "C:/Windows/Fonts/cour.ttf"
    };

    if (std::any_of(
            K_FONT_PATHS.begin(), K_FONT_PATHS.end(),
            [this](const auto &path) { return font_.loadFromFile(path); })) {
        // Attempt to disable smoothing for the font texture (affects all sizes usually)
        // Note: SFML generates textures on the beam, so this is best-effort.
        return true;
    }

    return false;
}

auto UI::handle_event(sf::RenderWindow &window, const sf::Event &event,
                      size_t &maze_rows, size_t &maze_cols,
                      size_t &display_rows, size_t &display_cols,
                      bool & /*is_generating*/, bool & /*is_solving*/,
                      bool &is_paused, int &solver_type, int &gen_type, int &animation_speed,
                      const std::function<void(bool)> &start_gen,
                      const std::function<void()>     &start_sol,
                      const std::function<void()>     &step_fn,
                      const std::function<void()>     &reset_fn,
                      const std::function<void(size_t, size_t, int)> &toggle_wall_fn,
                      const std::function<void()>     &save_fn,
                      const std::function<void()>     &load_fn,
                      float &zoom_factor, sf::Vector2f &camera_offset) -> void {

    const sf::Vector2f MPOS =
        window.mapPixelToCoords(sf::Mouse::getPosition(window));
    const auto &l = layout_;

    static constexpr size_t K_MIN_MAZE_SIZE  = 5;
    static constexpr size_t K_MAX_MAZE_SIZE  = 100;
    static constexpr int    K_SPEED_STEP     = 5;
    static constexpr int    K_MIN_ANIM_SPEED = 1;
    static constexpr int    K_MAX_ANIM_SPEED = 200;

    static bool is_panning = false;
    static sf::Vector2f last_mouse_pos;

    // Handle Zoom
    if (event.type == sf::Event::MouseWheelScrolled && event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
        float delta = event.mouseWheelScroll.delta;
        zoom_factor = std::clamp(zoom_factor + delta * 0.1f, 0.5f, 5.0f);
    }

    // Handle Panning
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Middle) {
        is_panning = true;
        last_mouse_pos = MPOS;
    } else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Middle) {
        is_panning = false;
    } else if (event.type == sf::Event::MouseMoved && is_panning) {
        sf::Vector2f delta = MPOS - last_mouse_pos;
        camera_offset += delta;
        last_mouse_pos = MPOS;
    }

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
        const bool CLICKED_ROW = l.row_input_box.contains(MPOS);
        const bool CLICKED_COL = l.col_input_box.contains(MPOS);
        const bool CLICKED_SPEED = l.speed_input_box.contains(MPOS);

        // Commit speed if focus is lost
        if (speed_focused_ && !CLICKED_SPEED) {
            commit_speed();
        }

        row_focused_ = CLICKED_ROW;
        col_focused_ = CLICKED_COL;
        speed_focused_ = CLICKED_SPEED;

        if (l.apply_btn.contains(MPOS)) {
            commit_dimensions();
        } else if (l.gen_inst_btn.contains(MPOS)) start_gen(false);
        else if (l.gen_step_btn.contains(MPOS)) start_gen(true);
        else if (l.recursive_gen_btn.contains(MPOS)) gen_type = 0;
        else if (l.ellers_gen_btn.contains(MPOS)) gen_type = 1;
        else if (l.bfs_btn.contains(MPOS)) solver_type = 0;
        else if (l.dfs_btn.contains(MPOS)) solver_type = 1;
        else if (l.astar_btn.contains(MPOS)) solver_type = 2;
        else if (l.flood_btn.contains(MPOS)) solver_type = 3;
        else if (l.wall_btn.contains(MPOS)) solver_type = 4;
        else if (l.solve_btn.contains(MPOS)) start_sol();
        else if (l.solve_inst_btn.contains(MPOS)) start_sol();
        else if (l.play_btn.contains(MPOS)) is_paused = !is_paused;
        else if (l.step_btn.contains(MPOS)) { is_paused = true; step_fn(); }
        else if (l.reset_btn.contains(MPOS)) reset_fn();
        else if (l.save_btn.contains(MPOS)) save_fn();
        else if (l.load_btn.contains(MPOS)) load_fn();
        else {
            // Check if clicked in maze area
            const float WINDOW_WIDTH = static_cast<float>(window.getSize().x);
            const float WINDOW_HEIGHT = static_cast<float>(window.getSize().y);
            const float SIDEBAR_WIDTH = std::clamp(WINDOW_WIDTH * 0.2f, 200.0f, 350.0f);
            const float AVAILABLE_WIDTH = WINDOW_WIDTH - SIDEBAR_WIDTH - 40.0f;
            const float AVAILABLE_HEIGHT = WINDOW_HEIGHT - 40.0f;
            
            const float BASE_CELL_SIZE_W = AVAILABLE_WIDTH / static_cast<float>(maze_cols);
            const float BASE_CELL_SIZE_H = AVAILABLE_HEIGHT / static_cast<float>(maze_rows);
            const float BASE_CELL_SIZE = std::min({BASE_CELL_SIZE_W, BASE_CELL_SIZE_H, 40.0f});
            
            const float CELL_SIZE = BASE_CELL_SIZE * zoom_factor;
            const float MAZE_W = static_cast<float>(maze_cols) * CELL_SIZE;
            const float MAZE_H = static_cast<float>(maze_rows) * CELL_SIZE;
            const float OFFSET_X = SIDEBAR_WIDTH + (WINDOW_WIDTH - SIDEBAR_WIDTH - MAZE_W) / 2.0F + camera_offset.x;
            const float OFFSET_Y = (WINDOW_HEIGHT - MAZE_H) / 2.0F + camera_offset.y;

            float rel_x = MPOS.x - OFFSET_X;
            float rel_y = MPOS.y - OFFSET_Y;

            if (rel_x >= 0 && rel_x < MAZE_W && rel_y >= 0 && rel_y < MAZE_H) {
                size_t c = static_cast<size_t>(rel_x / CELL_SIZE);
                size_t r = static_cast<size_t>(rel_y / CELL_SIZE);
                
                float cell_rel_x = fmod(rel_x, CELL_SIZE);
                float cell_rel_y = fmod(rel_y, CELL_SIZE);

                // Proximity threshold for clicking a wall (25% of cell size)
                const float THRESHOLD = CELL_SIZE * 0.25f;

                // Determine which wall is closest
                float dist_top = cell_rel_y;
                float dist_bottom = CELL_SIZE - cell_rel_y;
                float dist_left = cell_rel_x;
                float dist_right = CELL_SIZE - cell_rel_x;

                float min_dist = std::min({dist_top, dist_bottom, dist_left, dist_right});
                
                if (min_dist < THRESHOLD) {
                    if (min_dist == dist_top) toggle_wall_fn(r, c, 0);
                    else if (min_dist == dist_right) toggle_wall_fn(r, c, 1);
                    else if (min_dist == dist_bottom) toggle_wall_fn(r, c, 2);
                    else if (min_dist == dist_left) toggle_wall_fn(r, c, 3);
                }
            }
        }
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
              bool is_paused, int solver_type, int gen_type,
              int animation_speed, const GeneratorInterface &maze_gen,
              const sf::Vector2f &mouse_pos,
              float zoom_factor, const sf::Vector2f &camera_offset,
              const std::vector<int>& grid_values,
              int heading) -> void {

    draw_sidebar(window, display_rows, display_cols, is_generating, is_solving,
                 is_paused, solver_type, gen_type, exploration_path.size(), 
                 solved_path.size(), animation_speed, mouse_pos);

    draw_maze(window, maze_rows, maze_cols, grid, exploration_path, 
              solved_path, is_generating, maze_gen, grid_values, heading,
              zoom_factor, camera_offset);
}
auto UI::draw_sidebar(sf::RenderWindow &window, size_t display_rows,
                      size_t display_cols, bool is_generating, bool is_solving,
                      bool is_paused, int solver_type, int gen_type,
                      size_t exploration_count, size_t solved_count,
                      int animation_speed,
                      const sf::Vector2f &mouse_pos) -> void {
    // Calculate responsive dimensions
    const float WINDOW_WIDTH = static_cast<float>(window.getSize().x);
    const float WINDOW_HEIGHT = static_cast<float>(window.getSize().y);
    const float SIDEBAR_WIDTH = std::clamp(WINDOW_WIDTH * 0.2f, 200.0f, 350.0f);
    const float PADDING = std::clamp(WINDOW_WIDTH * 0.015f, 12.0f, 24.0f);
    
    // Responsive font sizes
    const unsigned int HEADER_SIZE = static_cast<unsigned int>(std::clamp(WINDOW_HEIGHT * 0.03f, 18.0f, 26.0f));
    const unsigned int SECTION_SIZE = static_cast<unsigned int>(std::clamp(WINDOW_HEIGHT * 0.02f, 12.0f, 16.0f));
    const unsigned int TEXT_SIZE = static_cast<unsigned int>(std::clamp(WINDOW_HEIGHT * 0.018f, 11.0f, 14.0f));
    const unsigned int STATS_SIZE = static_cast<unsigned int>(std::clamp(WINDOW_HEIGHT * 0.017f, 10.0f, 13.0f));
    
    sf::RectangleShape sb_bg(
        sf::Vector2f(SIDEBAR_WIDTH, WINDOW_HEIGHT));
    sb_bg.setFillColor(Theme::SIDEBAR);
    window.draw(sb_bg);

    float cx = PADDING;
    float cy = PADDING;

    // Header
    sf::Text header("MICROMOUSE", font_, HEADER_SIZE);
    header.setStyle(sf::Text::Bold);
    header.setFillColor(Theme::PRIMARY);
    header.setPosition(cx, cy);
    window.draw(header);
    cy += HEADER_SIZE + PADDING * 1.5f;

    // Section: Maze Configuration
    sf::Text cfg_title("MAZE CONFIG", font_, SECTION_SIZE);
    cfg_title.setFillColor(Theme::TEXT_DIM);
    cfg_title.setPosition(cx, cy);
    window.draw(cfg_title);
    cy += SECTION_SIZE + PADDING * 0.5f;

    const float INPUT_HEIGHT = std::clamp(WINDOW_HEIGHT * 0.05f, 32.0f, 40.0f);
    const float INPUT_BOX_WIDTH = SIDEBAR_WIDTH - PADDING * 2.0f;
    
    UIComponents::draw_input_box(window, font_, "Rows", row_input_buffer_, layout_.row_input_box, cx, cy, INPUT_BOX_WIDTH, row_focused_);
    cy += INPUT_HEIGHT;
    UIComponents::draw_input_box(window, font_, "Cols", col_input_buffer_, layout_.col_input_box, cx, cy, INPUT_BOX_WIDTH, col_focused_);
    cy += INPUT_HEIGHT + PADDING * 0.4f;

    UIComponents::draw_btn(window, font_, "APPLY DIMENSIONS", layout_.apply_btn, cx, cy,
             SIDEBAR_WIDTH - PADDING * 2.0f, false, mouse_pos);
    cy += INPUT_HEIGHT + PADDING * 0.5f;

    // Section: Speed Control
    sf::Text spd_title("ANIMATION SPEED", font_, SECTION_SIZE);
    spd_title.setFillColor(Theme::TEXT_DIM);
    spd_title.setPosition(cx, cy);
    window.draw(spd_title);
    cy += SECTION_SIZE + PADDING * 0.5f;

    UIComponents::draw_input_box(window, font_, "Speed", speed_input_buffer_, layout_.speed_input_box, cx, cy, INPUT_BOX_WIDTH, speed_focused_);
    cy += INPUT_HEIGHT + PADDING;

    // Section: Generation
    sf::Text gen_title("GENERATION", font_, SECTION_SIZE);
    gen_title.setFillColor(Theme::TEXT_DIM);
    gen_title.setPosition(cx, cy);
    window.draw(gen_title);
    cy += SECTION_SIZE + PADDING * 0.5f;
    const float BTN_SPACING = PADDING * 0.5f;
    const float BTN_WIDTH = (SIDEBAR_WIDTH - PADDING * 2.0f - BTN_SPACING) / 2.0F;
    const float BTN_HEIGHT_SPACING = std::clamp(WINDOW_HEIGHT * 0.062f, 38.0f, 50.0f);
    UIComponents::draw_btn(window, font_, "Quick Gen", layout_.gen_inst_btn, cx, cy,
             BTN_WIDTH, false, mouse_pos);
    UIComponents::draw_btn(window, font_, "Step Gen", layout_.gen_step_btn,
             cx + BTN_WIDTH + BTN_SPACING, cy,
             BTN_WIDTH, false, mouse_pos);
    cy += BTN_HEIGHT_SPACING - PADDING * 0.2f;

    UIComponents::draw_btn(window, font_, "Recursive", layout_.recursive_gen_btn, cx, cy,
             BTN_WIDTH, gen_type == 0, mouse_pos);
    UIComponents::draw_btn(window, font_, "Eller's", layout_.ellers_gen_btn,
             cx + BTN_WIDTH + BTN_SPACING, cy,
             BTN_WIDTH, gen_type == 1, mouse_pos);
    cy += BTN_HEIGHT_SPACING;

    // Section: Solver Algorithms
    sf::Text sol_title("ALGORITHMS", font_, SECTION_SIZE);
    sol_title.setFillColor(Theme::TEXT_DIM);
    sol_title.setPosition(cx, cy);
    window.draw(sol_title);
    cy += SECTION_SIZE + PADDING * 0.5f;
    const float ALGO_BTN_WIDTH = (SIDEBAR_WIDTH - PADDING * 2.0f - BTN_SPACING) / 2.0F;
    UIComponents::draw_btn(window, font_, "BFS", layout_.bfs_btn, cx, cy, ALGO_BTN_WIDTH, solver_type == 0,
             mouse_pos);
    UIComponents::draw_btn(window, font_, "DFS", layout_.dfs_btn, cx + ALGO_BTN_WIDTH + BTN_SPACING, cy, ALGO_BTN_WIDTH,
             solver_type == 1, mouse_pos);
    cy += BTN_HEIGHT_SPACING - PADDING * 0.2f;
    UIComponents::draw_btn(window, font_, "A*", layout_.astar_btn, cx, cy, ALGO_BTN_WIDTH, solver_type == 2,
             mouse_pos);
    UIComponents::draw_btn(window, font_, "Flood Fill", layout_.flood_btn, cx + ALGO_BTN_WIDTH + BTN_SPACING, cy,
             ALGO_BTN_WIDTH, solver_type == 3, mouse_pos);
    cy += BTN_HEIGHT_SPACING - PADDING * 0.2f;
    UIComponents::draw_btn(window, font_, "Wall Follow", layout_.wall_btn, cx, cy,
             SIDEBAR_WIDTH - PADDING * 2.0f, solver_type == 4, mouse_pos);
    cy += BTN_HEIGHT_SPACING;
    UIComponents::draw_btn(window, font_, "ANIMATE SOLVE", layout_.solve_btn, cx, cy, ALGO_BTN_WIDTH, false,
             mouse_pos);
    UIComponents::draw_btn(window, font_, "INSTANT SOLVE", layout_.solve_inst_btn,
             cx + ALGO_BTN_WIDTH + BTN_SPACING, cy, ALGO_BTN_WIDTH, false, mouse_pos);
    cy += BTN_HEIGHT_SPACING;

    // Section: Global Controls
    std::string pl = is_paused ? "RESUME" : "PAUSE";
    if (!is_generating && !is_solving) pl = "START";
    UIComponents::draw_btn(window, font_, pl, layout_.play_btn, cx, cy, ALGO_BTN_WIDTH,
             !is_paused && (is_generating || is_solving), mouse_pos);
    UIComponents::draw_btn(window, font_, "STEP", layout_.step_btn, cx + ALGO_BTN_WIDTH + BTN_SPACING, cy, ALGO_BTN_WIDTH,
             false, mouse_pos);
    cy += BTN_HEIGHT_SPACING - PADDING * 0.2f;
    UIComponents::draw_btn(window, font_, "RESET SIMULATOR", layout_.reset_btn, cx, cy,
             SIDEBAR_WIDTH - PADDING * 2.0f, false, mouse_pos);
    cy += BTN_HEIGHT_SPACING;

    // Section: File Operations
    sf::Text file_title("FILE OPERATIONS", font_, SECTION_SIZE);
    file_title.setFillColor(Theme::TEXT_DIM);
    file_title.setPosition(cx, cy);
    window.draw(file_title);
    cy += SECTION_SIZE + PADDING * 0.5f;

    UIComponents::draw_btn(window, font_, "EXPORT MAZE", layout_.save_btn, cx, cy, ALGO_BTN_WIDTH, false, mouse_pos);
    UIComponents::draw_btn(window, font_, "OPEN MAZE", layout_.load_btn, cx + ALGO_BTN_WIDTH + BTN_SPACING, cy, ALGO_BTN_WIDTH, false, mouse_pos);
    cy += BTN_HEIGHT_SPACING;

    // Section: Stats Card
    // Section: Stats Card
    if (exploration_count > 0 || solved_count > 0) {
        const float CARD_HEIGHT = std::clamp(WINDOW_HEIGHT * 0.08f, 50.0f, 70.0f);
        sf::RectangleShape card(sf::Vector2f(SIDEBAR_WIDTH - PADDING * 2.0f, CARD_HEIGHT));
        card.setPosition(cx, cy);
        card.setFillColor(sf::Color(15, 23, 42, 180));
        card.setOutlineThickness(1.0F);
        card.setOutlineColor(Theme::SURFACE);
        window.draw(card);

        std::stringstream ss;
        ss << "Explored: " << exploration_count << "\n";
        if (solver_type == 4) { // Wall Follower
             ss << "Actual Path: " << solved_count;
        } else {
             ss << "Final Path: " << solved_count;
        }
        
        sf::Text st(ss.str(), font_, STATS_SIZE);
        st.setFillColor(Theme::TEXT_MAIN);
        st.setPosition(cx + 12.0F, cy + 12.0F);
        window.draw(st);
    }
}

auto UI::draw_maze(sf::RenderWindow &window, size_t rows, size_t cols,
                   const std::vector<Cell>   &grid,
                   const std::vector<size_t> &exploration_path,
                   const std::vector<size_t> &solved_path,
                   bool is_generating,
                   const GeneratorInterface &maze_gen,
                   const std::vector<int>& grid_values,
                   int heading,
                   float zoom_factor, const sf::Vector2f &camera_offset) -> void {
    if (grid.empty()) return;

    // Calculate responsive dimensions
    const float WINDOW_WIDTH = static_cast<float>(window.getSize().x);
    const float WINDOW_HEIGHT = static_cast<float>(window.getSize().y);
    const float SIDEBAR_WIDTH = std::clamp(WINDOW_WIDTH * 0.2f, 200.0f, 350.0f);
    const float AVAILABLE_WIDTH = WINDOW_WIDTH - SIDEBAR_WIDTH - 40.0f;
    const float AVAILABLE_HEIGHT = WINDOW_HEIGHT - 40.0f;
    
    // Scale base cell size by zoom factor
    const float BASE_CELL_SIZE_W = AVAILABLE_WIDTH / static_cast<float>(cols);
    const float BASE_CELL_SIZE_H = AVAILABLE_HEIGHT / static_cast<float>(rows);
    const float BASE_CELL_SIZE = std::min({BASE_CELL_SIZE_W, BASE_CELL_SIZE_H, 40.0f});
    
    const float CELL_SIZE = BASE_CELL_SIZE * zoom_factor;
    const float WALL_THICKNESS = std::max(1.0f, CELL_SIZE * 0.08f);
    const float MAZE_W = static_cast<float>(cols) * CELL_SIZE;
    const float MAZE_H = static_cast<float>(rows) * CELL_SIZE;
    const float OFFSET_X = SIDEBAR_WIDTH + (WINDOW_WIDTH - SIDEBAR_WIDTH - MAZE_W) / 2.0F + camera_offset.x;
    const float OFFSET_Y = (WINDOW_HEIGHT - MAZE_H) / 2.0F + camera_offset.y;

    // --- Performance Optimization: Frustum Culling ---
    // Calculate the range of cells currently visible in the window
    int start_col = std::max(0, static_cast<int>((-OFFSET_X + SIDEBAR_WIDTH) / CELL_SIZE));
    int end_col   = std::min(static_cast<int>(cols) - 1, static_cast<int>((WINDOW_WIDTH - OFFSET_X) / CELL_SIZE));
    int start_row = std::max(0, static_cast<int>(-OFFSET_Y / CELL_SIZE));
    int end_row   = std::min(static_cast<int>(rows) - 1, static_cast<int>((WINDOW_HEIGHT - OFFSET_Y) / CELL_SIZE));

    // If no cells are visible, we can skip most of the drawing
    if (start_col > end_col || start_row > end_row) return;

    // 1. Draw Grid Cells using VertexArray for batching
    sf::VertexArray cell_va(sf::Quads);
    for (int r = start_row; r <= end_row; ++r) {
        const size_t ROW_OFFSET = r * cols;
        for (int c = start_col; c <= end_col; ++c) {
            const size_t IDX = ROW_OFFSET + c;
            float x = OFFSET_X + c * CELL_SIZE;
            float y = OFFSET_Y + r * CELL_SIZE;
            
            sf::Color color;
            if (grid[IDX].visited) {
                color = Theme::VISITED_CELL;
            } else {
                color = sf::Color(30, 41, 59, 40);
            }
            if (IDX == 0) color = sf::Color(34, 197, 94);
            else if (IDX == (rows * cols - 1)) color = Theme::ACCENT;

            cell_va.append(sf::Vertex(sf::Vector2f(x, y), color));
            cell_va.append(sf::Vertex(sf::Vector2f(x + CELL_SIZE, y), color));
            cell_va.append(sf::Vertex(sf::Vector2f(x + CELL_SIZE, y + CELL_SIZE), color));
            cell_va.append(sf::Vertex(sf::Vector2f(x, y + CELL_SIZE), color));
        }
    }
    window.draw(cell_va);

    // 2. Draw Maze Generation Animation Path
    if (is_generating) {
        const auto &gp = maze_gen.get_path();
        if (gp.size() > 1) {
            for (size_t i = 0; i < gp.size() - 1; ++i) {
                sf::Vertex line[] = {
                    sf::Vertex(sf::Vector2f(OFFSET_X + gp[i].second * CELL_SIZE + CELL_SIZE / 2.0F,
                                            OFFSET_Y + gp[i].first * CELL_SIZE + CELL_SIZE / 2.0F),
                               Theme::PRIMARY),
                    sf::Vertex(sf::Vector2f(OFFSET_X + gp[i+1].second * CELL_SIZE + CELL_SIZE / 2.0F,
                                            OFFSET_Y + gp[i+1].first * CELL_SIZE + CELL_SIZE / 2.0F),
                               Theme::PRIMARY)};
                window.draw(line, 2, sf::Lines);
            }
            sf::CircleShape h(CELL_SIZE * 0.35F);
            h.setOrigin(h.getRadius(), h.getRadius());
            h.setFillColor(sf::Color::White);
            h.setPosition(OFFSET_X + gp.back().second * CELL_SIZE + CELL_SIZE / 2.0F,
                          OFFSET_Y + gp.back().first * CELL_SIZE + CELL_SIZE / 2.0F);
            window.draw(h);
        }
    }

    // 3. Draw Exploration Data as Dots
    if (!exploration_path.empty()) {
        const float DOT_RADIUS = std::max(2.0f, CELL_SIZE * 0.22f);
        sf::CircleShape dot(DOT_RADIUS);
        dot.setOrigin(DOT_RADIUS, DOT_RADIUS);
        dot.setFillColor(Theme::EXPLORATION_COLOR);
        for (const auto NODE_IDX : exploration_path) {
            auto [r, c] = MazeUtils::get_2d_coords(NODE_IDX, cols);
            dot.setPosition(OFFSET_X + c * CELL_SIZE + CELL_SIZE / 2.0F,
                            OFFSET_Y + r * CELL_SIZE + CELL_SIZE / 2.0F);
            window.draw(dot);
        }

        // Highlight head
        size_t head_node = exploration_path.back();
        auto [hr, hc] = MazeUtils::get_2d_coords(head_node, cols);
        float hx = OFFSET_X + hc * CELL_SIZE + CELL_SIZE / 2.0F;
        float hy = OFFSET_Y + hr * CELL_SIZE + CELL_SIZE / 2.0F;

        if (heading >= 0) {
            // Draw a directional triangle
            sf::ConvexShape triangle(3);
            float r = CELL_SIZE * 0.4f;
            triangle.setPoint(0, sf::Vector2f(0, -r));
            triangle.setPoint(1, sf::Vector2f(r * 0.866f, r * 0.5f));
            triangle.setPoint(2, sf::Vector2f(-r * 0.866f, r * 0.5f));
            triangle.setFillColor(sf::Color::Yellow);
            triangle.setPosition(hx, hy);
            triangle.setRotation(static_cast<float>(heading * 90));
            window.draw(triangle);
        } else {
            // Draw a rectangle
            sf::RectangleShape head(sf::Vector2f(CELL_SIZE, CELL_SIZE));
            head.setPosition(OFFSET_X + hc * CELL_SIZE, OFFSET_Y + hr * CELL_SIZE);
            head.setFillColor(sf::Color(255, 255, 255, 100));
            window.draw(head);
        }
    }

    // New: Draw Grid Values (Flood Fill Distances)
    if (!grid_values.empty() && grid_values.size() == rows * cols) {
        // Use a larger base size for better quality when scaling down
        unsigned int base_size = 32;
        
        // Disable smoothing for crisp pixel font rendering
        // We need to const_cast because SFML's getTexture returns a const reference
        const_cast<sf::Texture&>(font_.getTexture(base_size)).setSmooth(false);

        sf::Text txt("", font_, base_size); 
        txt.setFillColor(sf::Color::White); // White text
        txt.setOutlineColor(sf::Color::Black); // Black outline
        txt.setOutlineThickness(2.0f); // Thick outline for contrast
        txt.setStyle(sf::Text::Bold);

        for (int r = start_row; r <= end_row; ++r) {
            const size_t ROW_OFFSET = r * cols;
            for (int c = start_col; c <= end_col; ++c) {
                int val = grid_values[ROW_OFFSET + c];
                // Only draw reachable/relevant values
                if (val < static_cast<int>(rows * cols)) { 
                    txt.setString(std::to_string(val));
                    sf::FloatRect b = txt.getLocalBounds();
                    
                    // Reset scale
                    txt.setScale(1.0f, 1.0f);
                    
                    // Calculate scale to fit within the cell
                    float max_w = CELL_SIZE * 0.75f; // Slightly more padding
                    float max_h = CELL_SIZE * 0.75f;
                    float scale = 1.0f;
                    
                    if (b.width > 0 && b.height > 0) {
                         float s_w = max_w / b.width;
                         float s_h = max_h / b.height;
                         scale = std::min(s_w, s_h);
                    }
                    
                    // Cap the max scale so numbers don't look huge in large cells
                    scale = std::min(scale, 0.8f);
                    // Also don't draw if tiny (sub-pixel)
                    if (scale * base_size < 2.0f) continue;

                    txt.setScale(scale, scale);

                    // Re-center
                    txt.setOrigin(b.left + b.width/2.0f, b.top + b.height/2.0f);
                    txt.setPosition(OFFSET_X + c * CELL_SIZE + CELL_SIZE/2.0f,
                                    OFFSET_Y + r * CELL_SIZE + CELL_SIZE/2.0f);
                    
                    window.draw(txt);
                }
            }
        }
    }

    // 4. Overlay Solution Path as Ribbon
    if (!solved_path.empty()) {
        const float RIBBON_THICK = std::max(3.0f, CELL_SIZE * 0.25f);
        for (size_t i = 0; i < solved_path.size() - 1; ++i) {
            auto [r1, c1] = MazeUtils::get_2d_coords(solved_path[i], cols);
            auto [r2, c2] = MazeUtils::get_2d_coords(solved_path[i+1], cols);
            float x1 = OFFSET_X + c1 * CELL_SIZE + CELL_SIZE / 2.0F, y1 = OFFSET_Y + r1 * CELL_SIZE + CELL_SIZE / 2.0F;
            float x2 = OFFSET_X + c2 * CELL_SIZE + CELL_SIZE / 2.0F, y2 = OFFSET_Y + r2 * CELL_SIZE + CELL_SIZE / 2.0F;
            float dx = x2 - x1, dy = y2 - y1;
            float length = std::sqrt(dx*dx + dy*dy);
            float angle = std::atan2(dy, dx) * 180.0F / 3.14159F;
            sf::RectangleShape seg(sf::Vector2f(length, RIBBON_THICK));
            seg.setOrigin(0, RIBBON_THICK / 2.0F);
            seg.setPosition(x1, y1);
            seg.setRotation(angle);
            seg.setFillColor(Theme::PATH_COLOR);
            window.draw(seg);
        }
    }

    // 5. Draw Walls
    sf::RectangleShape wh(sf::Vector2f(CELL_SIZE + WALL_THICKNESS, WALL_THICKNESS));
    sf::RectangleShape wv(sf::Vector2f(WALL_THICKNESS, CELL_SIZE + WALL_THICKNESS));
    wh.setFillColor(Theme::WALL_COLOR);
    wv.setFillColor(Theme::WALL_COLOR);

    // 5. Draw Walls using VertexArray
    sf::VertexArray wall_va(sf::Quads);
    for (int r = start_row; r <= end_row; ++r) {
        const size_t ROW_OFFSET = r * cols;
        for (int c = start_col; c <= end_col; ++c) {
            const auto &cell = grid[ROW_OFFSET + c];
            float px = OFFSET_X + c * CELL_SIZE, py = OFFSET_Y + r * CELL_SIZE;
            
            auto append_wall = [&](float x, float y, float w, float h) {
                wall_va.append(sf::Vertex(sf::Vector2f(x, y), Theme::WALL_COLOR));
                wall_va.append(sf::Vertex(sf::Vector2f(x + w, y), Theme::WALL_COLOR));
                wall_va.append(sf::Vertex(sf::Vector2f(x + w, y + h), Theme::WALL_COLOR));
                wall_va.append(sf::Vertex(sf::Vector2f(x, y + h), Theme::WALL_COLOR));
            };

            if (cell.top)    append_wall(px - WALL_THICKNESS/2.0f, py - WALL_THICKNESS/2.0f, CELL_SIZE + WALL_THICKNESS, WALL_THICKNESS);
            if (cell.bottom) append_wall(px - WALL_THICKNESS/2.0f, py + CELL_SIZE - WALL_THICKNESS/2.0f, CELL_SIZE + WALL_THICKNESS, WALL_THICKNESS);
            if (cell.left)   append_wall(px - WALL_THICKNESS/2.0f, py - WALL_THICKNESS/2.0f, WALL_THICKNESS, CELL_SIZE + WALL_THICKNESS);
            if (cell.right)  append_wall(px + CELL_SIZE - WALL_THICKNESS/2.0f, py - WALL_THICKNESS/2.0f, WALL_THICKNESS, CELL_SIZE + WALL_THICKNESS);
        }
    }
    window.draw(wall_va);
}


