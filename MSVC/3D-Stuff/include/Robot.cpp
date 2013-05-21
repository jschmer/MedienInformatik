#include "Robot.h"

#include <glm/gtc/matrix_transform.hpp>

using glm::mat4;
using glm::vec3;

// quad vertices helper
std::vector<GLfloat> produceQuad(float width, float height, float depth) {
    auto& w = width;
    auto& h = height;
    auto& d = depth;
    
    const GLfloat vertices[] = {
        0, 0, 0,
        0, h, 0,
        0, 0, d,
        0, h, d, // left

        w, 0, d,
        w, h, d, // front

        w, 0, 0,
        w, h, 0, // right

        0, 0, 0,
        0, h, 0, // back

        0, h, d,
        0, h, 0,
        w, h, 0,
        w, h, d, // top, seperate quad!

        0, 0, d,
        0, 0, 0,
        w, 0, 0, // bottom, seperate quad!
        w, 0, d
    };
    return std::vector<GLfloat>(std::begin(vertices), std::end(vertices));
}

Robot::Robot()
{
    // setting up default camera
    _camera.eye    = vec3(0, 2, 5);
    _camera.center = vec3(0, 1, 0);
    _camera.up     = vec3(0, 1, 0);

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
    const auto append_text = "\n"
        "Robot \\o/"
        ""
        ""
        "";

    return append_text;
}

void Robot::render() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(  _camera.eye[0], _camera.eye[1], _camera.eye[2], 
                _camera.center[0], _camera.center[1], _camera.center[2], 
                _camera.up[0], _camera.up[1], _camera.up[2]);

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
    
    // left upper arm
    glTranslatef(.25f, 1.f, .5f);
    // custom user transforms
    glMultMatrixf(&_left_upper_arm_transform[0][0]);

    glColor3f(0.0f,1.0f,0.0f);
    glVertexPointer(3, GL_FLOAT, 0, &_upper_arm_vertices[0]);
    glDrawArrays(GL_QUAD_STRIP, 0, 10); // base without top & bottom
    glDrawArrays(GL_QUADS, 10, 8); // base top & bottom

    // left lower arm
    glTranslatef(-1.f+.25f, 1.f, 0.f);
    // custom user transforms
    glMultMatrixf(&_left_lower_arm_transform[0][0]);

    glColor3f(1.0f,0.0f,0.0f);
    glVertexPointer(3, GL_FLOAT, 0, &_lower_arm_vertices[0]);
    glDrawArrays(GL_QUAD_STRIP, 0, 10); // base without top & bottom
    glDrawArrays(GL_QUADS, 10, 8); // base top & bottom

    // left hand
    glTranslatef(-.1f, -.125f, -.125f);
    // custom user transforms
    glMultMatrixf(&_left_hand_transform[0][0]);

    glColor3f(1.0f,1.0f,0.0f);
    glVertexPointer(3, GL_FLOAT, 0, &_hand_vertices[0]);
    glDrawArrays(GL_QUAD_STRIP, 0, 10); // base without top & bottom
    glDrawArrays(GL_QUADS, 10, 8); // base top & bottom

    glPopMatrix();
}

void Robot::DrawRightArm() const {
    glPushMatrix();
    
    // left upper arm
    glTranslatef(1.5f, 1.f, .5f);
    // custom user transforms
    glMultMatrixf(&_right_upper_arm_transform[0][0]);

    glColor3f(0.0f,1.0f,0.0f);
    glVertexPointer(3, GL_FLOAT, 0, &_upper_arm_vertices[0]);
    glDrawArrays(GL_QUAD_STRIP, 0, 10); // base without top & bottom
    glDrawArrays(GL_QUADS, 10, 8); // base top & bottom

    // left lower arm
    glTranslatef(0.f, 1.f, 0.f);
    // custom user transforms
    glMultMatrixf(&_right_lower_arm_transform[0][0]);

    glColor3f(1.0f,0.0f,0.0f);
    glVertexPointer(3, GL_FLOAT, 0, &_lower_arm_vertices[0]);
    glDrawArrays(GL_QUAD_STRIP, 0, 10); // base without top & bottom
    glDrawArrays(GL_QUADS, 10, 8); // base top & bottom

    // left hand
    glTranslatef(1.f, -.125f, -.125f);
    // custom user transforms
    glMultMatrixf(&_right_hand_transform[0][0]);

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

    // Use the Projection Matrix
    glMatrixMode(GL_PROJECTION);
    // Reset Matrix
    glLoadIdentity();

    // Set the viewport to be the entire window
    glViewport(0, 0, width, height);

    auto fovy = 45;
    auto ratio = 1.0 * width / height;
    gluPerspective(fovy, ratio, 1, 1000);

    // Switch back to the Modelview
    glMatrixMode(GL_MODELVIEW);
}