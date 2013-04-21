#pragma once

#include <glm/glm.hpp>

#include "Point2D.h"

inline glm::mat3 translate(glm::vec2 v) {
    return glm::mat3(glm::vec3(1, 0, 0),
                     glm::vec3(0, 1, 0),
                     glm::vec3(v.x, v.y, 1));
}

inline glm::mat3 translate(Point2D p) {
    return glm::mat3(glm::vec3(1, 0, 0),
                     glm::vec3(0, 1, 0),
                     glm::vec3(p.x, p.y, 1));
}

inline glm::mat3 scale(float x, float y) {
    return glm::mat3(glm::vec3(x, 0, 0),
                     glm::vec3(0, y, 0),
                     glm::vec3(0, 0, 1));
}

// CCW!
inline glm::mat3 rotate(float angle) {
    using glm::cos;
    using glm::sin;

    return glm::mat3(glm::vec3(cos(angle),  sin(angle), 0),
                     glm::vec3(-sin(angle), cos(angle), 0),
                     glm::vec3(0, 0, 1));
}