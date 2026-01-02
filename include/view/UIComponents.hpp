#pragma once

#include <SFML/Graphics.hpp>
#include <string>

/**
 * @namespace UIComponents
 * @brief Reusable UI drawing functions.
 */
namespace UIComponents {

    /**
     * @brief Draws a reusable button element with hover and active states.
     * @param window The render window to draw to.
     * @param font The font to use for the button label.
     * @param label The text to display on the button.
     * @param bounds [out] The bounding box of the calculated button area.
     * @param x The X-coordinate of the button's top-left corner.
     * @param y The Y-coordinate of the button's top-left corner.
     * @param width The target width of the button.
     * @param active Whether the button is currently selected/active.
     * @param mouse_pos Current mouse position for hover detection.
     */
    void draw_btn(sf::RenderWindow &window, const sf::Font &font,
                 const std::string &label, sf::FloatRect &bounds,
                 float x, float y, float width,
                 bool active, const sf::Vector2f &mouse_pos);

    /**
     * @brief Draws a stylized text input box with focus indicators.
     * @param window The render window to draw to.
     * @param font The font to use for labels and input text.
     * @param label The descriptive label above or beside the input.
     * @param value The current string value to display inside the box.
     * @param bounds [out] The bounding box of the interactive input area.
     * @param x The X-coordinate of the input box's top-left corner.
     * @param y The Y-coordinate of the input box's top-left corner.
     * @param width The target width of the input box.
     * @param focused Whether the input box currently has keyboard focus.
     */
    void draw_input_box(sf::RenderWindow &window, const sf::Font &font,
                       const std::string &label, const std::string &value,
                       sf::FloatRect &bounds, float x, float y,
                       float width, bool focused);

} // namespace UIComponents

