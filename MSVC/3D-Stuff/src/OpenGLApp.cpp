/* 
 * Copyright (c) 2013 Jens Schmer
 * This file is distributed under the MIT License.
 * Consult COPYING within this package for further information. 
 */
#include <OpenGLApp.h>

#include <Freeglut/glut.h>

OpenGLApp::OpenGLApp(int* argc, char** argv)
    : _teapot(),
    _robot(),
    _objviewer(),
    _mode(&_teapot)
{
    glutInit(argc, argv);
}

bool OpenGLApp::OnInit() {
    if (!Super::OnInit())
        return false;

    const auto append_text = "\n"
        "1 = Teapot\n"
        "2 = Robot\n"
        "3 = ObjViewer\n";

    auto help = _help_text.getString();
    help.insert(help.getSize(), append_text);
    _help_text.setString(help);

    glClearColor(0, 0, 0, 1);

    return true;
}

void OpenGLApp::OnRender() {
    // render mode
    _mode->render();

    // render "HUD"
    sf::String append;
    append = _mode->HelpInfo();

    Super::RenderHelpText(append);
    Super::RenderFPS();
    Super::RenderMousePos();
}

void OpenGLApp::OnKeyPressed(sf::Keyboard::Key key, bool ctrl, bool alt, bool shift, bool system) {
    typedef sf::Keyboard::Key Key;

    // switch mode
    switch (key) {
    case Key::Num1:
        _mode = &_teapot;
        break;
    case Key::Num2:
        _mode = &_robot;
        break;
    case Key::Num3:
        _mode = &_objviewer;
        break;
    }

    // handle mode
    _mode->OnKeyPressed(key, ctrl, alt, shift, system);
}

void OpenGLApp::OnResized(uint width, uint height) {
    Super::OnResized(width, height);

    _mode->OnResized(width, height);
}