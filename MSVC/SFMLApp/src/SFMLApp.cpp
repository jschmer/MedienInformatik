#include <SFMLApp.h>

#include <cstdio>
#include <algorithm>

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

const auto HELP_TEXT = L"\
ESC = Close the App\n\
F = Toggle FPS\n\
";

SFMLApp::SFMLApp()
    : _running(true),
    _show_help(false),
    _show_fps(false),
    _fps(0),
    _frametime(0.f),
    _fps_clock(),
    _frame_clock()
{
    _help_info = sf::Text(sf::String(L"Press 'h' for help..."), sf::Font(), 15u);
    _help_info.setPosition(4, 0);

    _help_text = sf::Text(sf::String(HELP_TEXT), sf::Font(), 15u);
    _help_text.setPosition(4, 30);

    _fps_text = sf::Text("", sf::Font(), 15u);
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

        // start the clock for getting time to render a frame
        _frame_clock.restart();

        // clear the buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render something
        OnRender();

        // end the current frame (internally swaps the front and back buffers)
        _window.display();

        // store the elapsed time for rendering a frame
        _frametime = _frame_clock.getElapsedTime().asSeconds();
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
    _fps_text.setFont(_font);

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
    RenderScene();
    RenderHelpText();
    RenderFPS();
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
    glRotatef(1.7f, 0.0f, 1.0f, 0.0f);

    glBegin(GL_TRIANGLES);
        glVertex3f(-0.5, -0.5, 0.0);
        glVertex3f( 0.5,  0.0, 0.0);
        glVertex3f( 0.0,  0.5, 0.0);
    glEnd();

    // Flush drawing commands
    glFlush();
}

///////////////////////////////////////////////////////////
// Drawing the FPS (counts the produced frames in one second)
void SFMLApp::RenderFPS()
{
    ++_fps;
    auto elapsed = _fps_clock.getElapsedTime().asSeconds();

    // display fps after one second
    if (elapsed >= 1.f) {
        // convert to c-string
        char fps_buffer[32];
        sprintf_s(fps_buffer, "%20d", _fps);

        // trim leading whitespace
        auto fps_start = fps_buffer;
        while (*fps_start == ' ')
            ++fps_start;

        // convert to sf::String
        sf::String fps_str(fps_start);

        // prepend descriptive text
        fps_str.insert(0, sf::String("FPS: "));

        // generate a SFML text for rendering
        _fps_text.setString(fps_str);
        _fps_text.setPosition(4, static_cast<float>(_window.getSize().y - 20u));

        // reset fps and clock
        _fps = 0;
        _fps_clock.restart(); 
    }

    if (_show_fps) {
        // draw the fps text
        _window.pushGLStates();
        _window.draw(_fps_text);
        _window.popGLStates();
    }
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
    case Key::F:
        _show_fps = !_show_fps;
        break;
    }
}