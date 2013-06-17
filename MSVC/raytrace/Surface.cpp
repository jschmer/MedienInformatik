#include "StdAfx.h"
#include "Surface.h"
#include "Ray.h"

using glm::dot;
using glm::cross;

Vector QuadricSurface::get_normal(Vector &v) {
    Vector normal( 
        v.dot(Vector(a+a, b, c)) + d,
        v.dot(Vector(b, e+e, f)) + g,
        v.dot(Vector(c, f, h+h)) + j);

    return normal.normalize();
}

double QuadricSurface::intersect(Ray& ray) {
    double t = -1.0,
        acoef, bcoef, ccoef, root, disc;

    auto& direction = ray.getDirection();
    auto& origin = ray.getOrigin();

    acoef = Vector(direction.dot(Vector(a, b, c)),
        e*direction.y + f*direction.z,
        h*direction.z).dot(direction);

    bcoef = 
        Vector(d, g, j).dot(direction) + 
        origin.dot( Vector( 
        direction.dot(Vector(a+a , b, c)), 
        direction.dot(Vector(b, e+e, f)), 
        direction.dot(Vector(c, f, h+h))));

    ccoef = origin.dot( 
        Vector(Vector(a, b, c).dot(origin) + d,
        e*origin.y + f*origin.z + g,
        h*origin.z + j)) + k;

    if (1.0 + acoef != 1.0) {
        disc = bcoef * bcoef - 4 * acoef * ccoef;
        if (disc > -DBL_EPSILON) {
            root = sqrt( disc );
            t = ( -bcoef - root ) / ( acoef + acoef );
            if (t < 0.0) {
                t = ( -bcoef + root ) / ( acoef + acoef );
            }
        }
    }
    return ((0.001 < t) ? t : -1.0);
}

double TriangleSurface::intersect(Ray& ray) {
    auto normal = vec3(_normal.x, _normal.y, _normal.z);
    auto& rd = ray.getDirection();
    auto ray_dir = vec3(rd.x, rd.y, rd.z);
    auto& ro = ray.getOrigin();
    auto ray_pos = vec3(ro.x, ro.y, ro.z);

    float nDotRay = dot(normal, ray_dir);
    if (nDotRay == 0)
        return -1.0f; // ray parallel to triangle

    float d = dot(normal, v0);
    float t = -(dot(normal, ray_pos) - d) / nDotRay;

    // ray behind triangle
    if (t < 0)
        return -1.0f;

    // compute intersection point
    vec3 Phit = ray_pos + t * ray_dir;
    vec3 v0p = Phit - v0;

    auto v0v1 = v1-v0;

    // inside-out test edge0
    float v = dot(normal, cross(v0v1, v0p));
    if (v < 0)
        return -1.0f; // P outside triangle

    auto v1v2 = v2-v1;

    // inside-out test edge1
    float w = dot(normal, cross(v1v2, Phit - v1));
    if (w < 0)
        return -1.0f; // P outside triangle

    auto v2v0 = v0-v2;

    // inside-out test edge2
    float u = dot(normal, cross(v2v0, Phit - v2));
    if (u < 0)
        return -1.0f; // P outside triangle

    return t;
}
Vector TriangleSurface::get_normal(Vector &v) {
    return _normal;
}