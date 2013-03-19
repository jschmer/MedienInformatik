#include <algorithm>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

///////////////////////////////////////////////////////////
// Called to draw scene every time the window is repainted
void RenderScene()
{
    // Clear the window with current clearing color
    glClear(GL_COLOR_BUFFER_BIT);

    glRotatef(1.f, 0.0f, 0.0f, 1.0f);

    glBegin(GL_TRIANGLES);
        glVertex3f(-0.5, -0.5, -2.0);
        glVertex3f( 0.5,  0.0, -2.0);
        glVertex3f( 0.0,  0.5, -2.0);
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

int main() {
    const auto width = 600u, height = 800u;

    // create the window
    sf::Window window(sf::VideoMode(height, width), "OpenGL", sf::Style::Close, sf::ContextSettings(32));
    window.setVerticalSyncEnabled(true);

    // initializing viewport and perspective
    ChangeSize(height, width);

    // load resources, initialize the OpenGL states, ...
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

    // run the main loop
    bool running = true;
    while (running)
    {
        // handle events
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                // end the program
                running = false;
            }
            else if (event.type == sf::Event::Resized)
            {
                // adjust the viewport when the window is resized
                ChangeSize(event.size.width, event.size.height);
            }
        }

        // clear the buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw...
        RenderScene();

        // end the current frame (internally swaps the front and back buffers)
        window.display();
    }

    return EXIT_SUCCESS;
}