/* 
 * Copyright (c) 2013 Jens Schmer
 * This file is distributed under the MIT License.
 * See COPYING within this package for further information. 
 */
#pragma once

#include "objLoader.hpp"
#include "IRender.h"
#include "CameraInterface.hpp"

class ObjViewer :
    public CameraInterface,
    public IRender
{
public:
    ObjViewer();
    ~ObjViewer();

    enum class NormalMode {
        Triangle,
        Averaged
    };

    void render();
    void enableLighting();

    // keyboard events
    sf::String HelpInfo() const;
    void OnKeyPressed(sf::Keyboard::Key key, bool ctrl, bool alt, bool shift, bool system);

    // other events
    void OnResized(uint width, uint height);

private:
    Obj _obj;
    NormalMode _normalmode;
    float _translation_factor;

    std::string _help_info_append;
    glm::mat4 _rotation_mat_user_input;
};

