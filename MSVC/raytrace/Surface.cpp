#include "StdAfx.h"
#include "Surface.h"
#include "Ray.h"

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