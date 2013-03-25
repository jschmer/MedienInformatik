#include <Point2D.h>

#include <algorithm>

//
// Point2D struct definition
Point2D::Point2D()
    : x(0U), y(0U)
{}

Point2D::Point2D(const float x, const float y)
    : x(x), y(y)
{}

Point2D::Point2D(const int x, const int y)
    : x(static_cast<float>(x)), y(static_cast<float>(y))
{}

//
// Point2D free standing operator functions
const Point2D operator*(const float& lhs, const Point2D& rhs)
{
  /* Erzeugen eines neuen Objektes, dessen Attribute gezielt einzeln gesetzt werden. Oder: */
  Point2D tmp(rhs); //Kopie des linken Operanden
  tmp *= lhs;
  return tmp;
}

const Point2D operator+(const Point2D& lhs, const Point2D& rhs)
{
  /* Erzeugen eines neuen Objektes, dessen Attribute gezielt einzeln gesetzt werden. Oder: */
  Point2D tmp(lhs); //Kopie des linken Operanden
  tmp += rhs;
  return tmp;
}

//
// Point2D member operators
Point2D& Point2D::operator*=(float const& factor)
{
  x *= factor;
  y *= factor;
  return *this;
}

Point2D& Point2D::operator+=(const Point2D& rhs)
{
    x += rhs.x;
    y += rhs.y;
    return *this;
}