/* 
 * Copyright (c) 2013 Jens Schmer
 * This file is distributed under the MIT License.
 * Consult COPYING within this package for further information. 
 */
#include <OpenGLApp.h>

#include <cassert>

#include <Freeglut/glut.h>

OpenGLApp::OpenGLApp(int* argc, char** argv) {
    glutInit(argc, argv);
}

void OpenGLApp::OnRender() {
    UtahTeapot();

    // render "HUD"
    Super::RenderHelpText();
    Super::RenderFPS();
    Super::RenderMousePos();
}

void OpenGLApp::UtahTeapot() {
    glutWireTeapot(.5);
}