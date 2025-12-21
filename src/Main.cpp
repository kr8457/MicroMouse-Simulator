#include "MazeGen.hpp"
#include "MazeSolver.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include <chrono>
#include <cstddef>  // for size_t
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {

// Define constants for rendering (UPPER_CASE as requested by clang-tidy)
constexpr float K_CELL_SIZE      = 25.0F;
constexpr float K_WALL_THICKNESS = 2.0F;
constexpr float K_PADDING        = 10.0F;
constexpr float K_UI_HEIGHT      = 140.0F;
constexpr float K_TEXT_SIZE      = 14.0F;
constexpr float K_BUTTON_WIDTH   = 110.0F;
constexpr float K_BUTTON_HEIGHT  = 25.0F;

// UI Layout Constants
constexpr float K_BTN_PLAY_WIDTH  = 80.0F;
constexpr float K_BTN_SMALL_WIDTH = 60.0F;
constexpr float K_BTN_WALL_WIDTH  = 100.0F;
constexpr float K_BTN_SOLVE_SCALE = 1.5F;
constexpr float K_BTN_SOLVE_WIDTH = K_BUTTON_WIDTH * K_BTN_SOLVE_SCALE;

// Magic numbers replacement
constexpr size_t K_DEFAULT_ROWS            = 20;
constexpr size_t K_DEFAULT_COLS            = 30;
constexpr int    K_ANIMATION_FAST_INTERVAL = 5;
constexpr int    K_ANIMATION_SLOW_INTERVAL = 50;

// Global maze dimensions
// Made static to enforce internal linkage
static size_t g_maze_rows = K_DEFAULT_ROWS;
static size_t g_maze_cols = K_DEFAULT_COLS;

// Global maze data
static MazeGen             g_maze_gen;  // Stateful generator
static std::vector<Cell>   g_maze_grid;
static Graph               g_maze_graph;
static std::vector<size_t> g_solved_path;

enum class SolverType : std::uint8_t { BFS, DFS, WALL_FOLLOWER };

// Global Animation State
static bool       g_is_generating = false;
static bool       g_is_solving    = false;
static bool       g_is_paused     = true;
static size_t     g_anim_step_idx = 0;
static double     g_last_solve_ms = 0.0;
static SolverType g_solver_type   = SolverType::BFS;
static sf::Clock  g_anim_clock;

// UI Layout
struct UILayout {
    sf::FloatRect gen_inst_btn;
    sf::FloatRect gen_step_btn;
    sf::FloatRect play_btn;
    sf::FloatRect step_btn;

    sf::FloatRect bfs_btn;
    sf::FloatRect dfs_btn;
    sf::FloatRect wall_btn;
    sf::FloatRect solve_btn;
};
static UILayout g_ui_layout;

// Forward Function Decls
static void start_generation(bool animated);
static void start_solve();

// Drawing helpers
static void draw_grid_cells(sf::RenderWindow        &window,
                            const std::vector<Cell> &grid, size_t rows,
                            size_t cols);
static void draw_path(sf::RenderWindow &window, size_t cols,
                      const std::vector<size_t> &path, size_t highlight_idx);
static void draw_walls(sf::RenderWindow &window, const std::vector<Cell> &grid,
                       size_t rows, size_t cols);
static void draw_maze(sf::RenderWindow &window, const std::vector<Cell> &grid,
                      size_t rows, size_t cols, const std::vector<size_t> &path,
                      size_t current_path_highlight_idx);

static void draw_ui(sf::RenderWindow &window, sf::Font &font);
static void handle_ui_events(const sf::Event &event);
static void update_animation();

static void start_generation(bool animated) {
    g_maze_gen.initialize(g_maze_rows, g_maze_cols);
    g_maze_grid = g_maze_gen.get_grid();  // Initial state
    g_solved_path.clear();
    g_maze_graph = Graph();  // Clear graph

    g_is_solving    = false;
    g_anim_step_idx = 0;

    if (animated) {
        g_is_generating = true;
        g_is_paused     = true;
    } else {
        while (g_maze_gen.step()) { }
        g_maze_grid = g_maze_gen.get_grid();
        g_maze_graph =
            MazeSolver::convert_to_graph(g_maze_grid, g_maze_rows, g_maze_cols);
        g_is_generating = false;
        g_is_paused     = true;
    }
}

static void start_solve() {
    if (g_is_generating) {
        return;
    }

    if (g_maze_graph.empty() && !g_maze_grid.empty()) {
        g_maze_graph =
            MazeSolver::convert_to_graph(g_maze_grid, g_maze_rows, g_maze_cols);
    }

    g_solved_path.clear();
    g_anim_step_idx = 0;
    g_is_solving    = false;

    if (g_maze_grid.empty()) {
        std::cerr << "Error: No maze generated to solve.\n";
        return;
    }

    const size_t start_idx = MazeSolver::get_1d_index(0, 0, g_maze_cols);
    const size_t end_idx =
        MazeSolver::get_1d_index(g_maze_rows - 1, g_maze_cols - 1, g_maze_cols);

    const size_t total_nodes = g_maze_rows * g_maze_cols;

    auto t_start = std::chrono::high_resolution_clock::now();

    switch (g_solver_type) {
    case SolverType::BFS:
        g_solved_path = MazeSolver::bfs_solve(g_maze_graph, start_idx, end_idx,
                                              total_nodes);
        break;
    case SolverType::DFS:
        g_solved_path = MazeSolver::dfs_solve(g_maze_graph, start_idx, end_idx,
                                              total_nodes);
        break;
    case SolverType::WALL_FOLLOWER:
        g_solved_path = MazeSolver::wall_follower_solve(
            g_maze_grid, g_maze_rows, g_maze_cols, start_idx, end_idx);
        break;
    }

    auto t_end = std::chrono::high_resolution_clock::now();
    g_last_solve_ms =
        std::chrono::duration<double, std::milli>(t_end - t_start).count();

    if (!g_solved_path.empty()) {
        g_is_solving = true;
        g_is_paused  = false;
        g_anim_clock.restart();
    } else {
        std::cout << "Solver could not find a path.\n";
    }
}

static void do_step() {
    if (g_is_generating) {
        if (g_maze_gen.step()) {
            g_maze_grid = g_maze_gen.get_grid();
        } else {
            g_maze_grid  = g_maze_gen.get_grid();
            g_maze_graph = MazeSolver::convert_to_graph(
                g_maze_grid, g_maze_rows, g_maze_cols);
            g_is_generating = false;
        }
    } else if (g_is_solving) {
        if (g_anim_step_idx < g_solved_path.size()) {
            g_anim_step_idx++;
        } else {
            g_is_solving = false;
        }
    }
}

static void update_animation() {
    if (!g_is_paused) {
        const int interval = g_is_generating ? K_ANIMATION_FAST_INTERVAL
                                             : K_ANIMATION_SLOW_INTERVAL;
        if (g_anim_clock.getElapsedTime().asMilliseconds() > interval) {
            do_step();
            g_anim_clock.restart();
        }
    }
}

static void draw_grid_cells(sf::RenderWindow        &window,
                            const std::vector<Cell> &grid, size_t rows,
                            size_t cols) {
    const float        offset_y = K_PADDING + K_UI_HEIGHT;
    sf::RectangleShape cell_shape(sf::Vector2f(K_CELL_SIZE, K_CELL_SIZE));

    for (size_t rr = 0; rr < rows; ++rr) {
        for (size_t cc = 0; cc < cols; ++cc) {
            size_t node_idx = MazeSolver::get_1d_index(rr, cc, cols);
            cell_shape.setPosition(
                K_PADDING + static_cast<float>(cc) * K_CELL_SIZE,
                offset_y + static_cast<float>(rr) * K_CELL_SIZE);

            if (grid[node_idx].visited) {
                cell_shape.setFillColor(sf::Color(30, 30, 30));
            } else {
                cell_shape.setFillColor(sf::Color(50, 50, 50));
            }

            if (node_idx == MazeSolver::get_1d_index(0, 0, cols)) {
                cell_shape.setFillColor(sf::Color::Green);
            } else if (node_idx ==
                       MazeSolver::get_1d_index(rows - 1, cols - 1, cols)) {
                cell_shape.setFillColor(sf::Color::Red);
            }

            window.draw(cell_shape);
        }
    }
}

static void draw_path(sf::RenderWindow &window, size_t cols,
                      const std::vector<size_t> &path, size_t highlight_idx) {
    const float offset_y = K_PADDING + K_UI_HEIGHT;
    if (!path.empty()) {
        sf::RectangleShape path_segment(sf::Vector2f(K_CELL_SIZE, K_CELL_SIZE));
        path_segment.setFillColor(sf::Color(0, 150, 255, 150));
        for (size_t i = 0; i <= highlight_idx && i < path.size(); ++i) {
            size_t node_idx = path[i];
            auto [r, c]     = MazeSolver::get_2d_coords(node_idx, cols);
            path_segment.setPosition(
                K_PADDING + static_cast<float>(c) * K_CELL_SIZE,
                offset_y + static_cast<float>(r) * K_CELL_SIZE);
            window.draw(path_segment);
        }
    }
}

static void draw_walls(sf::RenderWindow &window, const std::vector<Cell> &grid,
                       size_t rows, size_t cols) {
    const float        offset_y = K_PADDING + K_UI_HEIGHT;
    sf::RectangleShape wall_h(
        sf::Vector2f(K_CELL_SIZE + K_WALL_THICKNESS, K_WALL_THICKNESS));
    sf::RectangleShape wall_v(
        sf::Vector2f(K_WALL_THICKNESS, K_CELL_SIZE + K_WALL_THICKNESS));
    wall_h.setFillColor(sf::Color::White);
    wall_v.setFillColor(sf::Color::White);

    for (size_t rr = 0; rr < rows; ++rr) {
        for (size_t cc = 0; cc < cols; ++cc) {
            size_t      node_idx = MazeSolver::get_1d_index(rr, cc, cols);
            const Cell &cell     = grid[node_idx];

            const float px = K_PADDING + static_cast<float>(cc) * K_CELL_SIZE;
            const float py = offset_y + static_cast<float>(rr) * K_CELL_SIZE;

            if (cell.top) {
                wall_h.setPosition(px, py);
                window.draw(wall_h);
            }
            if (cell.left) {
                wall_v.setPosition(px, py);
                window.draw(wall_v);
            }
            if (cc == cols - 1 && cell.right) {
                wall_v.setPosition(px + K_CELL_SIZE, py);
                window.draw(wall_v);
            }
        }
    }
    // Bottom walls for last row
    for (size_t cc = 0; cc < cols; ++cc) {
        size_t node_idx = MazeSolver::get_1d_index(rows - 1, cc, cols);
        if (grid[node_idx].bottom) {
            wall_h.setPosition(K_PADDING + static_cast<float>(cc) * K_CELL_SIZE,
                               offset_y +
                                   static_cast<float>(rows) * K_CELL_SIZE);
            window.draw(wall_h);
        }
    }
}

static void draw_maze(sf::RenderWindow &window, const std::vector<Cell> &grid,
                      size_t rows, size_t cols, const std::vector<size_t> &path,
                      size_t current_path_highlight_idx) {
    if (grid.empty()) {
        return;
    }
    draw_grid_cells(window, grid, rows, cols);
    if (g_is_solving || !g_is_generating) {
        draw_path(window, cols, path, current_path_highlight_idx);
    }
    draw_walls(window, grid, rows, cols);
}

static void draw_btn(sf::RenderWindow &window, sf::Font &font,
                     const std::string &label, sf::FloatRect &bounds,
                     float btn_x, float btn_y, float width = K_BUTTON_WIDTH,
                     bool active = false) {
    sf::RectangleShape shape(sf::Vector2f(width, K_BUTTON_HEIGHT));
    shape.setPosition(btn_x, btn_y);
    shape.setFillColor(active ? sf::Color(0, 100, 0) : sf::Color(70, 70, 70));
    window.draw(shape);
    bounds = shape.getGlobalBounds();

    sf::Text text(label, font, static_cast<unsigned int>(K_TEXT_SIZE));
    text.setFillColor(sf::Color::White);

    // Simple centering
    sf::FloatRect text_bounds = text.getLocalBounds();
    text.setPosition(
        btn_x + (width - text_bounds.width) / 2.0F,
        btn_y + (K_BUTTON_HEIGHT - text_bounds.height) / 2.0F - 4.0F);
    window.draw(text);
}

static void draw_ui(sf::RenderWindow &window, sf::Font &font) {
    sf::RectangleShape ui_bg(
        sf::Vector2f(static_cast<float>(window.getSize().x), K_UI_HEIGHT));
    ui_bg.setFillColor(sf::Color(20, 20, 20));
    window.draw(ui_bg);

    const float row1_y = K_PADDING;
    const float row2_y = K_PADDING + K_BUTTON_HEIGHT + K_PADDING;
    const float row3_y = K_PADDING + (K_BUTTON_HEIGHT + K_PADDING) * 2.0F;

    float cx = K_PADDING;

    // Row 1
    draw_btn(window, font, "Gen Instant", g_ui_layout.gen_inst_btn, cx, row1_y);
    cx += K_BUTTON_WIDTH + K_PADDING;
    draw_btn(window, font, "Gen Steps", g_ui_layout.gen_step_btn, cx, row1_y);
    cx += K_BUTTON_WIDTH + K_PADDING;

    std::string play_label = g_is_paused ? "Play" : "Pause";
    if (!g_is_generating && !g_is_solving) {
        play_label = "Play";
    }
    draw_btn(window, font, play_label, g_ui_layout.play_btn, cx, row1_y,
             K_BTN_PLAY_WIDTH,
             !g_is_paused && (g_is_generating || g_is_solving));
    cx += K_BTN_PLAY_WIDTH + K_PADDING;

    draw_btn(window, font, "Step", g_ui_layout.step_btn, cx, row1_y,
             K_BTN_SMALL_WIDTH);

    // Row 2
    cx = K_PADDING;
    sf::Text lbl("Solver:", font, static_cast<unsigned int>(K_TEXT_SIZE));
    lbl.setPosition(cx, row2_y + 4.0F);
    window.draw(lbl);
    cx += K_BTN_SMALL_WIDTH;

    draw_btn(window, font, "BFS", g_ui_layout.bfs_btn, cx, row2_y,
             K_BTN_SMALL_WIDTH, g_solver_type == SolverType::BFS);
    cx += K_BTN_SMALL_WIDTH + K_PADDING;

    draw_btn(window, font, "DFS", g_ui_layout.dfs_btn, cx, row2_y,
             K_BTN_SMALL_WIDTH, g_solver_type == SolverType::DFS);
    cx += K_BTN_SMALL_WIDTH + K_PADDING;

    draw_btn(window, font, "Wall Follow", g_ui_layout.wall_btn, cx, row2_y,
             K_BTN_WALL_WIDTH, g_solver_type == SolverType::WALL_FOLLOWER);

    // Row 3
    cx = K_PADDING;
    draw_btn(window, font, "Solve Maze", g_ui_layout.solve_btn, cx, row3_y,
             K_BTN_SOLVE_WIDTH);
    cx += K_BTN_SOLVE_WIDTH + K_PADDING;

    if (!g_solved_path.empty() || g_last_solve_ms > 0.0) {
        std::stringstream ss;
        ss << "Time: " << std::fixed << std::setprecision(3) << g_last_solve_ms
           << " ms  |  Path: " << g_solved_path.size();
        sf::Text stats_text(ss.str(), font,
                            static_cast<unsigned int>(K_TEXT_SIZE));
        stats_text.setFillColor(sf::Color::Yellow);
        stats_text.setPosition(cx,
                               row3_y + (K_BUTTON_HEIGHT - K_TEXT_SIZE) / 2.0F);
        window.draw(stats_text);
    }
}

static void handle_click(const sf::Vector2f &mpos) {
    if (g_ui_layout.gen_inst_btn.contains(mpos)) {
        start_generation(false);
    } else if (g_ui_layout.gen_step_btn.contains(mpos)) {
        start_generation(true);
    } else if (g_ui_layout.play_btn.contains(mpos)) {
        if (g_is_generating || g_is_solving) {
            g_is_paused = !g_is_paused;
        }
    } else if (g_ui_layout.step_btn.contains(mpos)) {
        if (g_is_generating || g_is_solving) {
            g_is_paused = true;
            do_step();
        }
    } else if (g_ui_layout.bfs_btn.contains(mpos)) {
        g_solver_type = SolverType::BFS;
        start_solve();
        g_is_paused = true;
    } else if (g_ui_layout.dfs_btn.contains(mpos)) {
        g_solver_type = SolverType::DFS;
        start_solve();
        g_is_paused = true;
    } else if (g_ui_layout.wall_btn.contains(mpos)) {
        g_solver_type = SolverType::WALL_FOLLOWER;
        start_solve();
        g_is_paused = true;
    } else if (g_ui_layout.solve_btn.contains(mpos)) {
        start_solve();
    }
}

static void handle_ui_events(const sf::Event &event) {
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        const float mx = static_cast<float>(event.mouseButton.x);
        const float my = static_cast<float>(event.mouseButton.y);
        handle_click(sf::Vector2f(mx, my));
    }
}

}  // namespace

auto main() -> int {
    const float window_width =
        static_cast<float>(g_maze_cols) * K_CELL_SIZE + K_PADDING * 2.0F;
    const float window_height = static_cast<float>(g_maze_rows) * K_CELL_SIZE +
                                K_PADDING * 2.0F +
                                K_UI_HEIGHT;

    sf::RenderWindow window(
        sf::VideoMode(static_cast<unsigned int>(window_width),
                      static_cast<unsigned int>(window_height)),
        "MicroMouse Simulator");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        // Fallback
        if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
            return 1;
        }
    }

    start_generation(false);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            handle_ui_events(event);
        }

        update_animation();

        window.clear(sf::Color(34, 34, 34));
        draw_ui(window, font);
        draw_maze(window, g_maze_grid, g_maze_rows, g_maze_cols, g_solved_path,
                  g_anim_step_idx);
        window.display();
    }
    return 0;
}
