/* 
 * Copyright (c) 2013 Jens Schmer
 * This file is distributed under the MIT License.
 * See COPYING within this package for further information. 
 */
#pragma once

#include <cassert>

#include <SFMLApp.h>

#include <Teapot.h>
#include <Robot.h>

class OpenGLApp : public SFMLApp {
    typedef SFMLApp Super;

    enum class Mode {
        Teapot,
        Robot
    };

public:
    OpenGLApp(int* argc, char** argv);

    // init
    bool OnInit() override;

    // render loop hook
    void OnRender() override;

    // event hooks
    virtual void OnKeyPressed(sf::Keyboard::Key key, bool ctrl, bool alt, bool shift, bool system) override;
    virtual void OnResized(uint width, uint height) override;

private:
    Mode _current_mode;
    Teapot _teapot;
    Robot _robot;
};