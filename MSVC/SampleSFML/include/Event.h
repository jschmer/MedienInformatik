#pragma once

#include <SFML/Window/Event.hpp>

typedef unsigned int uint;
typedef sf::Uint32 UTF_Code;

class Event {
public:
    Event();
    virtual ~Event();

    // Event processor
    virtual void OnEvent(sf::Event& event);

    // Event handler
    virtual void OnClosed();
    virtual void OnResized(uint width, uint height);
    virtual void OnLostFocus();
    virtual void OnGainedFocus();
    virtual void OnTextEntered(UTF_Code code);
    virtual void OnKeyPressed(sf::Keyboard::Key key, bool ctrl, bool alt, bool shift, bool system);
    virtual void OnKeyReleased(sf::Keyboard::Key key, bool ctrl, bool alt, bool shift, bool system);
    virtual void OnMouseButtonPressed(sf::Mouse::Button button, int x, int y);
    virtual void OnMouseButtonReleased(sf::Mouse::Button button, int x, int y);
    virtual void OnMouseWheelMoved(int delta);
    virtual void OnMouseMoved(int x, int y);
    virtual void OnMouseEntered();
    virtual void OnMouseLeft();
};