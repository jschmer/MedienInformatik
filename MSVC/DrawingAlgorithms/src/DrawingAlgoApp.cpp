#include <DrawingAlgoApp.h>

#include <SFML/OpenGL.hpp>

//
// Pixel struct definition
DrawingAlgoApp::Pixel::Pixel()
    : R(0), G(0), B(0)
{}

DrawingAlgoApp::Pixel::Pixel(uint R, uint G, uint B)
    : R(R), G(G), B(B)
{}

//
// DrawingAlgoApp class definition
DrawingAlgoApp::DrawingAlgoApp()
    : _width(400u),
    _height(300u),
    _num_pixels(_width*_height),
    _draw_type(DrawingType::None),
    _mouse_pos_cache(0, 0)
{
    _pixel_data.reset(new Pixel[_width*_height]);

    // clear pixel data
    memset(_pixel_data.get(), 0, _width*_height);
}

DrawingAlgoApp::~DrawingAlgoApp()
{}

bool DrawingAlgoApp::OnInit() {
    // loading font...
    if (!_font.loadFromFile("geo_1.ttf"))
        return false;

    _help_text.setFont(_font);
    _help_info.setFont(_font);
    _fps_text.setFont(_font);

    // adjusting help text
    const auto append_text = "\
C = Clear Pixelbuffer\n\
1 = Mode None\n\
2 = Mode Line\n\
3 = Mode Circle\n";

    auto help = _help_text.getString();
    help.insert(help.getSize(), append_text);
    _help_text.setString(help);

    // create the window
    _window.create(sf::VideoMode(_width, _height), "Drawing Algorithms", sf::Style::Default, sf::ContextSettings(32));
    _window.setVerticalSyncEnabled(true);

    // getting the view
    _view = _window.getView();
    _window.setView(_view);

    // Set the screen color for clearing
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // rgba

    return true;
}

void DrawingAlgoApp::OnRender() {
    // do some drawing (into pixel buffer)
    //DrawLineBresenham(0, 0, 50, 50);
    //DrawLineMidpoint(10, 0, 60, 50);
    //DrawCircle(50, 50, 10);

    // render the pixel buffer 
    RenderPixelArray();

    // render "HUD"
    Super::RenderHelpText();
    Super::RenderFPS();
}

//
// drawing helper
void DrawingAlgoApp::setPixel(uint x, uint y, Pixel &pix) {
    if (x >= _width || y >= _height)
        return;

    auto idx = (_height - 1 - y) * _width + x;
    idx = std::min(idx, _num_pixels-1);

    auto size = sizeof(pix);
    memcpy(&_pixel_data[idx], &pix, size);
}

void DrawingAlgoApp::RenderPixelArray() {
    // draw pixel data
    glDrawPixels(_width, _height, GL_RGB, GL_UNSIGNED_BYTE, _pixel_data.get());

    // Flush drawing commands
    glFlush();
}

//
// drawing algorithms
void DrawingAlgoApp::DrawLineBresenham(uint x1, uint y1, uint x2, uint y2) {
    Pixel pix(0, 255, 0);
    
    int dx = x2 - x1; 
    int dy = y2 - y1; 
    int const1 = dx + dx; 
    int const2 = dy + dy; 
    int error = const2 - dx; 
    while(x1 <= x2) { 
        setPixel(x1, y1, pix); 
        x1++; 
        if(error <= 0) 
            error += const2; 
        else { 
            y1++; error += const2 - const1; 
        }
    }
}

void DrawingAlgoApp::DrawLineMidpoint(uint x1, uint y1, uint x2, uint y2) {
    Pixel pix(0, 255, 0);

    int x = x1;
    int y = y1;
    int dx = x2 - x1;
    int dy = y2 - y1;
    int f = dy - dx/2;
    for (int i = 1; i <= dx; ++i) { 
        setPixel(x, y, pix);
        x++;
        if (f > 0) { 
            y += 1;
            f -= dx; }
        f += dy;
    }
}

void DrawingAlgoApp::DrawCircle(uint posx, uint posy, uint radius) {
    Pixel pix(0, 255, 0);

    int x1 = 0;
    int y1 = radius;
    int f = 1-radius;
    int dx = 3;
    int dy = 2 - 2*radius;
    while(x1 <= y1) { 
        setPixel(posx+x1, posy-y1, pix);
        setPixel(posx+x1, posy+y1, pix);
        setPixel(posx-x1, posy-y1, pix);
        setPixel(posx-x1, posy+y1, pix);
        setPixel(posx+y1, posy-x1, pix);
        setPixel(posx+y1, posy+x1, pix);
        setPixel(posx-y1, posy-x1, pix);
        setPixel(posx-y1, posy+x1, pix);
        x1++;
        if(f > 0) { 
            y1--; 
            f += dy; 
            dy += 2; 
        } 
        f += dx; 
        dx +=2; 
    }
}

//
// event handler
void DrawingAlgoApp::OnKeyReleased(sf::Keyboard::Key key, bool ctrl, bool alt, bool shift, bool system) {
    typedef sf::Keyboard::Key Key;
    
    // key 'h' and 'f' are handled by SFMLApp
    Super::OnKeyReleased(key, ctrl, alt, shift, system);

    switch (key) {
    case Key::C:
        // clear pixel data
        memset(_pixel_data.get(), 0, _width*_height*sizeof(Pixel));
        break;
    case Key::Num1:
        _draw_type = DrawingType::None;
        break;
    case Key::Num2:
        _draw_type = DrawingType::Line;
        break;
    case Key::Num3:
        _draw_type = DrawingType::Circle;
        break;
    }
}

void DrawingAlgoApp::OnMouseButtonPressed(sf::Mouse::Button button, int x, int y) {
    _mouse_pos_cache.x = x;
    _mouse_pos_cache.y = y;
}

void DrawingAlgoApp::OnMouseButtonReleased(sf::Mouse::Button button, int x, int y) {
    auto& cachex = _mouse_pos_cache.x;
    auto& cachey = _mouse_pos_cache.y;
    
    switch (_draw_type) {
    case DrawingType::Line:
        DrawLineMidpoint(cachex, cachey, x, y);
        break;
    case DrawingType::Circle:
        // calc distance between cache and this point
        auto dx = cachex - x;
        auto dy = cachey - y;
        auto radius = std::sqrt(dx*dx + dy*dy);

        DrawCircle(_mouse_pos_cache.x, _mouse_pos_cache.y, static_cast<int>(radius));
        break;
    }
}