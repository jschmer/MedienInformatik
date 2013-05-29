/* 
 * Copyright (c) 2013 Jens Schmer
 * This file is distributed under the MIT License.
 * See COPYING within this package for further information. 
 */
#pragma once

#include "IRender.h"
#include "CameraInterface.hpp"

class ObjViewer :
    public CameraInterface,
    public IRender
{
public:
    ObjViewer();
    ~ObjViewer();

    void render();

    // keyboard events
    sf::String HelpInfo() const;
    void OnKeyPressed(sf::Keyboard::Key key, bool ctrl, bool alt, bool shift, bool system);

    // other events
    void OnResized(uint width, uint height);
};

