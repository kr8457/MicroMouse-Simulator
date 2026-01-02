/**
 * @file UIComponents.cpp
 * @brief Implementation of reusable UI components.
 */

#include "view/UIComponents.hpp"

#include "view/UITheme.hpp"
#include <algorithm>

namespace UIComponents {

void draw_btn(sf::RenderWindow &window, const sf::Font &font,
             const std::string &label, sf::FloatRect &bounds,
             float x, float y, float width,
             bool active, const sf::Vector2f &mouse_pos) {
    // Responsive button height based on window size
    const float WINDOW_HEIGHT = static_cast<float>(window.getSize().y);
    const float BTN_HEIGHT = std::clamp(WINDOW_HEIGHT * 0.05f, 30.0f, 42.0f);
    const float OUTLINE_THICK = 2.0F;
    const unsigned int FONT_SIZE = static_cast<unsigned int>(std::clamp(WINDOW_HEIGHT * 0.02f, 12.0f, 16.0f));

    sf::RectangleShape shape(sf::Vector2f(width, BTN_HEIGHT));
    shape.setPosition(x, y);
    bounds = shape.getGlobalBounds();

    const bool HOVERED = bounds.contains(mouse_pos);

    if (active) {
        shape.setFillColor(Theme::PRIMARY);
        shape.setOutlineThickness(OUTLINE_THICK);
        shape.setOutlineColor(sf::Color::White);
    } else if (HOVERED) {
        shape.setFillColor(Theme::PRIMARY_HOVER);
    } else {
        shape.setFillColor(Theme::SURFACE);
    }

    window.draw(shape);

    sf::Text text(label, font, FONT_SIZE);
    text.setFillColor(Theme::TEXT_MAIN);
    sf::FloatRect tb = text.getLocalBounds();
    
    // Auto-scale text if too wide for the button
    const float MAX_W = width - 12.0f; // 6px padding on each side
    if (tb.width > MAX_W) {
        float s = MAX_W / tb.width;
        text.setScale(s, s);
    }

    // Origin-based centering is easier for scaled text
    text.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
    text.setPosition(x + width / 2.0f, y + BTN_HEIGHT / 2.0f);
    
    window.draw(text);
}

void draw_input_box(sf::RenderWindow &window, const sf::Font &font,
                   const std::string &label, const std::string &value,
                   sf::FloatRect &bounds, float x, float y,
                   float width, bool focused) {
    const float LABEL_WIDTH = 55.0f;
    const float PADDING = 8.0f;
    const float HEIGHT = std::clamp(static_cast<float>(window.getSize().y) * 0.045f, 28.0f, 36.0f);

    sf::Text t(label + ":", font, 14);
    t.setFillColor(Theme::TEXT_DIM);
    t.setPosition(x, y + (HEIGHT - 20.0f) / 2.0f);
    window.draw(t);

    sf::RectangleShape rect(sf::Vector2f(width - LABEL_WIDTH, HEIGHT));
    rect.setPosition(x + LABEL_WIDTH, y);
    rect.setFillColor(sf::Color(15, 23, 42));
    rect.setOutlineThickness(focused ? 2.0f : 1.0f);
    rect.setOutlineColor(focused ? Theme::PRIMARY : Theme::SURFACE);
    window.draw(rect);
    bounds = rect.getGlobalBounds();

    sf::Text val(value + (focused ? "_" : ""), font, 14);
    val.setFillColor(Theme::TEXT_MAIN);
    val.setPosition(x + LABEL_WIDTH + PADDING, y + (HEIGHT - 20.0f) / 2.0f);
    window.draw(val);
}

} // namespace UIComponents
