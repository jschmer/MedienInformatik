/* 
 * Copyright (c) 2013 Jens Schmer
 * This file is distributed under the MIT License.
 * Consult COPYING within this package for further information. 
 */
#pragma once

#include <SFMLApp.h>

class OpenGLApp : public SFMLApp {
    typedef SFMLApp Super;

public:
    OpenGLApp(int* argc, char** argv);

    // render loop hook
    void OnRender();

    void UtahTeapot();
};