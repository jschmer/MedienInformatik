#include <Point2D.h>

#include <algorithm>

//
// Point2D static initialization
std::function<bool(Point2D, Point2D)> Point2D::less_y = [](const Point2D& first, const Point2D& second) { return first.y < second.y; };

//
// Point2D struct definition
Point2D::Point2D()
    : x(0.f), y(0.f), w(0.f)
{}

Point2D::Point2D(float x, float y, float w)
    : x(x), y(y), w(w)
{}

Point2D::Point2D(int x, int y, int w)
    : x(static_cast<float>(x)), y(static_cast<float>(y)), w(static_cast<float>(w))
{}

Point2D::Point2D(sf::Vector2f v)
    :  x(static_cast<float>(v.x)), y(static_cast<float>(v.y)), w(1.f)
{}

Point2D::Point2D(glm::vec3 v)
    :  x(static_cast<float>(v.x)), y(static_cast<float>(v.y)), w(static_cast<float>(v.z))
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

const Point2D operator*(const Point2D& lhs, const float& rhs)
{
  /* Erzeugen eines neuen Objektes, dessen Attribute gezielt einzeln gesetzt werden. Oder: */
  Point2D tmp(lhs); //Kopie des linken Operanden
  tmp *= rhs;
  return tmp;
}

const Point2D operator*(const glm::mat3& lhs, const Point2D& rhs)
{
    glm::vec3 tmp;
    tmp.x = rhs.x;
    tmp.y = rhs.y;
    tmp.z = rhs.w;

    tmp = lhs * tmp;

    return tmp;
}

const Point2D operator+(const Point2D& lhs, const Point2D& rhs)
{
  /* Erzeugen eines neuen Objektes, dessen Attribute gezielt einzeln gesetzt werden. Oder: */
  Point2D tmp(lhs); //Kopie des linken Operanden
  tmp += rhs;
  return tmp;
}

const Point2D operator-(const Point2D& lhs, const Point2D& rhs)
{
  /* Erzeugen eines neuen Objektes, dessen Attribute gezielt einzeln gesetzt werden. Oder: */
  Point2D tmp(lhs); //Kopie des linken Operanden
  tmp -= rhs;
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

Point2D& Point2D::operator-=(const Point2D& rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    return *this;
}

Point2D Point2D::operator-() const {
      Point2D tmp;
      tmp.x = -x;
      tmp.y = -y;
      return tmp;
   }