#pragma once

#include <SFML/Graphics/Color.hpp>

/**
 * @namespace Theme
 * @brief Defines the visual style and color palette of the application.
 * 
 * Implements a "Sunset" theme using deep purples for surfaces and 
 * vibrant oranges and roses for interactive elements and highlights.
 */
namespace Theme {
    // Deep Twilight/Purple base
    const sf::Color BACKGROUND = sf::Color(30, 27, 46);   ///< Deep Midnight Purple for the main window background.
    const sf::Color SIDEBAR    = sf::Color(45, 35, 66);   ///< Muted Plum color for the sidebar container.
    const sf::Color SURFACE    = sf::Color(68, 56, 100);  ///< Lighter Purple for buttons and panel surfaces.

    // The "Sun" Colors (Warm Tones)
    const sf::Color PRIMARY       = sf::Color(251, 146, 60);  ///< Bright Sunset Orange for primary actions and highlights.
    const sf::Color PRIMARY_HOVER = sf::Color(249, 115, 22);  ///< Deep Burnt Orange for primary buttons in hovered state.
    const sf::Color ACCENT        = sf::Color(244, 63, 112);  ///< Vivid Rose/Pink for accentuating specific UI elements.

    // Typography
    const sf::Color TEXT_MAIN = sf::Color(255, 247, 237);  ///< Warm Cream White for main body and heading text.
    const sf::Color TEXT_DIM  = sf::Color(167, 139, 192);  ///< Soft Lavender Gray for secondary or disabled text.

    // State Indicators
    const sf::Color SUCCESS           = sf::Color(52, 211, 153);  ///< Seafoam Green for successful operations or status indicators.
    const sf::Color PATH_COLOR        = sf::Color(253, 224, 71);  ///< Golden Hour Yellow for rendering the final solved path.
    const sf::Color EXPLORATION_COLOR = sf::Color(56, 189, 248);  ///< Light Sky Blue for rendering explored areas/nodes.
    const sf::Color DOT_COLOR         = sf::Color(255, 255, 255); ///< Pure White for marker dots and interactive indicators.
    const sf::Color WALL_COLOR        = sf::Color(91, 76, 125);   ///< Dusty Purple for maze walls.

    // Visited cells use a subtle warm glow
    const sf::Color VISITED_CELL = sf::Color(251, 146, 60, 40); ///< Transparent Orange Glow for marking visited cells during generation.
}  // namespace Theme

