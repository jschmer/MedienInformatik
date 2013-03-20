#pragma once

#include <Event.h>

#include <SFML/Graphics.hpp>

class SFMLApp : public Event {
public:
    SFMLApp();
    ~SFMLApp();

    // render loop
    int Run();

private:
    // helper
    void RenderPixels();
    void RenderScene();

    // render loop events
    bool OnInit();
    void OnRender();
    void OnCleanup();

    // event handlers
    void OnClosed() override;
    void OnResized(uint width, uint height) override;

private:
    // instance data
    bool _running;
    sf::RenderWindow  _window;
};