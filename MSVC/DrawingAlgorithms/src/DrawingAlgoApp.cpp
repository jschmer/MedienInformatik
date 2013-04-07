#include <DrawingAlgoApp.h>

#include <algorithm>
#include <fstream>
#include <sstream>

#include <SFML/OpenGL.hpp>

// signum function
template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

//
// Pixel struct definition
DrawingAlgoApp::Pixel::Pixel()
    : R(0), G(0), B(0)
{}

DrawingAlgoApp::Pixel::Pixel(uint R, uint G, uint B)
    : R(R), G(G), B(B)
{}

DrawingAlgoApp::Pixel::Pixel(uint hex) {
    R = (hex & 0xFF0000) >> 2*8;
    G = (hex & 0x00FF00) >> 8;
    B = hex & 0x0000FF;
}

//
// DrawingAlgoApp class definition
DrawingAlgoApp::DrawingAlgoApp()
    : Super(800u, 600u, "Drawing Algorithms", sf::Style::Close),
    _num_pixels(_width*_height),
    _draw_type(DrawingType::None),
    _mouse_pos_cache(0, 0)
{
    _pixel_data.reset(new Pixel[_width*_height]);

    ClearPixelData();
}

DrawingAlgoApp::~DrawingAlgoApp()
{}

//
// render loop events
bool DrawingAlgoApp::OnInit() {
    if (!Super::OnInit())
        return false;

    // adjusting help text
    const auto append_text = R"(
C = Clear Pixelbuffer
X = Capture Screen
Modes:
1 = None
2 = Line
3 = Circle
4 = Bezier
)";

    auto help = _help_text.getString();
    help.insert(help.getSize(), append_text);
    _help_text.setString(help);

    // Set the screen color for clearing
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // rgba

    return true;
}

void DrawingAlgoApp::OnRender() {
    // render the pixel buffer 
    RenderPixelArray();

    // render "HUD"
    Super::RenderHelpText();
    Super::RenderFPS();
    Super::RenderMousePos();
}

//
// drawing helper
void DrawingAlgoApp::SetPixel(uint x, uint y, const Pixel &pix) {
    if (x >= _width || y >= _height)
        return;

    auto idx = (_height - 1 - y) * _width + x;
    idx = std::min(idx, _num_pixels-1);

    auto size = sizeof(pix);
    memcpy(&_pixel_data[idx], &pix, size);
}

void DrawingAlgoApp::ClearPixelData() {
    memset(_pixel_data.get(), 0, sizeof(Pixel)*_width*_height);
}

void DrawingAlgoApp::RenderPixelArray() {
    // draw pixel data
    glDrawPixels(_width, _height, GL_RGB, GL_UNSIGNED_BYTE, _pixel_data.get());

    // Flush drawing commands
    glFlush();
}

void DrawingAlgoApp::SaveAsPPM(const char* filename) {
    using std::endl;
    
    const auto magic_number = "P6";
    const auto bytes_per_pixel = 3;

    std::stringstream ss;
    ss << _width;
    const auto width = ss.str();

    ss = std::stringstream();
    ss << _height;
    const auto height = ss.str();

    ss = std::stringstream();
    ss << 255;
    const auto max_color = ss.str();

    std::ofstream out(filename);
    out << magic_number << endl;
    out << width << " " << height << endl;
    out << max_color << "\n";

    for (auto h = 0U; h < _height; ++h) {
        for (auto w = 0U; w < _width; ++w) {
             auto& pix = _pixel_data[(_height - h - 1)*_width + w];
             out << pix.G << pix.B << pix.R;
        }
    }
}

//
// drawing algorithms
void DrawingAlgoApp::DrawLineBresenham(uint x1, uint y1, uint x2, uint y2, const Pixel& pix) {
    // Startpunkt
    int x = x1, y = y1;

    int dx = x2 - x1;
    int dy = y2 - y1;

    // Schrittrichtung für Diagonalschritt ermitteln (sgn liefert -1, 0, 1)
    int diagonal_dx = sgn(dx);
    int diagonal_dy = sgn(dy);

    int errSR, errLR;               // Fehleranpassung in schneller Richtung (SR) und langsamer Richtung (LR)
    int parallel_dx, parallel_dy;   // Schrittweite für Parallelschritt 
    int num_elements;               // Anzahl zu zeichnender Pixel

    // Unterscheidung zwischen schneller und langsamer Richtung
    if (std::abs(dx) > std::abs(dy)) {
        // x ist schnelle Richtung, d.h. mache keinen Schritt in y-Richtung bei Parallelschritt
        parallel_dx = diagonal_dx;
        parallel_dy = 0;
        errSR = std::abs(dx)*2;
        errLR = std::abs(dy)*2;
        num_elements = std::abs(dx);    // Anzahl zu zeichnender Pixel in schneller Richtung
    } else {
        // y ist schnelle Richtung, d.h. mache keinen Schritt in x-Richtung bei Parallelschritt
        parallel_dx = 0;
        parallel_dy = diagonal_dy;
        errSR = std::abs(dy)*2;
        errLR = std::abs(dx)*2;
        num_elements = std::abs(dy);    // Anzahl zu zeichnender Pixel in schneller Richtung
    }

    // Fehlervariable initialisieren
    int error = errLR - num_elements;

    for (int i = 0; i < num_elements; ++i) {
        SetPixel(x, y, pix);
        
        // Fehlervariable aktualisieren (Schritt in schneller Richtung)
        error += errLR;
        if(error <= 0) {
            // Nur Schritt in schnelle Richtung (nur parallel)
            x += parallel_dx;
            y += parallel_dy;
        }
        else {
            // Schritt in langsame und schnelle Richtung (diagonal)
            x += diagonal_dx;
            y += diagonal_dy;

            // Fehlervariable aktualisieren (Schritt in langsamer Richtung)
            error -= errSR;
        }
    }
}

void DrawingAlgoApp::DrawLineMidpoint(uint x1, uint y1, uint x2, uint y2, const Pixel& pix) {
    int x = x1;
    int y = y1;
    int dx = x2 - x1;
    int dy = y2 - y1;
    int f = dy - dx/2;

    // immer Schritt in schnelle Richtung, gelegentlich in langsame Richtung
    for (int i = 1; i <= dx; ++i) { 
        SetPixel(x, y, pix);
        x++;
        if (f > 0) { 
            y += 1;
            f -= dx; }
        f += dy;
    }
}

void DrawingAlgoApp::DrawCircle(uint posx, uint posy, uint radius, const Pixel& pix) {
    int x1 = 0;
    int y1 = radius;
    int f  = 1 - radius;
    int dx = 3;
    int dy = 2 - 2*radius;

    while(x1 <= y1) {
        // drawing each possible arc
        SetPixel(posx+x1, posy-y1, pix);
        SetPixel(posx+x1, posy+y1, pix);
        SetPixel(posx-x1, posy-y1, pix);
        SetPixel(posx-x1, posy+y1, pix);
        SetPixel(posx+y1, posy-x1, pix);
        SetPixel(posx+y1, posy+x1, pix);
        SetPixel(posx-y1, posy-x1, pix);
        SetPixel(posx-y1, posy+x1, pix);

        x1++;
        if(f > 0) {
            y1--;
            f  += dy;
            dy += 2;
        }
        f  += dx;
        dx +=2;
    }
}

void DrawingAlgoApp::DrawBezier(const std::vector<Point2D>& support_points) {
    const auto t_stepsize = .01f;
    const auto size = support_points.size();
    
    // TODO: rasterize curve
    std::vector<Point2D> points;

    if (size < 2U)
        return;

    std::unique_ptr<std::unique_ptr<Point2D[]>[]> b(new std::unique_ptr<Point2D[]>[size]);
    for (auto i = 0U; i < size; ++i)
        b[i].reset(new Point2D[size]);

    for (float t = .0f; t <= 1.f; t += t_stepsize) {
        // de Casteljau algorithm
        // for i = 0 .. n
        for (auto i = 0u; i < size; ++i) {
            //    b(i,0) = bi
            b[i][0] = support_points[i];
        }

        // for j = 1 .. n
        for (auto j = 1u; j < size; ++j) {
            // for i = j .. n
            for (auto i = j; i < size; ++i) { 
                // b(i,j) = (1-t) * b(i-1,j-1) + t * b(i,j-1)
                b[i][j] = (1.f - t) * b[i-1][j-1] + t * b[i][j-1];
            }
        }

        // p(t) = b(n,n)
        points.push_back(b[size-1][size-1]);
    }

    // render support points in red
    for (auto& p: support_points) {
        DrawCircle(static_cast<uint>(p.x), static_cast<uint>(p.y), 1, Pixel(0xFF0000));
    }

    // render curve
    auto p0 = *points.cbegin();
    for (auto it = points.cbegin() + 1; it != points.cend(); ++it) {
        auto& p1 = *it;

        DrawLineBresenham(static_cast<uint>(p0.x),
                          static_cast<uint>(p0.y),
                          static_cast<uint>(p1.x),
                          static_cast<uint>(p1.y));

        p0 = p1;
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
        ClearPixelData();
        
        // clear bezier points
        _bezier_points.clear();
        break;
    case Key::X:
        SaveAsPPM();
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
    case Key::Num4:
        _draw_type = DrawingType::Bezier;
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
        DrawLineBresenham(cachex, cachey, x, y);
        break;
    case DrawingType::Circle:
        {
            // calc distance between cache and current point
            auto dx = cachex - x;
            auto dy = cachey - y;
            auto radius = std::sqrt(dx*dx + dy*dy);

            DrawCircle(cachex, cachey, static_cast<int>(radius));
        }
        break;
    case DrawingType::Bezier:
        // add point to support_points!
        _bezier_points.push_back(Point2D(cachex, cachey));

        // clear pixeldata
        ClearPixelData();

        // render bezier
        DrawBezier(_bezier_points);
        break;
    }
}