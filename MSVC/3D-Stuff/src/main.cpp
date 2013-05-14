/* 
 * Copyright (c) 2013 Jens Schmer
 * This file is distributed under the MIT License.
 * Consult COPYING within this package for further information. 
 */
#include <OpenGLApp.h>

#include <Freeglut/glut.h>

int main(int argc, char* argv[]) {
    OpenGLApp app(&argc, argv);
    return app.Run();
}