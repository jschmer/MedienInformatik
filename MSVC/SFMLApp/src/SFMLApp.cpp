/* 
 * Copyright (c) 2013 Jens Schmer
 * This file is distributed under the MIT License.
 * Consult COPYING within this package for further information. 
 */
#include <SFMLApp.h>

#include <cstdio>
#include <algorithm>

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

const auto HELP_TEXT = L"\
ESC = Close the App\n\
F = Toggle FPS\n\
M = Toggle Mouse position\n\
";

SFMLApp::SFMLApp()
    : SFMLApp(800u, 600u, "SFMLApp")
{
}

SFMLApp::SFMLApp(uint width, uint height, const char* app_name, uint style, sf::ContextSettings gl_context)
    : _running(true),
    _show_help(false),
    _show_fps(true),
    _show_mouse_pos(false),
    _fps(0),
    _frametime(0.f),
    _fps_clock(),
    _frame_clock(),
    _width(width),
    _height(height),
    _style(style),
    _app_name(app_name),
    _gl_context(gl_context)
{
    _help_info = sf::Text(sf::String(L"Press 'h' for help..."), sf::Font(), 15u);
    _help_info.setPosition(4, 0);

    _help_text = sf::Text(sf::String(HELP_TEXT), sf::Font(), 15u);
    _help_text.setPosition(4, 30);

    _fps_text = sf::Text("", sf::Font(), 15u);

    _mouse_pos = sf::Text("", sf::Font(), 15u);
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

bool SFMLApp::InitFont(const char* font_path) {
    // loading font...
    if (!_font.loadFromFile(font_path))
        return false;

    _help_text.setFont(_font);
    _help_info.setFont(_font);
    _fps_text.setFont(_font);
    _mouse_pos.setFont(_font);

    return true;
}

bool SFMLApp::OnInit() {
    if (!InitFont("geo_1.ttf"))
        return false;

    // create the window
    _window.create(sf::VideoMode(_width, _height), _app_name, _style, _gl_context);
    _window.setVerticalSyncEnabled(true);

    // getting the view
    _view = _window.getView();
    _window.setView(_view);

    // initializing viewport and perspective
    OnResized(_width, _height);

    // Set the screen color for clearing
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f); // rgba

    return true;
}

void SFMLApp::OnRender()
{
    RenderScene();
    RenderHelpText();
    RenderFPS();
    RenderMousePos();
}

void SFMLApp::OnCleanup() {
}

///////////////////////////////////////////////////////////
// Drawing text
void SFMLApp::RenderHelpText(sf::String additional)
{   
    // preserve opengl states! if you don't do this, the text will overwrite
    // everything else we told opengl earlier! (like drawing a triangle)
    _window.pushGLStates();

    // always draw help info
    _window.draw(_help_info);

    // toggled by OnKeyReleased() event
    if (_show_help) {
        auto help_copy = _help_text;
        help_copy.setString(_help_text.getString() + additional);
        _window.draw(help_copy);
    }

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
// Drawing the current mouse position
void SFMLApp::RenderMousePos() {
    // convert mouse positions to c-string
    char mouse_x[32];
    char mouse_y[32];
    sprintf_s(mouse_x, "%5d", _current_mouse_pos.x);
    sprintf_s(mouse_y, "%5d", _current_mouse_pos.y);

    // trim leading whitespace
    auto mouse_x_start = mouse_x;
    auto mouse_y_start = mouse_y;
    while (*mouse_x_start == ' ')
        ++mouse_x_start;
    while (*mouse_y_start == ' ')
        ++mouse_y_start;

    // get length
    auto xlen = strlen(mouse_x_start);
    auto ylen = strlen(mouse_y_start);

    // catch the case if the mouse position is too long
    if (xlen + ylen + 1 >= 32)
        return;

    // concat them
    char mouse_pos[32];
    sprintf_s(mouse_pos, "%s/%s", mouse_x_start, mouse_y_start);

    // convert to sf::String
    sf::String mouse_pos_str(mouse_pos);

    // generate a SFML text for rendering
    _mouse_pos.setString(mouse_pos_str);
    _mouse_pos.setPosition(4, static_cast<float>(_window.getSize().y - 40u));

    if (_show_mouse_pos) {
        // draw the mouse position text
        _window.pushGLStates();
        _window.draw(_mouse_pos);
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

    // updating member vars
    _width  = width;
    _height = height;
}

void SFMLApp::OnKeyReleased(sf::Keyboard::Key key, bool ctrl, bool alt, bool shift, bool system) {
    typedef sf::Keyboard::Key Key;

    switch (key) {
    case Key::Escape:
        OnClosed();
        break;
    case Key::H:
        _show_help = !_show_help;
        break;
    case Key::F:
        _show_fps = !_show_fps;
        break;
    case Key::M:
        _show_mouse_pos = !_show_mouse_pos;
        break;
    }
}

void SFMLApp::OnMouseMoved(int x, int y) {
    _current_mouse_pos.x = x;
    _current_mouse_pos.y = y;
}