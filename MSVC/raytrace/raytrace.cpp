// raytrace.cpp : Defines the entry point for the console application.
//

#include "stdafx.h" 
#include "Ray.h"
#include "Color.h"
#include "Image.h"
#include "vector"

const double SCREENWIDTH = 1000;
const double SCREENHEIGHT = 1000;

using namespace std;

vector<Surface> surfaces;
vector<Property> properties;
vector<Objekt> objekte;
vector<Light> lights;

int g_width  = 1250;
int g_height = 1250;

Vector eye(0, 0, SCREENHEIGHT * 8.0);
Vector lookat(0, 0, 0);

extern "C" {
	extern FILE *yyin;
	int yyparse();

	void add_light(char *n, double dirx, double diry, double dirz, double colr, double colg, double colb) {
		fprintf(stderr,"  adding light %f %f %f %f %f %f\n", dirx, diry, dirz, colr, colg, colb);
		lights.push_back(Light(Vector(dirx,diry,dirz).normalize(), Color(colr, colg, colb)));
	};
	void add_quadric(char *n, double a, double b, double c, double d, double e, double f, double g, double h, double j, double k) {
		fprintf(stderr,"  adding quadric %s %f %f %f %f %f %f %f %f %f %f %f %f\n", n, a,b,c,d,e,f,g,h,j,k);
		surfaces.push_back(Surface(n, a,b,c,d,e,f,g,h,j,k));
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

		for(vector<Surface>::iterator i = surfaces.begin(); i != surfaces.end(); ++i) 
			if(i->getName() == ss) {
				s = &(*i);
				break;
			}
		for(vector<Property>::iterator i = properties.begin(); i != properties.end(); ++i) 
			if(i->getName() == sp) {
				p = &(*i);
				break;
			}

		if(s == NULL) {
			fprintf(stderr, "Surface not found: %s\n", ns);
			exit(1);
		}
		if(p == NULL) {
			fprintf(stderr, "Property not found: %s\n", np);
			exit(1);
		}
		objekte.push_back(Objekt(s, p));
		fprintf(stderr, "  adding object: surface %s, property %s\n", ns, np);
	}

	void define_resolution(int width, int height) {
		g_width = width;
		g_height = height;
	}

	void define_eye(double x, double y, double z) {
		eye = Vector(x, y, z);
	}
	void define_lookat(double x, double y, double z) {
		lookat = Vector(x, y, z);
	}
}

int main(int argc, _TCHAR* argv[])
{
	printf(">>> Reading input...\n");

	/* parse the input file */
	auto file = "data/dflt.data";
	yyin = fopen(file, "r");
	if(yyin == NULL) {
		fprintf(stderr, "Error: Konnte Datei \"%s\" nicht oeffnen\n", file);
		return 1;
	}
	yyparse();
	fclose (yyin);
	
	printf(">>> Reading input DONE!\n\n");

	int Xresolution = g_width;
	int Yresolution = g_height;

	printf("Using resolution: %d %d\n\n", Xresolution, Yresolution);

	double dx = SCREENWIDTH / (double)Xresolution;
	double dy = SCREENHEIGHT / (double)Yresolution;
	double y = -0.5 * SCREENHEIGHT;
	
	// calc ray direction
	auto dir = lookat.vsub(eye).normalize();

	// direction, origin
	Ray	ray(dir, eye , 0);

	Image bild(Xresolution, Yresolution);

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

	char *name = "raytrace-bild.ppm";
	bild.save(name);

	return 0;
}

