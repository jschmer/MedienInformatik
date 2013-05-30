/* 
 * Copyright (c) 2013 Jens Schmer
 * This file is distributed under the MIT License.
 * See COPYING within this package for further information. 
 */
#pragma once
#include <glm/glm.hpp>

#include <SFMLApp.h>

#include <IRender.h>
#include <CameraInterface.hpp>

class Teapot : public CameraInterface, public IRender {
public:
    Teapot(uint width = 600, uint height = 400);
    ~Teapot();
    void render();

    void enableLighting();

    // keyboard events
    sf::String HelpInfo() const;
    void OnKeyPressed(sf::Keyboard::Key key, bool ctrl, bool alt, bool shift, bool system);

    // other events
    void OnResized(uint width, uint height);

private:
    void SetProjectionPerspective();
    void SetProjectionParallel();

private:
    glm::mat4 _rotation_mat_user_input;
    uint _width, _height;
    bool _perspective;
    double _near, _far;
};