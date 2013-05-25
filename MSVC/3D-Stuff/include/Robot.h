/* 
 * Copyright (c) 2013 Jens Schmer
 * This file is distributed under the MIT License.
 * See COPYING within this package for further information. 
 */
#pragma once
#include <vector>

#include <glm/glm.hpp>

#include <SFML/OpenGL.hpp>
#include <SFMLApp.h>

#include "CameraInterface.hpp"

class Robot : public CameraInterface
{
public:
    Robot();
    ~Robot();

    void render();

    // keyboard events
    sf::String HelpInfo() const;
    void OnKeyPressed(sf::Keyboard::Key key, bool ctrl, bool alt, bool shift, bool system);

    // other events
    void OnResized(uint width, uint height);

private:
    void DrawBase() const;
    void DrawLeftArm() const;
    void DrawRightArm() const;

private:
    std::vector<GLfloat> _base_vertices;
    std::vector<GLfloat> _upper_arm_vertices;
    std::vector<GLfloat> _lower_arm_vertices;
    std::vector<GLfloat> _hand_vertices;
    glm::mat4 _base_transform;
    glm::mat4 _left_upper_arm_transform;
    glm::mat4 _right_upper_arm_transform;
    glm::mat4 _left_lower_arm_transform;
    glm::mat4 _right_lower_arm_transform;
    glm::mat4 _left_hand_transform;
    glm::mat4 _right_hand_transform;
};

