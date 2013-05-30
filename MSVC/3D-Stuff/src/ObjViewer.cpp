#include "ObjViewer.h"

#include <exception>
#include <string>

#include <glm/glm.hpp>
#include <SFML/OpenGL.hpp>

using glm::vec3;
using glm::vec4;

ObjViewer::ObjViewer()
{
    try {
        loadObj("bunny.obj", _obj);
        //loadObj("Anno_complete1.obj", _obj);
        //loadObj("Dragon Fusion.obj", _obj);
    } catch (std::exception &e) {
        _help_info_append = std::string("\n") + std::string(e.what());
    }

    // bunny
    _eye    = vec4(0.f, 1.f, 3.f, 1.f);
    _center = vec4(0.f, 1.f, 0.f, 1.f);

    // Anno
    //_eye    = vec4(0.f, 250.f, 650.f, 1.f);
    //_center = vec4(0.f, 250.f, 0.f, 1.f);

    _up     = vec4(0.f, 1.f, 0.f, 0.f);
}

ObjViewer::~ObjViewer()
{
}

void ObjViewer::render()
{
    // render loaded _obj
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // view matrix
    auto lookAt = glm::lookAt(vec3(_eye), vec3(_center), vec3(_up));
    glMultMatrixf(&lookAt[0][0]);

    glEnableClientState(GL_VERTEX_ARRAY);

    // enable z-Buffer and Backface Culling
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glScalef(10.f, 10.f, 10.f); // bunny

    glColor3f(1.0f, 0.0f, 0.0f);
    glVertexPointer(3, GL_FLOAT, 0, &_obj.gl_vertices[0]);
    glDrawArrays(GL_TRIANGLES, 0, _obj.faces.size()*3);

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    glDisableClientState(GL_VERTEX_ARRAY);
}

// keyboard events
sf::String ObjViewer::HelpInfo() const
{
    const auto append_text = R"(
Translate Camera:
WASD - Up/Left/Down/Right
Shift + W/D - Forward/Backward
)";

    return std::string(append_text) + _help_info_append;
}

void ObjViewer::OnKeyPressed(sf::Keyboard::Key key, bool ctrl, bool alt, bool shift, bool system)
{
    typedef sf::Keyboard::Key Key;

    const auto rotate_angle_delta = 2.f;
    const auto translate_length_delta = .1f;

    glm::vec3 v;
    switch (key) {
        // Arrows for camera translation
    case Key::Up:
        if (shift)
            translateCamera(Direction::Forward);
        else
            translateCamera(Direction::Up);
        break;
    case Key::Left:
        translateCamera(Direction::Left);
        break;
    case Key::Down:
        if (shift)
            translateCamera(Direction::Backward);
        else
            translateCamera(Direction::Down);
        break;
    case Key::Right:
        translateCamera(Direction::Right);
        break;
    default:
        break;
    }
}

// other events
void ObjViewer::OnResized(uint width, uint height)
{
    // Prevent a divide by zero, when window is too short
    // (you cant make a window of zero height).
    height = std::max(height, 1u);

    // Set the viewport to be the entire window
    glViewport(0, 0, width, height);

    // Use the Projection Matrix
    glMatrixMode(GL_PROJECTION);
    // Reset Matrix
    glLoadIdentity();

    auto fovy  = 45;
    auto ratio = 1.0 * width / height;
    gluPerspective(fovy, ratio, 1, 1000);

    // Switch back to the Modelview
    glMatrixMode(GL_MODELVIEW);
}