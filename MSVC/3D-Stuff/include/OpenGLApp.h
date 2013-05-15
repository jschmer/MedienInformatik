/* 
 * Copyright (c) 2013 Jens Schmer
 * This file is distributed under the MIT License.
 * Consult COPYING within this package for further information. 
 */
#pragma once

#include <SFMLApp.h>

#include <glm/glm.hpp>

class OpenGLApp : public SFMLApp {
    typedef SFMLApp Super;

public:
    OpenGLApp(int* argc, char** argv);

    // render loop hook
    void OnRender();

    // event hooks
    virtual void OnKeyPressed(sf::Keyboard::Key key, bool ctrl, bool alt, bool shift, bool system) override;

    // render functions
    void UtahTeapot();

private:
    glm::mat4 _rotation_mat_user_input;
};