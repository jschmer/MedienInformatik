#pragma once

#include <Event.h>

#include <SFML/Graphics.hpp>

//
// @brief   Standard SFMLApp with a RenderWindow and a default simple application lifecycle.
//          Also handles window resizing and window closing.
//          It even loads a default Font for text rendering: geo_1.ttf.
//          The font must be in the working directory of the application if you want to see a window ;-)
//          There is also a little help text to demonstrate text rendering.
//          This class can be used as a starting point for deriving your own class.
//          Derive directly from Event if you'll implement your own application lifecycle.
//
class SFMLApp : public Event {
public:
    SFMLApp();
    ~SFMLApp();

    // @brief   Run the program (e.g. its render loop)
    virtual int Run();

    // helper functions
protected:
    bool InitFont(const char* font_path = "geo_1.ttf");
    void RenderHelpText();
    void RenderScene();
    void RenderFPS();
    void RenderMousePos();

    // render loop events
protected:
    virtual bool OnInit();
    virtual void OnRender();
    virtual void OnCleanup();

    // custom event handlers
protected:
    virtual void OnClosed() override;
    virtual void OnResized(uint width, uint height) override;
    virtual void OnKeyReleased(sf::Keyboard::Key key, bool ctrl, bool alt, bool shift, bool system) override;
    virtual void OnMouseMoved(int x, int y);

    // instance data
protected:
    bool _running;
    sf::RenderWindow  _window;
    sf::Font          _font;
    sf::View          _view;

    bool              _show_help;
    sf::Text          _help_text;
    sf::Text          _help_info;
    sf::Text          _fps_text;
    sf::Text          _mouse_pos;

    sf::Clock         _frame_clock;
    float             _frametime;
    sf::Clock         _fps_clock;
    int               _fps;
    bool              _show_fps;

    bool              _show_mouse_pos;
    sf::Vector2i      _current_mouse_pos;
};