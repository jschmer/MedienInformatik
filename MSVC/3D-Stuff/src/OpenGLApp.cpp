/* 
 * Copyright (c) 2013 Jens Schmer
 * This file is distributed under the MIT License.
 * Consult COPYING within this package for further information. 
 */
#include <OpenGLApp.h>

#include <Freeglut/glut.h>

OpenGLApp::OpenGLApp(int* argc, char** argv)
    : _current_mode(Mode::Teapot)
{
    glutInit(argc, argv);
}

bool OpenGLApp::OnInit() {
    if (!Super::OnInit())
        return false;

    const auto append_text = "\n"
        "1 = Teapot\n"
        "2 = Robot\n";

    auto help = _help_text.getString();
    help.insert(help.getSize(), append_text);
    _help_text.setString(help);

    glClearColor(0, 0, 0, 1);

    return true;
}

void OpenGLApp::OnRender() {
    // render mode
    switch (_current_mode) {
    case Mode::Teapot:
        _teapot.render();
        break;
    case Mode::Robot:
        _robot.render();
        break;
    default:
        assert(false);
    }

    // render "HUD"
    sf::String append;
    switch (_current_mode) {
    case Mode::Teapot:
        append = _teapot.HelpInfo();
        break;
    case Mode::Robot:
        append = _robot.HelpInfo();
        break;
    default:
        assert(false);
    }
    Super::RenderHelpText(append);
    Super::RenderFPS();
    Super::RenderMousePos();
}

void OpenGLApp::OnKeyPressed(sf::Keyboard::Key key, bool ctrl, bool alt, bool shift, bool system) {
    typedef sf::Keyboard::Key Key;

    // switch mode
    switch (key) {
    case Key::Num1:
        _current_mode = Mode::Teapot;
        break;
    case Key::Num2:
        _current_mode = Mode::Robot;
        break;
    }

    // handle mode
    switch (_current_mode) {
    case Mode::Teapot:
        _teapot.OnKeyPressed(key, ctrl, alt, shift, system);
        break;
    case Mode::Robot:
        //_robot.OnKeyPressed(key, ctrl, alt, shift, system);
        break;
    default:
        assert(false);
    }
}

void OpenGLApp::OnResized(uint width, uint height) {
    Super::OnResized(width, height);

    switch (_current_mode) {
    case Mode::Teapot:
        _teapot.OnResized(width, height);
        break;
    case Mode::Robot:
        _robot.OnResized(width, height);
        break;
    default:
        assert(false);
    }
}