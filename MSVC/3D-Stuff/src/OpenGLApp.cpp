/* 
 * Copyright (c) 2013 Jens Schmer
 * This file is distributed under the MIT License.
 * Consult COPYING within this package for further information. 
 */
#include <OpenGLApp.h>

#include <cassert>

#include <Freeglut/glut.h>
#include <glm/gtc/matrix_transform.hpp>

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
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(&_rotation_mat_user_input[0][0]);

    glutWireTeapot(.5);
}

void OpenGLApp::OnKeyPressed(sf::Keyboard::Key key, bool ctrl, bool alt, bool shift, bool system) {
    typedef sf::Keyboard::Key Key;

    const auto rotate_angle_delta = 2.f;
    const auto translate_length_delta = .1f;

    glm::vec3 v;

    switch (key) {
    // WASD for rotation
    case Key::W:
        // rotate object upwards
         v = glm::vec3(1, 0, 0);
        _rotation_mat_user_input = glm::rotate(_rotation_mat_user_input, rotate_angle_delta, v);
        break;
    case Key::A:
        // rotate object left
        v = glm::vec3(0, 1, 0);
        _rotation_mat_user_input = glm::rotate(_rotation_mat_user_input, -rotate_angle_delta, v);
        break;
    case Key::S:
        // rotate object downwards
        v = glm::vec3(1, 0, 0);
        _rotation_mat_user_input = glm::rotate(_rotation_mat_user_input, -rotate_angle_delta, v);
        break;
    case Key::D:
        // rotate object right
        v = glm::vec3(0, 1, 0);
        _rotation_mat_user_input = glm::rotate(_rotation_mat_user_input, rotate_angle_delta, v);
        break;

    // Arrows for translation
    case Key::Up:
        // translate object upwards
         v = translate_length_delta * glm::vec3(0, 1, 0);
        _rotation_mat_user_input = glm::translate(_rotation_mat_user_input, v);
        break;
    case Key::Left:
        // translate object left
        v = translate_length_delta * glm::vec3(-1, 0, 0);
        _rotation_mat_user_input = glm::translate(_rotation_mat_user_input, v);
        break;
    case Key::Down:
        // translate object downwards
        v = translate_length_delta * glm::vec3(0, -1, 0);
        _rotation_mat_user_input = glm::translate(_rotation_mat_user_input, v);
        break;
    case Key::Right:
        // translate object right
        v = translate_length_delta * glm::vec3(1, 0, 0);
        _rotation_mat_user_input = glm::translate(_rotation_mat_user_input, v);
        break;

    default:
        break;
    }
   
}