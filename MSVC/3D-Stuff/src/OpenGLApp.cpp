/* 
 * Copyright (c) 2013 Jens Schmer
 * This file is distributed under the MIT License.
 * Consult COPYING within this package for further information. 
 */
#include <OpenGLApp.h>

#include <Freeglut/glut.h>

OpenGLApp::OpenGLApp(int* argc, char** argv) {
    glutInit(argc, argv);
}

void OpenGLApp::OnRender() {
    _teapot.render();

    // render "HUD"
    Super::RenderHelpText(_teapot.HelpInfo());
    Super::RenderFPS();
    Super::RenderMousePos();
}

void OpenGLApp::OnKeyPressed(sf::Keyboard::Key key, bool ctrl, bool alt, bool shift, bool system) {
    _teapot.OnKeyPressed(key, ctrl, alt, shift, system);
}

void OpenGLApp::OnResized(uint width, uint height) {
    _teapot.OnResized(width, height);
}