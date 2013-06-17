#pragma once

#include "string"
#include "Vector.h"

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