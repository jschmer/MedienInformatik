#include <Teapot.h>

#include <Freeglut/glut.h>
#include <glm/gtc/matrix_transform.hpp>

Teapot::Teapot()
    : _rotation_mat_user_input(1.f),
    _perspective(true),
    _width(600),
    _height(400),
    _near(1),
    _far(1000)
{}

void Teapot::render() {
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(&_rotation_mat_user_input[0][0]);

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
    // Set the correct perspective.
    auto fovy = 100.0;
    auto ratio = 1.0 * _width / _height;
    gluPerspective(fovy, ratio, _near, _far);

    const double eye[]    = {0.0, 0.0, 2.0};
    const double center[] = {0.0, 0.0, 0.0};
    const double up[]     = {0.0, 1.0, 0.0};

    // setting camera
    gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], up[0], up[1], up[2]);
}

void Teapot::SetProjectionParallel() {
    // Set the correct perspective.
    auto ar = 1.0 * _width / _height;

    if(_width <= _height)
        glOrtho(-1.0, 1.0, -1.0/ar, 1.0/ar, 1.0, -1.0);
    else
        glOrtho(-1.0*ar, 1.0*ar, -1.0, 1.0, 1.0, -1.0);

    const double eye[]    = {0.0, 0.0, 1.0};
    const double center[] = {0.0, 0.0, 0.0};
    const double up[]     = {0.0, 1.0, 0.0};

    // setting camera
    gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], up[0], up[1], up[2]);
}

sf::String Teapot::HelpInfo() const {
    const auto append_text = R"(
WASD - Obj Rotation
Arrows - Obj Translation
P - Toggle Projection
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