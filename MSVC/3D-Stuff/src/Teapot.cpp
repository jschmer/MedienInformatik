#include <Teapot.h>

#include <Freeglut/glut.h>
#include <glm/gtc/matrix_transform.hpp>

using glm::vec3;
using glm::vec4;

Teapot::Teapot(uint width, uint height)
    : _rotation_mat_user_input(1.f),
    _perspective(true),
    _width(width),
    _height(height),
    _near(1),
    _far(1000)
{
    // setting up default camera
    _eye    = vec4(0, 0, 5, 1);
    _center = vec4(0, 0, 0, 1);
    _up     = vec4(0, 1, 0, 0);
}

Teapot::~Teapot()
{
}

void Teapot::render() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMultMatrixf(&_rotation_mat_user_input[0][0]);

    if (_perspective) {
        // view matrix
        auto lookAt = glm::lookAt(vec3(_eye), vec3(_center), vec3(_up));
        glMultMatrixf(&lookAt[0][0]);
    }

    glColor3f(1.f, 1.f, 1.f);
    glutWireTeapot(.5);
}

void Teapot::OnResized(uint width, uint height) {
    // Prevent a divide by zero, when window is too short
    // (you cant make a window of zero height).
    height = std::max(height, 1u);

    _width = width;
    _height = height;

    // Set the viewport to be the entire window
    glViewport(0, 0, _width, _height);

    // Use the Projection Matrix
    glMatrixMode(GL_PROJECTION);
    // Reset Matrix
    glLoadIdentity();

    if (_perspective)
        SetProjectionPerspective();
    else
        SetProjectionParallel();

    // Switch back to the Modelview
    glMatrixMode(GL_MODELVIEW);
}

void Teapot::SetProjectionPerspective() {
    _near = 1;
    _far  = 1000;

    // Set the correct perspective.
    auto fovy = 60.0;
    auto ratio = 1.0 * _width / _height;
    gluPerspective(fovy, ratio, _near, _far);

    _eye    = vec4(0, 0, 2, 1);
    _center = vec4(0, 0, 0, 1);
    _up     = vec4(0, 1, 0, 0);
}

void Teapot::SetProjectionParallel() {
    _near = -5;
    _far  = 5; 
   
    // Set the correct perspective.
    auto ar = 1.0 * _width / _height;

    if(_width <= _height)
        glOrtho(-1.0, 1.0, -1.0/ar, 1.0/ar, _near, _far);
    else
        glOrtho(-1.0*ar, 1.0*ar, -1.0, 1.0, _near, _far);
}

sf::String Teapot::HelpInfo() const {
    const auto append_text = R"(
WASD - Obj Rotation
Arrows - Obj Translation
P - Toggle Projection

IJKL - Camera Translation
)";

    return append_text;
}

// keyboard events
void Teapot::OnKeyPressed(sf::Keyboard::Key key, bool ctrl, bool alt, bool shift, bool system) {
    typedef sf::Keyboard::Key Key;

    const auto rotate_angle_delta = 2.f;
    const auto translate_length_delta = .1f;

    glm::vec3 v;

    switch (key) {
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

    // Arrows for object translation
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

    // Arrows for camera translation
    case Key::I:
        translateCamera(Direction::Up);
        break;
    case Key::J:
        translateCamera(Direction::Left);
        break;
    case Key::K:
        translateCamera(Direction::Down);
        break;
    case Key::L:
        translateCamera(Direction::Right);
        break;

        // projection toggle
    case Key::P:
        _perspective = !_perspective;
        _rotation_mat_user_input = glm::mat4(1.f);
        OnResized(_width, _height); // trigger resized event to update projection
        break;
    default:
        break;
    }
   
}