#include "ObjViewer.h"

#include <exception>
#include <string>
#include <algorithm>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <SFML/OpenGL.hpp>

using namespace std;
using glm::vec3;
using glm::vec4;

ObjViewer::ObjViewer()
    : _normalmode(NormalMode::Triangle)
{
    try {
        loadObj("../bunny.obj", _obj);
        //loadObj("../Anno_complete1.obj", _obj);
        //loadObj("../Dragon Fusion.obj", _obj);
    } catch (std::exception &e) {
        _help_info_append = std::string("\n") + std::string(e.what());
    }

    // bunny
    _eye    = vec4(-.1f, 2.f, 6.f, 1.f);
    _center = vec4(0.f, 2.f, 0.f, 1.f);
    _translation_factor = 1.f;

    // Anno
    //_eye    = vec4(0.f, 250.f, 650.f, 1.f);
    //_center = vec4(0.f, 250.f, 0.f, 1.f);
    //_translation_factor = 25.f;

    _up     = vec4(0.f, 1.f, 0.f, 0.f);
}

ObjViewer::~ObjViewer()
{
}

void ObjViewer::enableLighting() {
    float factor = 6.f;

    GLfloat ambient[]  = { 0.3f*factor, 0.1f*factor, 0.1f*factor, 1.0 };
    GLfloat diffuse[]  = { 0.8f*factor, 0.4f*factor, 0.4f*factor, 1.0 };
    GLfloat specular[] = { 0.6f*factor, 0.8f*factor, 0.6f*factor, 1.0 };

    GLfloat position[] = { 0.0, 1.0, 3.0, 1.0 };

    glShadeModel( GL_SMOOTH );

    glLightfv( GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuse );
    glLightfv( GL_LIGHT0, GL_SPECULAR, specular);
    glLightfv( GL_LIGHT0, GL_POSITION, position);

    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );
}

void ObjViewer::render()
{
    // render loaded _obj
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // view matrix
    auto lookAt = glm::lookAt(vec3(_eye), vec3(_center), vec3(_up));
    glMultMatrixf(&lookAt[0][0]);
    glMultMatrixf(&_rotation_mat_user_input[0][0]);

    enableLighting();

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    // enable z-Buffer and Backface Culling
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    auto scale = 20.f;
    glScalef(scale, scale, scale); // bunny

    auto normal_array = &_obj.gl_normals;
    if (_normalmode == NormalMode::Averaged)
        normal_array = &_obj.gl_normals_average; // use averaged normals

    glVertexPointer(3, GL_FLOAT, 0, &_obj.gl_vertices[0]);
    glNormalPointer(GL_FLOAT, 0, &(*normal_array)[0]);
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
Arrows - Up/Left/Down/Right
Shift + Up/Down - Forward/Backward

Rotate Object:
WASD

Normal Rendering:
F1 - Triangle
F2 - Averaged
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
    // normal mode switch
    case Key::F1:
        _normalmode = NormalMode::Triangle;
        break;
    case Key::F2:
        _normalmode = NormalMode::Averaged;
        break;

    // Arrows for camera translation
    case Key::Up:
        if (shift)
            translateCamera(Direction::Forward, _translation_factor);
        else
            translateCamera(Direction::Up, _translation_factor);
        break;
    case Key::Left:
        translateCamera(Direction::Left, _translation_factor);
        break;
    case Key::Down:
        if (shift)
            translateCamera(Direction::Backward, _translation_factor);
        else
            translateCamera(Direction::Down, _translation_factor);
        break;
    case Key::Right:
        translateCamera(Direction::Right, _translation_factor);
        break;

    // WASD for object rotation
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