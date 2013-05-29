/* 
 * Copyright (c) 2013 Jens Schmer
 * This file is distributed under the MIT License.
 * See COPYING within this package for further information. 
 */
#pragma once

#include <cassert>

#include <glm/glm.hpp>

#include <SFMLApp.h>

#include "IRender.h"
#include "Teapot.h"
#include "Robot.h"
#include "ObjViewer.h"

class OpenGLApp : public SFMLApp {
    typedef SFMLApp Super;

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
    // just a pointer to the current mode, ownership is still at the reference object itself
    IRender* _mode;

    Teapot _teapot;
    Robot _robot;
    ObjViewer _objviewer;
};