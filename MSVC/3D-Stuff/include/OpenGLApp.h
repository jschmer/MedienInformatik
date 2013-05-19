/* 
 * Copyright (c) 2013 Jens Schmer
 * This file is distributed under the MIT License.
 * Consult COPYING within this package for further information. 
 */
#pragma once

#include <cassert>

#include <SFMLApp.h>

#include <Teapot.h>

class OpenGLApp : public SFMLApp {
    typedef SFMLApp Super;

public:
    OpenGLApp(int* argc, char** argv);

    // render loop hook
    void OnRender();

    // event hooks
    virtual void OnKeyPressed(sf::Keyboard::Key key, bool ctrl, bool alt, bool shift, bool system) override;
    virtual void OnResized(uint width, uint height) override;

private:
    Teapot _teapot;
};