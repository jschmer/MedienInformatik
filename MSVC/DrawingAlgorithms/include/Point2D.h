#include <functional>

#include <SFMLApp.h>

struct Point2D {
    float x, y;

    Point2D();
    Point2D(const float x, const float y);
    Point2D(const int x, const int y);
    Point2D(sf::Vector2f v);

    Point2D& operator*=(const float& factor);
    Point2D& operator+=(const Point2D& rhs);

    static std::function<bool(Point2D, Point2D)> less_y;
};

const Point2D operator*(const float& lhs, const Point2D& rhs);
const Point2D operator+(const Point2D& lhs, const Point2D& rhs);