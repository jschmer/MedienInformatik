/* 
 * Copyright (c) 2013 Jens Schmer
 * This file is distributed under the MIT License.
 * Consult COPYING within this package for further information. 
 */
#pragma once

#include <functional>

#include <glm/glm.hpp>
#include <SFMLApp.h>

struct Point2D {
    float x, y, w;

    Point2D();
    Point2D(float x, float y, float w = 1.0f);
    Point2D(int x, int y, int w = 1);
    Point2D(sf::Vector2f v);
    Point2D(glm::vec3 v);

    Point2D& operator*=(const float& factor);
    Point2D& operator+=(const Point2D& rhs);
    Point2D& operator-=(const Point2D& rhs);
    Point2D operator-() const;
    operator glm::vec2(void) const; 

    static std::function<bool(Point2D, Point2D)> less_y;
};

const Point2D operator*(const float& lhs, const Point2D& rhs);
const Point2D operator*(const Point2D& lhs, const float& rhs);
const Point2D operator*(const glm::mat3& lhs, const Point2D& rhs);
const Point2D operator+(const Point2D& lhs, const Point2D& rhs);
const Point2D operator-(const Point2D& lhs, const Point2D& rhs);