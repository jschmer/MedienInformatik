#include <SFMLApp.h>

#include <algorithm>

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

SFMLApp::SFMLApp()
    : _running(true),
    _show_help(false)
{
    _help_info = sf::Text(sf::String(L"Press 'h' for help..."), sf::Font(), 15u);
    _help_info.setPosition(4, 0);

    _help_text = sf::Text(sf::String(L"ESC = Close the App"), sf::Font(), 15u);
    _help_text.setPosition(4, 30);
}

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
    // loading font...
    if (!_font.loadFromFile("geo_1.ttf"))
        return false;

    _help_text.setFont(_font);
    _help_info.setFont(_font);

    // create the window
    const auto width = 800u, height = 600u;
    _window.create(sf::VideoMode(width, height), "OpenGL", sf::Style::Default, sf::ContextSettings(32));
    _window.setVerticalSyncEnabled(true);

    // getting the view
    _view = _window.getView();
    _window.setView(_view);

    // initializing viewport and perspective
    OnResized(width, height);

    // Set the screen color for clearing
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f); // rgba

    return true;
}

void SFMLApp::OnRender()
{
    // clear the buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RenderScene();
    RenderHelpText();
}

void SFMLApp::OnCleanup() {
}

///////////////////////////////////////////////////////////
// Drawing text
void SFMLApp::RenderHelpText()
{   
    // preserve opengl states! if you don't do this, the text will overwrite
    // everything else we told opengl earlier! (like drawing a triangle)
    _window.pushGLStates();

    // always draw help info
    _window.draw(_help_info);

    // toggled by OnKeyReleased() event
    if (_show_help)
        _window.draw(_help_text);

    _window.popGLStates();
}

///////////////////////////////////////////////////////////
// Drawing a opengl scene
void SFMLApp::RenderScene()
{
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

    // adjusting SFML view
    _view.setSize(static_cast<float>(width), static_cast<float>(height));
    _view.setCenter(sf::Vector2f(width/2.f, height/2.f));
    _window.setView(_view);
}

void SFMLApp::OnKeyReleased(sf::Keyboard::Key key, bool ctrl, bool alt, bool shift, bool system) {
    typedef sf::Keyboard::Key Key;

    switch (key) {
    case Key::H:
        _show_help = !_show_help;
        break;
    }
}