#include <SFMLApp.h>

#include <algorithm>

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

SFMLApp::SFMLApp()
    : _running(true)
{}

SFMLApp::~SFMLApp()
{}

///////////////////////////////////////////////////////////
// Render loop
int SFMLApp::Run() {
    // setup
    if (!OnInit())
        return EXIT_FAILURE;

    // run the main loop
    while (_running)
    {
        // handle events
        sf::Event event;
        while (_window.pollEvent(event))
        {
            OnEvent(event);
        }

        // render something
        OnRender();

        // end the current frame (internally swaps the front and back buffers)
        _window.display();
    }

    // do the cleanup
    OnCleanup();

    return EXIT_SUCCESS;
}

bool SFMLApp::OnInit() {
    const auto width = 800u, height = 600u;

    // create the window
    _window.create(sf::VideoMode(width, height), "OpenGL", sf::Style::Default, sf::ContextSettings(32));
    _window.setVerticalSyncEnabled(true);

    // initializing viewport and perspective
    OnResized(width, height);

    // clearing screen color
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f); // rgba

    return true;
}

void SFMLApp::OnRender()
{
    // clear the buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RenderScene();
}

void SFMLApp::OnCleanup() {
}

///////////////////////////////////////////////////////////
// Drawing a pixel array
void SFMLApp::RenderPixels()
{
    // Clear the window with current clearing color
    glClear(GL_COLOR_BUFFER_BIT);

    const auto width = 800u, height = 600u;
    const auto bpp = 3u;
    const auto width_bytes = bpp*width;

    auto data = new BYTE[bpp*width*height];
    memset(data, 0, bpp*width*height);

    for (auto y = 0u; y < height; ++y) {
        for (auto x = 0u; x < width_bytes; x += bpp) {
            auto idx = y*width_bytes + x;
            data[idx]   = 0;     // R
            data[++idx] = y%256; // G
            data[++idx] = 0;     // B
        }
    }

    glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, data);

    delete[] data;

    // Flush drawing commands
    glFlush();
}

///////////////////////////////////////////////////////////
// Drawing a opengl scene
void SFMLApp::RenderScene()
{
    // Clear the window with current clearing color
    glClear(GL_COLOR_BUFFER_BIT);

    glRotatef(1.f, 0.0f, 0.0f, 1.0f);
    glRotatef(1.5f, 0.0f, 1.0f, 0.0f);

    glBegin(GL_TRIANGLES);
        glVertex3f(-0.5, -0.5, 0.0);
        glVertex3f( 0.5,  0.0, 0.0);
        glVertex3f( 0.0,  0.5, 0.0);
    glEnd();

    // Flush drawing commands
    glFlush();
}

///////////////////////////////////////////////////////////
// Event handler functions
void SFMLApp::OnClosed() {
    _running = false;
}

void SFMLApp::OnResized(uint width, uint height) {
    // Prevent a divide by zero, when window is too short
    // (you cant make a window of zero width).
    height = std::max(height, 1u);
    float ratio = 1.0f * width / height;

    // Use the Projection Matrix
    glMatrixMode(GL_PROJECTION);

    // Reset Matrix
    glLoadIdentity();

    // Set the viewport to be the entire window
    glViewport(0, 0, width, height);

    // Set the correct perspective.
    gluPerspective(45, ratio, 1, 1000);

    const double eye[]    = {0.0, 0.0, 2.0};
    const double center[] = {0.0, 0.0, 0.0};
    const double up[]     = {0.0, 1.0, 0.0};

    // setting camera
    gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], up[0], up[1], up[2]);

    // Switch back to the Modelview
    glMatrixMode(GL_MODELVIEW);
}