// raytrace.cpp : Defines the entry point for the console application.
//

#include "stdafx.h" 
#include "Ray.h"
#include "Color.h"
#include "Image.h"
#include "vector"
#include <glm/glm.hpp>

const double SCREENWIDTH  = 1000;
const double SCREENHEIGHT = 1000;

using namespace std;

vector<int> indices;
vector<glm::vec3> vertices;
vector<Surface*>  surfaces;
vector<Property> properties;
vector<Objekt>   objekte;
vector<Light>    lights;

// -- scene parameter ------------
int g_width  = 1250;
int g_height = 1250;

Vector eye(0, 0, SCREENHEIGHT * 8.0);
Vector lookat(0, 0, 0);
Vector up (0, 1, 0);

auto fovy   = 45.0;
auto aspect = g_width/static_cast<double>(g_height);

Color background(0, 0, 0);
// -------------------------------

extern "C" {
    extern FILE *yyin;
    int yyparse();

    void add_light(char *n, double dirx, double diry, double dirz, double colr, double colg, double colb) {
        fprintf(stderr,"  adding light %f %f %f %f %f %f\n", dirx, diry, dirz, colr, colg, colb);
        lights.push_back(Light(Vector(dirx,diry,dirz).normalize(), Color(colr, colg, colb)));
    };
    void add_quadric(char *n, double a, double b, double c, double d, double e, double f, double g, double h, double j, double k) {
        fprintf(stderr,"  adding quadric %s %f %f %f %f %f %f %f %f %f %f %f %f\n", n, a,b,c,d,e,f,g,h,j,k);
        surfaces.push_back(new QuadricSurface(n, a,b,c,d,e,f,g,h,j,k));
    };
    void add_vertex(double x, double y, double z) {
        vertices.emplace_back(x, y, z);
    };
    void add_index(int idx) {
        indices.push_back(idx);
    };
    void add_poly(const char* name) {
        // construct TriangleSurface out of indices and vertices
        assert(indices.size() % 3 == 0);
        assert(indices.size() >= 3);

        for (auto i = 0u; i < indices.size(); ++i) {
            auto& v1 = vertices[indices[i] - 1];
            auto& v2 = vertices[indices[++i] - 1];
            auto& v3 = vertices[indices[++i] - 1];

            surfaces.push_back(new TriangleSurface(name, v1, v2, v3));
        }
    };
    void add_quad(double idx0, double idx1, double idx2, double idx3) {

    };
    void add_property(char *n,  double ar, double ag, double ab, double r, double g, double b, double s, double m) {
        fprintf(stderr,"  adding prop %f %f %f %f %f\n", r, g, b, s, m);
        properties.push_back(Property(n, Color(ar, ag, ab), Color(r, g, b), s, m));
    };
    void add_objekt(char *ns, char *np) {
        Surface *s = NULL;
        Property *p = NULL;
        string ss(ns);
        string sp(np);

        for(auto& surface : surfaces) {
            if(surface->getName() == ss) {
                s = surface;

                for(auto& prop : properties) {
                    if(prop.getName() == sp) {
                        p = &prop;
                    }
                }

                objekte.push_back(Objekt(s, p));
                fprintf(stderr, "  adding object: surface %s, property %s\n", ns, np);

            }
        }

        if(s == NULL) {
            fprintf(stderr, "Surface not found: %s\n", ns);
            exit(1);
        }
        if(p == NULL) {
            fprintf(stderr, "Property not found: %s\n", np);
            exit(1);
        }

        vertices.clear();
        indices.clear();
    }

    void set_resolution(int width, int height) {
        g_width = width;
        g_height = height;
    }

    void define_eye(double x, double y, double z) {
        eye = Vector(x, y, z);
    }
    void define_lookat(double x, double y, double z) {
        lookat = Vector(x, y, z);
    }
    void define_up(double x, double y, double z) {
        up = Vector(x, y, z);
    }
    void define_fovy(double dfovy) {
        fovy = dfovy;
    }
    void define_aspect(double daspect) {
        aspect = daspect;
    }
    void define_background_color(double r, double g, double b) {
        background = Color(r, g, b);
    }
}

int main(int argc, _TCHAR* argv[])
{
    /* parse the input file */
    printf(">>> Reading input...\n");

    auto file = "data/dflt.data";
    file = "data/scene.data";

    yyin = fopen(file, "r");
    if(yyin == NULL) {
        fprintf(stderr, "Error: Konnte Datei \"%s\" nicht oeffnen\n", file);
        return 1;
    }
    yyparse();
    fclose (yyin);
    printf(">>> Reading input DONE!\n\n");

    // setup image, camera and raytracer variables
    int Xresolution = g_width;
    int Yresolution = g_height;

    printf("Using resolution: %d %d\n", Xresolution, Yresolution);
    printf("Fovy: %f\nAspect: %f\n", fovy, aspect);

    // TODO: integrate fovy and aspect (SCREENWIDTH and SCREENHEIGHT?)
    double dx = SCREENWIDTH / (double)Xresolution;
    double dy = SCREENHEIGHT / (double)Yresolution;
    double y = -0.5 * SCREENHEIGHT;

    // calc ray direction
    auto dir = lookat.vsub(eye).normalize(); // (lookat - eye).normalize()

    // params: direction, origin, depth, background_color
    Ray	ray(dir, eye , 0, background);

    Image bild(Xresolution, Yresolution);

    // do the raytracing!
    printf("\n>>> Rendering...\n");
    for (int scanline=0; scanline < Yresolution; scanline++) {

        printf("%4d\r", Yresolution-scanline);
        y += dy;
        double x = -0.5 * SCREENWIDTH;

        for (int sx=0; sx < Xresolution; sx++) {
            ray.setDirection(Vector(x, y, 0.0).vsub(ray.getOrigin()).normalize());
            x += dx;
            Color color = ray.shade(objekte, lights);

            bild.set(sx, scanline,
                color.r > 1.0 ? 255 : int(255 * color.r),
                color.g > 1.0 ? 255 : int(255 * color.g),
                color.b > 1.0 ? 255 : int(255 * color.b));
        }
    }
    printf(">>> Rendering DONE!\n");

    printf("\n>>> Saving picture...\n");
    char *name = "raytrace-bild.ppm";
    bild.save(name);
    printf(">>> Saving picture DONE!\n");

    return 0;
}

