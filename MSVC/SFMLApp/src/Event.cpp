/* 
 * Copyright (c) 2013 Jens Schmer
 * This file is distributed under the MIT License.
 * Consult COPYING within this package for further information. 
 */
#include <Event.h>

Event::Event() {
}

Event::~Event() {
}

void Event::OnEvent(sf::Event& event) {
    switch (event.type) {
    case sf::Event::Closed:
        OnClosed();
        break;
    case sf::Event::Resized:
        OnResized(event.size.width, event.size.height);
        break;
    case sf::Event::LostFocus:
        OnLostFocus();
        break;
    case sf::Event::GainedFocus:
        OnLostFocus();
        break;
    case sf::Event::TextEntered:
        OnTextEntered(event.text.unicode);
        break;
    case sf::Event::KeyPressed:
        OnKeyPressed(event.key.code, event.key.control, event.key.alt, event.key.shift, event.key.system);
        break;
    case sf::Event::KeyReleased:
        OnKeyReleased(event.key.code, event.key.control, event.key.alt, event.key.shift, event.key.system);
        break;
    case sf::Event::MouseButtonPressed:
        OnMouseButtonPressed(event.mouseButton.button, event.mouseButton.x, event.mouseButton.y);
        break;
    case sf::Event::MouseButtonReleased:
        OnMouseButtonReleased(event.mouseButton.button, event.mouseButton.x, event.mouseButton.y);
        break;
    case sf::Event::MouseWheelMoved:
        OnMouseWheelMoved(event.mouseWheel.delta);
        break;
    case sf::Event::MouseMoved:
        OnMouseMoved(event.mouseMove.x, event.mouseMove.y);
        break;
    case sf::Event::MouseEntered:
        OnMouseEntered();
        break;
    case sf::Event::MouseLeft:
        OnMouseLeft();
        break;
    default:
        assert(false);
        break;
    }
}

// Event handlers
void Event::OnClosed() {
}

void Event::OnResized(uint width, uint height) {
}

void Event::OnLostFocus() {
}

void Event::OnGainedFocus() {
}

void Event::OnTextEntered(UTF_Code code) {
}

void Event::OnKeyPressed(sf::Keyboard::Key key, bool ctrl, bool alt, bool shift, bool system) {
    typedef sf::Keyboard::Key Key;

    // example
    switch (key) {
    case Key::Escape:
        OnClosed();
        break;
    default:
        break;
    }
}

void Event::OnKeyReleased(sf::Keyboard::Key key, bool ctrl, bool alt, bool shift, bool system) {
}

void Event::OnMouseButtonPressed(sf::Mouse::Button button, int x, int y) {
    typedef sf::Mouse::Button Button;

    // example
    switch (button) {
    case Button::Left:
        break;
    default:
        break;
    }
}

void Event::OnMouseButtonReleased(sf::Mouse::Button, int x, int y) {
}

void Event::OnMouseWheelMoved(int delta) {
}

void Event::OnMouseMoved(int x, int y) {
}

void Event::OnMouseEntered() {
}

void Event::OnMouseLeft() {
}
