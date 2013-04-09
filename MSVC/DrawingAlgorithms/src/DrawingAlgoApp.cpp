#include <DrawingAlgoApp.h>

#include <algorithm>
#include <fstream>
#include <sstream>
#include <regex>
#include <string>
using std::string;
using std::ostringstream;

#include <SFML/OpenGL.hpp>

// signum function
template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

// join
template <typename T>
string join(const T& v, const string& delim) {
    ostringstream s;
    for (const auto& i : v) {
        if (&i != &v[0]) {
            s << delim;
        }
        s << i;
    }
    return s.str();
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
    _mouse_pos_cache(0, 0),
    _config("DrawingAlgoApp.cfg")
{
    _pixel_data.reset(new Pixel[_width*_height]);

    ClearPixelData();

    // setup config file monitor
    updateConfigData();
    _config.setCallbackOnUpdate([&](ConfigFile &cfg) {
        cfg.disableCallback();
        updateConfigData();
        cfg.enableCallback();

        // redraw curves with updated values
        switch (_draw_type) {
        case DrawingType::Bezier:
            // clear pixeldata
            ClearPixelData();

            // render bezier
            DrawBezier(_bezier_points);
            break;
        case DrawingType::BSpline:
            // clear pixeldata
            ClearPixelData();

            // render bezier
            DrawBSpline(_bspline_points, _bspline_knot_vector);
            break;
        }
    });
    _config.monitorChanges();
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
5 = B-Spline
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
// Config loader helper
void DrawingAlgoApp::updateConfigData() {
    const std::string d_t             = _config.get("General", "delta_t", "0.01");
    const std::string bspline_degree  = _config.get("B-Spline", "poly_degree", "3");
    const std::string num_supp_points = _config.get("B-Spline", "num_support_points", "0");

    try {
        _delta_t = std::stof(d_t);

        auto poly_deg_tmp = std::stoi(bspline_degree);
        auto num_supp_points_tmp = std::stoi(num_supp_points);

        if (_bspline_poly_degree != poly_deg_tmp || _bspline_points.size() != num_supp_points_tmp) {
            // TODO: recalc knot vector and print out to config file
            auto new_knot_vec_len = _bspline_points.size() + poly_deg_tmp + 1;
            std::string knot_vec_str = "";

            if (new_knot_vec_len >= 2U * (poly_deg_tmp + 1U)) {
                // B-Spline is possible with these parameters

                // new vector
                decltype(_bspline_knot_vector) new_knot_vec;

                // populate new vector
                for (auto i = 0; i < poly_deg_tmp + 1; ++i)
                    new_knot_vec.push_back(0.f);

                auto individual_factors = new_knot_vec_len - (poly_deg_tmp + 1) * 2;
                auto inc = 1.f / (individual_factors + 1);
            
                float val = inc;
                for (auto i = 0U; i < individual_factors; ++i, val += inc)
                    new_knot_vec.push_back(val);

                for (auto i = 0; i < poly_deg_tmp + 1; ++i)
                    new_knot_vec.push_back(1.f);

                // TODO: write new knot vector to config file
                knot_vec_str = "[" + join(new_knot_vec, ", ") + "]";

                // update internal poly degree
                _bspline_poly_degree = poly_deg_tmp;
                // and knot vector
                _bspline_knot_vector = new_knot_vec;
            }
            else {
                // B-Spline not possible with this configuration

                auto supp_points = _bspline_points.size();
                // print "not possible with # support points"
                // to knot_vec config property

                knot_vec_str = "not possible with " + std::to_string(supp_points) + " support points";
            }

            _bspline_poly_degree = poly_deg_tmp;
            _config.put("B-Spline", "num_support_points", std::to_string(_bspline_points.size()));
            _config.put("B-Spline", "knot_vec", knot_vec_str);
        }
        else {
            // load updated knot_vector
            std::string bspline_knot_vec = _config.get("B-Spline", "knot_vec", "[0, 0, 0, 0, 0.49, 0.75, 1, 1, 1, 1]");

            // processing B-Spline knot vector
            // converting knot_vec into internal float vector
            const std::regex r(R"<<(([0-9]+(.[0-9]+)?))<<");
            std::smatch m;

            std::vector<float> temp;
            while (std::regex_search(bspline_knot_vec, m, r)) {
                std::string knot_val = m[0].str();
                temp.push_back(stof(knot_val));

                // next iteration after found match
                bspline_knot_vec = m.suffix().str();
            }

            // assign new vector
            _bspline_knot_vector = temp;
        }
        
    }
    catch (std::exception) {
        // don't let exceptions bubble up, failed reading from config files isn't a critical error
    }
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
    const auto size = support_points.size();

    if (size < 2U)
        return;

    std::vector<Point2D> points;
    std::vector<std::vector<Point2D>> b(size, std::vector<Point2D>(size));

    for (float t = .0f; t <= 1.f; t += _delta_t) {
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

void DrawingAlgoApp::DrawBSpline(const std::vector<Point2D>& support_points, const std::vector<float> knot_vector) {
    const auto support_points_size = support_points.size(); // p
    const auto& polynom_degree = _bspline_poly_degree; // n

    // knot_vector content:
    // n + 1   0en
    // n + 1   1en
    // rest: frei wählbar

    // größe: n + p + 1

    // render support points in red
    for (auto& p: support_points) {
        DrawCircle(static_cast<uint>(p.x), static_cast<uint>(p.y), 1, Pixel(0xFF0000));
    }

    if (knot_vector.size() != polynom_degree + 1 + support_points_size) {
        return;
    }

    std::vector<std::vector<Point2D>> b(support_points_size, std::vector<Point2D>(polynom_degree + 1));
    std::vector<Point2D> points;
    
    // p = Anzahl Stützpunkte
    // n = Polynomgrad

    const auto& n = polynom_degree;

    for (float t = .0f; t <= 1.f + _delta_t/2.f; t += _delta_t) {
        // Berechne i so, dass t_i ≤ t < t_i+1, 0 ≤ i ≤ p
        const int i = [&](float t) {
            int i = 0;
            while (i < support_points_size && knot_vector[i] <= t)
                ++i;
            return --i;
        }(t);

        // for j = 0 .. n 
        for (int j = 0; j <= n; ++j) {
        //  for l = i-n+j .. i
            for (int l = i - n + j; l <= i; ++l) {
        //      if j = 0
                if (j == 0) {
        //          b(l,0) = b_l
                    b[l][0] = support_points[l];
                }
        //      else 
                else {
        //          t’ = (t – t_l) / (t_l+n+1-j – t_l)
                    auto _t = (t - knot_vector[l]) / (knot_vector[l+n+1-j] - knot_vector[l]);
        //          b(l,j) = (1 – t’) * b(l-1,j-1) + t’ * b(l,j-1)
                    b[l][j] = (1 - _t) * b[l-1][j-1] + _t * b[l][j-1];
                }
            }
        }
        // p(t) = b(i,n)
        points.push_back(b[i][n]);
    }

    // render curve
    auto p0 = *points.cbegin();
    for (auto it = points.cbegin() + 1; it != points.cend(); ++it) {
        auto& p1 = *it;

        DrawLineBresenham(static_cast<uint>(p0.x),
                          static_cast<uint>(p0.y),
                          static_cast<uint>(p1.x),
                          static_cast<uint>(p1.y),
                          Pixel(0xFFFF00));

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
        
        // clear support points
        _bezier_points.clear();
        _bspline_points.clear();
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
    case Key::Num5:
        _draw_type = DrawingType::BSpline;
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
    case DrawingType::BSpline:
        // add point to support_points!
        _bspline_points.push_back(Point2D(cachex, cachey));
        updateConfigData();

        // clear pixeldata
        ClearPixelData();

        // render bezier
        DrawBSpline(_bspline_points, _bspline_knot_vector);
        break;
    }
}