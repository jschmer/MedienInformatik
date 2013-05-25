#include "Robot.h"

#include <glm/gtc/matrix_transform.hpp>

using glm::mat4;
using glm::vec3;
using glm::vec4;

// quad vertices helper
std::vector<GLfloat> produceQuad(float width, float height, float depth) {
    auto& w = width;
    auto& h = height;
    auto& d = depth;
    
    const GLfloat vertices[] = {
        // left
        0, 0, 0,
        0, h, 0,
        0, 0, d,
        0, h, d, 

        // front
        w, 0, d,
        w, h, d, 

        // right
        w, 0, 0,
        w, h, 0, 

        // back
        0, 0, 0,
        0, h, 0, 

        // top, seperate quad!
        0, h, d,
        0, h, 0,
        w, h, 0,
        w, h, d,

        // bottom, seperate quad!
        w, 0, 0,
        0, 0, 0,
        0, 0, d,
        w, 0, d
    };
    return std::vector<GLfloat>(std::begin(vertices), std::end(vertices));
}

Robot::Robot()
{
    // setting up default camera
    _eye    = vec4(0, 2, 4, 1);
    _center = vec4(0, 1, 0, 1);
    _up     = vec4(0, 1, 0, 0);

    // base
    auto w = 2.f; // width
    auto h = 1.f; // height
    auto d = h;   // depth
    _base_vertices = produceQuad(w, h, d);

    // upper arm
    w = .25f; // width
    h = 1.f;  // height
    d = w;    // depth
    _upper_arm_vertices = produceQuad(w, h, d);

    // lower arm
    w = 1.f;  // width
    h = .25f; // height
    d = h;    // depth
    _lower_arm_vertices = produceQuad(w, h, d);

    // hand
    w = .1f; // width
    h = .5f; // height
    d = h;   // depth
    _hand_vertices = produceQuad(w, h, d);

    // load default transformation matrix
    _base_transform            = mat4(1.f);
    _left_upper_arm_transform  = mat4(1.f);
    _right_upper_arm_transform = mat4(1.f);
    _left_lower_arm_transform  = mat4(1.f);
    _right_lower_arm_transform = mat4(1.f);
    _left_hand_transform       = mat4(1.f);
    _right_hand_transform      = mat4(1.f);
}

Robot::~Robot()
{
}

sf::String Robot::HelpInfo() const {
    const auto append_text = R"(
Robot \\o/
QW - left hand rot
AS - left lower arm rot
YX - left upper arm rot

UI - right hand rot
JK - right lower arm rot
M, - right upper arm rot

IJKL - camera translation
)";

    return append_text;
}

void Robot::render() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // view matrix
    auto lookAt = glm::lookAt(vec3(_eye), vec3(_center), vec3(_up));
    glMultMatrixf(&lookAt[0][0]);

    glEnableClientState(GL_VERTEX_ARRAY);

    // enable z-Buffer and Backface Culling
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    DrawBase();
    DrawLeftArm();
    DrawRightArm();

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    glDisableClientState(GL_VERTEX_ARRAY);
}

void Robot::DrawBase() const {
    // transformation matrix for base
    glMultMatrixf(&_base_transform[0][0]);
    glTranslatef(-1.f, 0, 0);

    glColor3f(0.0f,0.0f,1.0f);
    glVertexPointer(3, GL_FLOAT, 0, &_base_vertices[0]);
    glDrawArrays(GL_QUAD_STRIP, 0, 10); // base without top & bottom
    glDrawArrays(GL_QUADS, 10, 8); // base top & bottom
}

void Robot::DrawLeftArm() const {
    glPushMatrix();

    const auto tf_2 = .25f/2;
    
    // left upper arm
    glTranslatef(.25f, 1.f, .5f);
    // custom user transforms
    glTranslatef(tf_2, 0.f, tf_2);
    glMultMatrixf(&_left_upper_arm_transform[0][0]);
    glTranslatef(-tf_2, 0.f, -tf_2);

    glColor3f(0.0f,1.0f,0.0f);
    glVertexPointer(3, GL_FLOAT, 0, &_upper_arm_vertices[0]);
    glDrawArrays(GL_QUAD_STRIP, 0, 10); // base without top & bottom
    glDrawArrays(GL_QUADS, 10, 8); // base top & bottom

    // left lower arm
    glTranslatef(-1.f+.25f, 1.f, 0.f);
    // custom user transforms
    glTranslatef(1.f-.125f, 0.f, tf_2);
    glMultMatrixf(&_left_lower_arm_transform[0][0]);
    glTranslatef(-1.f+.125f, 0.f, -tf_2);

    glColor3f(1.0f,0.0f,0.0f);
    glVertexPointer(3, GL_FLOAT, 0, &_lower_arm_vertices[0]);
    glDrawArrays(GL_QUAD_STRIP, 0, 10); // base without top & bottom
    glDrawArrays(GL_QUADS, 10, 8); // base top & bottom

    // left hand
    glTranslatef(-.1f, -.125f, -.125f);
    // custom user transforms
    glTranslatef(0.f, .25f, .25f);
    glMultMatrixf(&_left_hand_transform[0][0]);
    glTranslatef(0.f, -.25f, -.25f);

    glColor3f(1.0f,1.0f,0.0f);
    glVertexPointer(3, GL_FLOAT, 0, &_hand_vertices[0]);
    glDrawArrays(GL_QUAD_STRIP, 0, 10); // base without top & bottom
    glDrawArrays(GL_QUADS, 10, 8); // base top & bottom

    glPopMatrix();
}

void Robot::DrawRightArm() const {
    glPushMatrix();
    
    const auto tf_2 = .25f/2;

    // left upper arm
    glTranslatef(1.5f, 1.f, .5f);
    // custom user transforms
    glTranslatef(tf_2, 0.f, tf_2);
    glMultMatrixf(&_right_upper_arm_transform[0][0]);
    glTranslatef(-tf_2, 0.f, -tf_2);

    glColor3f(0.0f,1.0f,0.0f);
    glVertexPointer(3, GL_FLOAT, 0, &_upper_arm_vertices[0]);
    glDrawArrays(GL_QUAD_STRIP, 0, 10); // base without top & bottom
    glDrawArrays(GL_QUADS, 10, 8); // base top & bottom

    // left lower arm
    glTranslatef(0.f, 1.f, 0.f);
    // custom user transforms
    glTranslatef(.125f, 0.f, tf_2);
    glMultMatrixf(&_right_lower_arm_transform[0][0]);
    glTranslatef(-.125f, 0.f, -tf_2);

    glColor3f(1.0f,0.0f,0.0f);
    glVertexPointer(3, GL_FLOAT, 0, &_lower_arm_vertices[0]);
    glDrawArrays(GL_QUAD_STRIP, 0, 10); // base without top & bottom
    glDrawArrays(GL_QUADS, 10, 8); // base top & bottom

    // left hand
    glTranslatef(1.f, -.125f, -.125f);
    // custom user transforms
    glTranslatef(0.f, .25f, .25f);
    glMultMatrixf(&_right_hand_transform[0][0]);
    glTranslatef(0.f, -.25f, -.25f);

    glColor3f(1.0f,1.0f,0.0f);
    glVertexPointer(3, GL_FLOAT, 0, &_hand_vertices[0]);
    glDrawArrays(GL_QUAD_STRIP, 0, 10); // base without top & bottom
    glDrawArrays(GL_QUADS, 10, 8); // base top & bottom

    glPopMatrix();
}

void Robot::OnResized(uint width, uint height) {
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

// keyboard events
void Robot::OnKeyPressed(sf::Keyboard::Key key, bool ctrl, bool alt, bool shift, bool system) {
    typedef sf::Keyboard::Key Key;

    const auto rotate_angle_delta = 2.f;
    const auto translate_length_delta = .1f;

    glm::vec3 v;

    switch (key) {
    // QW for left hand rotation
    case Key::Q:
        // rotate object upwards
         v = glm::vec3(1, 0, 0);
        _left_hand_transform = glm::rotate(_left_hand_transform, rotate_angle_delta, v);
        break;
    case Key::W:
        // rotate object left
        v = glm::vec3(1, 0, 0);
        _left_hand_transform = glm::rotate(_left_hand_transform, -rotate_angle_delta, v);
        break;

    // AS for left lower arm rotation
    case Key::A:
        // rotate object downwards
        v = glm::vec3(0, 1, 0);
        _left_lower_arm_transform = glm::rotate(_left_lower_arm_transform, -rotate_angle_delta, v);
        break;
    case Key::S:
        // rotate object right
        v = glm::vec3(0, 1, 0);
        _left_lower_arm_transform = glm::rotate(_left_lower_arm_transform, rotate_angle_delta, v);
        break;

    // YX for left upper arm rotation
    case Key::Y:
        // translate object upwards
         v = glm::vec3(0, 1, 0);
        _left_upper_arm_transform = glm::rotate(_left_upper_arm_transform, rotate_angle_delta, v);
        break;
    case Key::X:
        v = glm::vec3(0, 1, 0);
        _left_upper_arm_transform = glm::rotate(_left_upper_arm_transform, -rotate_angle_delta, v);
        break;

    // UI for right hand rotation
    case Key::U:
        // rotate object upwards
         v = glm::vec3(1, 0, 0);
        _right_hand_transform = glm::rotate(_right_hand_transform, rotate_angle_delta, v);
        break;
    case Key::I:
        // rotate object right
        v = glm::vec3(1, 0, 0);
        _right_hand_transform = glm::rotate(_right_hand_transform, -rotate_angle_delta, v);
        break;

    // JK for right lower arm rotation
    case Key::J:
        // rotate object downwards
        v = glm::vec3(0, 1, 0);
        _right_lower_arm_transform = glm::rotate(_right_lower_arm_transform, -rotate_angle_delta, v);
        break;
    case Key::K:
        // rotate object right
        v = glm::vec3(0, 1, 0);
        _right_lower_arm_transform = glm::rotate(_right_lower_arm_transform, rotate_angle_delta, v);
        break;

    // M; for right upper arm rotation
    case Key::M:
        // translate object upwards
         v = glm::vec3(0, 1, 0);
        _right_upper_arm_transform = glm::rotate(_right_upper_arm_transform, rotate_angle_delta, v);
        break;
    case Key::Comma:
        v = glm::vec3(0, 1, 0);
        _right_upper_arm_transform = glm::rotate(_right_upper_arm_transform, -rotate_angle_delta, v);
        break;

    // Arrows for camera translation
    case Key::Up:
        translateCamera(Direction::Up);
        break;
    case Key::Left:
        translateCamera(Direction::Left);
        break;
    case Key::Down:
        translateCamera(Direction::Down);
        break;
    case Key::Right:
        translateCamera(Direction::Right);
        break;

    default:
        break;
    }
   
}