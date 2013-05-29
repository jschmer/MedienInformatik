#pragma once

#include <SFMLApp.h>

class IRender {
public:
    virtual void render() = 0;

    // keyboard events
    virtual sf::String HelpInfo() const = 0;
    virtual void OnKeyPressed(sf::Keyboard::Key key, bool ctrl, bool alt, bool shift, bool system) = 0;

    // other events
    virtual void OnResized(uint width, uint height) = 0;
};