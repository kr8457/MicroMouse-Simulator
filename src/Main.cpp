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
static constexpr int WINDOW_WIDTH  = 1280;
static constexpr int WINDOW_HEIGHT = 720;
static constexpr int MIN_WINDOW_WIDTH  = 800;
static constexpr int MIN_WINDOW_HEIGHT = 600;

auto main() -> int {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),
                            "MicroMouse Simulator Pro",
                            sf::Style::Default);
    window.setFramerateLimit(60);

    // Load and set application icon
    sf::Image icon;
    if (icon.loadFromFile("assets/icon.png")) {
        window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    } else if (icon.loadFromFile("../assets/icon.png")) {
        window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    }

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
                    
                    if (width < MIN_WINDOW_WIDTH || height < MIN_WINDOW_HEIGHT) {
                        width = std::max(width, static_cast<unsigned int>(MIN_WINDOW_WIDTH));
                        height = std::max(height, static_cast<unsigned int>(MIN_WINDOW_HEIGHT));
                        window.setSize(sf::Vector2u(width, height));
                    }
                    
                    // Update the view to match the new window size
                    sf::FloatRect visible_area(0.f, 0.f, 
                                             static_cast<float>(width), 
                                             static_cast<float>(height));
                    window.setView(sf::View(visible_area));
                }
                simulator.handle_event(window, event);
            }

            // Update simulation
            const float DELTA_TIME = delta_clock.restart().asSeconds();
            simulator.update_mouse_position(window);
            simulator.update(DELTA_TIME);

            // Render
            window.clear(Theme::BACKGROUND);
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
