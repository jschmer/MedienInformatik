#pragma once

#include "string"
#include "Vector.h"

#include <glm/glm.hpp>
using glm::vec3;

class Ray;

class Surface
{
    std::string name;	

public:
    Surface(const char* surface_name)
        : name(surface_name)
    {}

    std::string getName() { return name; };

    virtual double intersect(Ray& ray) = 0;
    virtual Vector get_normal(Vector &v) = 0;
};

class QuadricSurface : public Surface {

public:
    double a, b, c, d, e, f, g, h, j, k;
    QuadricSurface()
        : Surface(""), a(0.0), b(0.0), c(0.0), d(0.0), e(0.0), f(0.0), g(0.0), h(0.0), j(0.0), k(0.0)
    {};

    QuadricSurface(char *n, double _a, double _b, double _c, double _d, double _e, double _f, double _g, double _h, double _j, double _k)
        : Surface(n), a(_a), b(_b), c(_c), d(_d), e(_e), f(_f), g(_g), h(_h), j(_j), k(_k)
    {};

    double intersect(Ray& ray) override;
    Vector get_normal(Vector &v) override;
};

class TriangleSurface : public Surface {
    vec3 v0, v1, v2;
    Vector _normal;

public:
    TriangleSurface()
        : Surface(""), v0(), v1(), v2()
    {}

    TriangleSurface(const char* n, vec3 v1, vec3 v2, vec3 v3)
        : Surface(n), v0(v1), v1(v2), v2(v3)
    {
        auto normal = glm::normalize(glm::cross(v2-v1, v3-v1));
        _normal = Vector(normal.x, normal.y, normal.z);
    }

    double intersect(Ray& ray) override;
    Vector get_normal(Vector &v) override;
};