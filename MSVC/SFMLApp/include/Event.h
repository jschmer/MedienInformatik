/* 
 * Copyright (c) 2013 Jens Schmer
 * This file is distributed under the MIT License.
 * Consult COPYING within this package for further information. 
 */
#pragma once

#include <SFML/Window/Event.hpp>

typedef unsigned int uint;
typedef sf::Uint32 UTF_Code;

//
// @brief   Base class for providing event handlers for SFML events
//          SFML Events explained: http://www.sfml-dev.org/tutorials/2.0/window-events.php
//
class Event {
protected:
    Event();
    virtual ~Event();

    // @brief   Event processor function, calls the corresponding functions for the events
    //          e.g. OnClosed() for the sf::Event::Closed event
    virtual void OnEvent(sf::Event& event);

    // Event handler
protected:
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