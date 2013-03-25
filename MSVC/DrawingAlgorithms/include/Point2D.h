typedef unsigned int uint;

struct Point2D {
    uint x, y;

    Point2D();
    Point2D(uint x, uint y);
    Point2D& operator*=(const float& factor);
    Point2D& operator+=(const Point2D& rhs);
};

const Point2D operator*(const float& lhs, const Point2D& rhs);
const Point2D operator+(const Point2D& lhs, const Point2D& rhs);