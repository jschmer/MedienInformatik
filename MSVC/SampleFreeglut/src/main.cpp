// Includes
#include <algorithm>

#define FREEGLUT_STATIC
#include <freeglut/glut.h>

///////////////////////////////////////////////////////////
// Called to draw scene every time the window is repainted
void RenderScene()
{
    // Clear the window with current clearing color
    glClear(GL_COLOR_BUFFER_BIT);

    glRotatef(.01f, 0.0f, 0.0f, 1.0f);

    glBegin(GL_TRIANGLES);
        glVertex3f(-0.5, -0.5, -5.0);
        glVertex3f( 0.5,  0.0, -5.0);
        glVertex3f( 0.0,  0.5, -5.0);
    glEnd();

    // Flush drawing commands
    glFlush();
}

///////////////////////////////////////////////////////////
// Called if the window is resized
void ChangeSize(int w, int h)
{
    // Prevent a divide by zero, when window is too short
    // (you cant make a window of zero width).
    h = std::max(h, 1);
    float ratio = 1.0f * w / h;

    // Use the Projection Matrix
    glMatrixMode(GL_PROJECTION);

    // Reset Matrix
    glLoadIdentity();

    // Set the viewport to be the entire window
    glViewport(0, 0, w, h);

    // Set the correct perspective.
    gluPerspective(45, ratio, 1, 1000);

    // Switch back to the Modelview
    glMatrixMode(GL_MODELVIEW);
}

///////////////////////////////////////////////////////////
// Setup the rendering state
void SetupRC()
{
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
}

///////////////////////////////////////////////////////////
// Main program entry point
int main(int argc, char* argv[])
{
    // Window initialization
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    glutCreateWindow("Drawing Algorithms");

    // Callback registration
    glutDisplayFunc(RenderScene);
    glutReshapeFunc(ChangeSize);
    glutIdleFunc(RenderScene);

    SetupRC();

    // neverending main loop
    glutMainLoop();
    
    return 0;
}
