/* 
 * Copyright (c) 2013 Jens Schmer
 * This file is distributed under the MIT License.
 * See COPYING within this package for further information. 
 */
#pragma once

#include <glm/glm.hpp>

#include <SFML/OpenGL.hpp>

#include <vector>
#include <string>

/*
An element of vector must be:

    Copyable, i.e. have a constructor compatible with T(const T&)
    Assignable, i.e. have an operator= compatible with operator=(const T&).
    Default-constructible, i.e. have a constructor compatible with T().
*/
struct Face {
    Face(const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2)
        : v0(v0),
        v1(v1),
        v2(v2),
        normal(glm::normalize(glm::cross(v1 - v0, v2 - v0)))
    {}

    ~Face() {

    }

    const glm::vec3& v0;
    const glm::vec3& v1;
    const glm::vec3& v2;
    const glm::vec3 normal;
};

struct Obj {
    std::vector<glm::vec3> vertices;
    std::vector<const Face> faces;
    std::vector<const GLfloat> gl_vertices;
    std::vector<const GLfloat> gl_normals;
};

void loadObj(std::string, Obj& obj);