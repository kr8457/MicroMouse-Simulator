/**
 * @file Main.cpp
 * @brief Entry point for the MicroMouse Simulator Pro.
 * @details Initializes the SFML environment and runs the main application loop.
 */

#include "Simulator.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <iostream>

// Application constants
static constexpr int K_WINDOW_WIDTH  = 1280;
static constexpr int K_WINDOW_HEIGHT = 720;
static constexpr int K_MIN_WINDOW_WIDTH  = 800;
static constexpr int K_MIN_WINDOW_HEIGHT = 600;

/**
 * @brief Application entry point.
 * @return 0 on successful exit, 1 on error.
 */
auto main() -> int {
    sf::RenderWindow window(sf::VideoMode(K_WINDOW_WIDTH, K_WINDOW_HEIGHT),
                            "MicroMouse Simulator Pro",
                            sf::Style::Default);
    window.setFramerateLimit(60);

    try {
        Simulator simulator;
        if (!simulator.initialize()) {
            return 1;
        }

        sf::Clock delta_clock;

        while (window.isOpen()) {
            sf::Event event{};
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
                if (event.type == sf::Event::Resized) {
                    // Enforce minimum window size
                    unsigned int width = event.size.width;
                    unsigned int height = event.size.height;
                    
                    if (width < K_MIN_WINDOW_WIDTH || height < K_MIN_WINDOW_HEIGHT) {
                        width = std::max(width, static_cast<unsigned int>(K_MIN_WINDOW_WIDTH));
                        height = std::max(height, static_cast<unsigned int>(K_MIN_WINDOW_HEIGHT));
                        window.setSize(sf::Vector2u(width, height));
                    }
                    
                    // Update the view to match the new window size
                    sf::FloatRect visibleArea(0.f, 0.f, 
                                             static_cast<float>(width), 
                                             static_cast<float>(height));
                    window.setView(sf::View(visibleArea));
                }
                simulator.handle_event(window, event);
            }

            // Update simulation
            const float delta_time = delta_clock.restart().asSeconds();
            simulator.update_mouse_position(window);
            simulator.update(delta_time);

            // Render
            window.clear(Theme::Background);
            simulator.render(window);
            window.display();
        }
    } catch (const std::exception &e) {
        std::cerr << "EXCEPTION: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "UNKNOWN EXCEPTION CAUGHT\n";
        return 1;
    }
}
