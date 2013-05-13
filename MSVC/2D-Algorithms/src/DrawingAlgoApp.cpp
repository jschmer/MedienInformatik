/* 
 * Copyright (c) 2013 Jens Schmer
 * This file is distributed under the MIT License.
 * Consult COPYING within this package for further information. 
 */
#include <DrawingAlgoApp.h>

#include <cassert>
#include <algorithm>
#include <set>
#include <fstream>
#include <sstream>
#include <regex>
#include <string>
#include <vector>
#include <cmath>

using std::string;
using std::ostringstream;
using std::vector;

#include <glm/gtx/transform.hpp>
#include <SFML/OpenGL.hpp>
#include <2d_transforms.h>

//#include <boost/heap/fibonacci_heap.hpp>

// signum function
template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

inline double fract_part(double d) {
    double intpart;
    auto fractpart = std::modf(d, &intpart);
    return fractpart;
}

inline double int_part(double d) {
    double intpart;
    auto fractpart = std::modf(d, &intpart);
    return intpart;
}


// precondition: an intersection exists!
inline Point2D GetLineIntersection(Point2D p00, Point2D p01, Point2D p10, Point2D p11) {
    float x12 = p00.x - p01.x;
    float x34 = p10.x - p11.x;
    float y12 = p00.y - p01.y;
    float y34 = p10.y - p11.y;

    float c = x12 * y34 - y12 * x34;

    // Intersection
    float a = p00.x * p01.y - p00.y * p01.x;
    float b = p10.x * p11.y - p10.y * p11.x;

    float x = (a * x34 - b * x12) / c;
    float y = (a * y34 - b * y12) / c;

    return Point2D(x, y);
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
// Color struct definition
Color operator *(float a, Color b) {
    return b * a;
}

Color::Color()
    : R(0), G(0), B(0)
{}

Color::Color(uint R, uint G, uint B)
    : R(R), G(G), B(B)
{}

Color::Color(uint hex) {
    R = (hex & 0xFF0000) >> 2*8;
    G = (hex & 0x00FF00) >> 8;
    B = hex & 0x0000FF;
}

Color Color::operator *(float b) {
    R*=b;
    G*=b;
    B*=b;
    return *this;
}

Color Color::operator +(Color other) {
    R += other.R;
    G += other.G;
    B += other.B;
    return *this;
}

//
// DrawingAlgoApp class definition
DrawingAlgoApp::DrawingAlgoApp()
    : Super(width, height, "Drawing Algorithms", sf::Style::Close),
    _num_Colors(_width*_height),
    _draw_type(DrawingType::None),
    _mouse_pos_cache(0, 0),
    _config("2D_Algorithms.cfg"),
    _transform_mat(1.0f),
    _transform_type(TransformationType::None),
    _transform_origin(0, 0),
    _show_current_mode(true),
    _antialiase(false)
{
    _current_mode_text = sf::Text("", sf::Font(), 15u);

    _Color_data.reset(new Color[_width*_height]);

    ClearColorData();

    // setup config file monitor
    updateConfigData();
    _config.setCallbackOnUpdate([&](ConfigFile &cfg) {
        cfg.disableCallback();
        updateConfigData();
        cfg.enableCallback();

        // redraw curves with updated values
        DrawCurrentMode();
    });
    _config.monitorChanges();

    // debug test stuff
    using glm::vec2;
    using glm::vec3;
    using glm::mat3;

    // column-major! rightmost transformation comes first!
    //_transform_mat = translate(vec2(10, -5));
    //_transform_mat = scale(1.5f, 2) * _transform_mat;
    //_transform_mat = rotate(glm::radians(90.0f)) * _transform_mat;

    Point2D p(10, 2);
    auto p1 = _transform_mat * p;

    // default clipping rectangle
    _clipping_rect.xmin = 0;
    _clipping_rect.xmax = width;
    _clipping_rect.ymin = 0;
    _clipping_rect.ymax = height;

    auto pasd = GetLineIntersection(Point2D(2, 0), Point2D(2, 2), Point2D(1, 1), Point2D(3, 1));

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
V = Show Current Mode
C = Clear Colorbuffer
X = Capture Screen

Modes:
1 = None
2 = Line
3 = Circle
4 = Bezier
5 = B-Spline
6 = B-Spline Closed
7 = Catmul-Rom-Spline
8 = Fill Rectangle
9 = Fill Triangle
0 = Fill Polygon
P = Pythagoras
B = Bouncing Rectangle
O = Set Clipping Rect

F1 = Toggle Antialiasing

Transformations:
(control with Arrow Keys)
Q = None
W = Translate
E = Scale
R = Rotate
T = Shear
Z = Set Transformation Origin
)";

    auto help = _help_text.getString();
    help.insert(help.getSize(), append_text);
    _help_text.setString(help);

    _current_mode_text.setFont(_font);

    // Set the screen color for clearing
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // rgba

    return true;
}

void DrawingAlgoApp::OnRender() {
    // render bouncing rectangle
    if (_draw_type == DrawingType::BouncingRect)
        RenderBouncingRect();
    // render the Color buffer 
    else
        RenderColorArray();


    // render "HUD"
    Super::RenderHelpText();
    Super::RenderFPS();
    Super::RenderMousePos();
    RenderCurrentMode();
}

void DrawingAlgoApp::RenderCurrentMode() {
    std::string title("Current Mode: ");

    switch (_draw_type) {
    case DrawingType::None:
        title += "None";
        break;
    case DrawingType::Bezier:
        title += "Bezier";
        break;
    case DrawingType::BSpline:
        title += "B-Spline";
        break;
    case DrawingType::BSplineClosed:
        title += "B-Spline Closed";
        break;
    case DrawingType::CatmulRomSpline:
        title += "Catmull-Rom Spline";
        break;
    case DrawingType::Circle:
        title += "Circle";
        break;
    case DrawingType::FillPolygon:
        title += "Fill Polygon";
        break;
    case DrawingType::FillRectangle:
        title += "Fill Rectangle";
        break;
    case DrawingType::FillTriangle:
        title += "Fill Triangle";
        break;
    case DrawingType::Line:
        title += "Line";
        break;
    case DrawingType::SetClippingRect:
        title += "Setting Clipping Rectangle";
        break;
    case DrawingType::BouncingRect:
        title += "Bouncing Rectangle";
        break;
    default:
        title += "Not implemented! Baka!";
        break;
    }

    title += " | ";

    switch (_transform_type) {
    case TransformationType::None:
        title += "None";
        break;
    case TransformationType::Rotate:
        title += "Rotate";
        break;
    case TransformationType::Scale:
        title += "Scale";
        break;
    case TransformationType::SetOrigin:
        title += "Set Origin";
        break;
    case TransformationType::Shear:
        title += "Shear";
        break;
    case TransformationType::Translate:
        title += "Translate";
        break;
    }

    // convert to sf::String
    sf::String current_mode_str(title);

    // generate a SFML text for rendering
    _current_mode_text.setString(current_mode_str);
    _current_mode_text.setPosition(4, static_cast<float>(_window.getSize().y - 60u));

    if (_show_current_mode) {
        // draw the mouse position text
        _window.pushGLStates();
        _window.draw(_current_mode_text);
        _window.popGLStates();
    }
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

        if (_bspline_poly_degree != poly_deg_tmp || _vertices.size() != num_supp_points_tmp) {
            // TODO: recalc knot vector and print out to config file
            auto new_knot_vec_len = _vertices.size() + poly_deg_tmp + 1;
            std::string knot_vec_str = "";

            if (new_knot_vec_len >= 2U * (poly_deg_tmp + 1U)) {
                // B-Spline is possible with these parameters

                // new vector
                decltype(_bspline_knot_vector) new_knot_vec;

                if (_draw_type == DrawingType::BSpline) {
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
                }
                else if (_draw_type == DrawingType::BSplineClosed) {

                    auto inc = 1.f/(_vertices.size());
                    auto val = 0.f;
                    // populate new vector, m+2 entries with m = number of control points - 1
                    for (auto i = 0; i < _vertices.size()+1; ++i, val += inc)
                        new_knot_vec.push_back(val);
                }

                // write new knot vector to config file
                knot_vec_str = "[" + join(new_knot_vec, ", ") + "]";

                // update internal poly degree
                _bspline_poly_degree = poly_deg_tmp;
                // and knot vector
                _bspline_knot_vector = new_knot_vec;
            }
            else {
                // B-Spline not possible with this configuration

                auto supp_points = _vertices.size();
                // print "not possible with # support points"
                // to knot_vec config property

                knot_vec_str = "not possible with " + std::to_string(supp_points) + " support points";
            }

            _bspline_poly_degree = poly_deg_tmp;
            _config.put("B-Spline", "num_support_points", std::to_string(_vertices.size()));
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

// origin is upper left corner
void DrawingAlgoApp::SetColor(uint x, uint y, const Color &color) {
    if (x >= _width || y >= _height)
        return;

    auto idx = (_height - 1 - y) * _width + x;

    auto size = sizeof(color);
    memcpy(&_Color_data[idx], &color, size);
}

void DrawingAlgoApp::SetColor(const Point2D pixel, const Color &color) {
    SetColor(pixel.x, pixel.y, color);
}

void DrawingAlgoApp::ClearColorData() {
    memset(_Color_data.get(), 0, sizeof(Color)*_width*_height);
}

void DrawingAlgoApp::DrawPoints(const std::vector<Point2D>& points, const Color& color) {
    for (auto& p: points) {
        DrawCircle(p, 1, color);
    }
}

void DrawingAlgoApp::RenderColorArray() {
    // draw Color data
    glDrawPixels(_width, _height, GL_RGB, GL_UNSIGNED_BYTE, _Color_data.get());

    // Flush drawing commands
    glFlush();
}

void DrawingAlgoApp::SaveAsPPM(const char* filename) {
    using std::endl;
    
    const auto magic_number = "P6";
    const auto bytes_per_Color = 3;

    std::ofstream out(filename);
    out << magic_number << endl;
    out << _width << " " << _height << endl;
    out << 255 << endl;
    out << (char)0 << char(0);
    for (auto h = 0U; h < _height; ++h) {
        for (auto w = 0U; w < _width; ++w) {
             auto& color = _Color_data[(_height - h - 1)*_width + w];
             out << color.R << color.G << color.B;
        }
    }
}

//
// drawing algorithms
void DrawingAlgoApp::DrawLineBresenham(Point2D p0, Point2D p1, const Color& color) {
    if (ClipLine(p0, p1) == false)
        return;

    if (_antialiase)
        return DrawLineAntialiasedWu(p0, p1, color);
    
    // Startpunkt
    int x = p0.x;
    int y = p0.y;

    int dx = p1.x - p0.x;
    int dy = p1.y - p0.y;

    // Schrittrichtung für Diagonalschritt ermitteln (sgn liefert -1, 0, 1)
    int diagonal_dx = sgn(dx);
    int diagonal_dy = sgn(dy);

    int errSR, errLR;               // Fehleranpassung in schneller Richtung (SR) und langsamer Richtung (LR)
    int parallel_dx, parallel_dy;   // Schrittweite für Parallelschritt 
    int num_elements;               // Anzahl zu zeichnender Color

    // Unterscheidung zwischen schneller und langsamer Richtung
    if (std::abs(dx) > std::abs(dy)) {
        // x ist schnelle Richtung, d.h. mache keinen Schritt in y-Richtung bei Parallelschritt
        parallel_dx = diagonal_dx;
        parallel_dy = 0;
        errSR = std::abs(dx)*2;
        errLR = std::abs(dy)*2;
        num_elements = std::abs(dx);    // Anzahl zu zeichnender Color in schneller Richtung
    } else {
        // y ist schnelle Richtung, d.h. mache keinen Schritt in x-Richtung bei Parallelschritt
        parallel_dx = 0;
        parallel_dy = diagonal_dy;
        errSR = std::abs(dy)*2;
        errLR = std::abs(dx)*2;
        num_elements = std::abs(dy);    // Anzahl zu zeichnender Color in schneller Richtung
    }

    // Fehlervariable initialisieren
    int error = errLR - num_elements;

    // HACK! set always the start pixel (the algorithm wouldn't set the pixel if start and end point are the same)
    SetColor(x, y, color);

    for (int i = 0; i < num_elements; ++i) {
        SetColor(x, y, color);
        
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

void DrawingAlgoApp::DrawLineAntialiasedWu(Point2D p0, Point2D p1, const Color& color) {
    //boolean steep := abs(y1 - y0) > abs(x1 - x0)
    auto steep = std::abs(p1.y - p0.y) > std::abs(p1.x - p0.x);

    //if steep then
    if (steep) {
    //    swap(x0, y0)
    //    swap(x1, y1)
        std::swap(p0.x, p0.y);
        std::swap(p1.x, p1.y);
    //end if
    }

    //if x0 > x1 then
    if (p0.x > p1.x) {
    //  swap(x0, x1)
    //  swap(y0, y1)
        std::swap(p0.x, p1.x);
        std::swap(p0.y, p1.y);
    //end if
    }

    //dx := x1 - x0
    //dy := y1 - y0
    //gradient := dy / dx
    auto dx = p1.x - p0.x;
    auto dy = p1.y - p0.y;
    auto gradient = dy/dx;

    //// handle first endpoint
    //xend := round(x0)
    //yend := y0 + gradient * (xend - x0)
    auto xend = p0.x;
    auto yend = p0.y + gradient * (xend - p0.x);

    //xgap := rfpart(x0 + 0.5)
    auto xgap = 1 - fract_part(p0.x + .5);

    //xpxl1 := xend   //this will be used in the main loop
    //ypxl1 := ipart(yend)
    auto xpxl1 = xend;
    auto ypxl1 = int_part(yend);

    //if steep then
    if (steep) {
    //    plot(ypxl1,   xpxl1, rfpart(yend) * xgap)
    //    plot(ypxl1+1, xpxl1,  fpart(yend) * xgap)
        auto param3 = (1 - fract_part(yend)) * xgap;
        auto col = param3 * color;
        this->SetColor(ypxl1,   xpxl1, col);

        param3 = fract_part(yend) * xgap;
        col = param3 * color;
        this->SetColor(ypxl1+1, xpxl1, col);
    }
    //else
    else {
    //    plot(xpxl1, ypxl1  , rfpart(yend) * xgap)
    //    plot(xpxl1, ypxl1+1,  fpart(yend) * xgap)
        auto param3 = (1 - fract_part(yend)) * xgap;
        auto col = param3 * color;
        this->SetColor(xpxl1, ypxl1,   col);

        param3 = fract_part(yend) * xgap;
        col = param3 * color;
        this->SetColor(xpxl1, ypxl1+1, col);
    }
    //end if

    //intery := yend + gradient // first y-intersection for the main loop
    auto intery = yend + gradient;

    //// handle second endpoint

    //xend := round(x1)
    //yend := y1 + gradient * (xend - x1)
    xend = p1.x;
    yend = p1.y + gradient * (xend - p1.x);

    //xgap := fpart(x1 + 0.5)
    xgap = 1; // fract_part(p1.x + .5);

    //xpxl2 := xend //this will be used in the main loop
    //ypxl2 := ipart(yend)
    auto xpxl2 = xend;
    auto ypxl2 = int_part(yend);

    //if steep then
    if (steep) {
    //    plot(ypxl2  , xpxl2, rfpart(yend) * xgap)
    //    plot(ypxl2+1, xpxl2,  fpart(yend) * xgap)
        auto param3 = (1 - fract_part(yend)) * xgap;
        auto col = param3 * color;
        SetColor(ypxl2,   xpxl2, col);

        param3 = fract_part(yend) * xgap;
        col = param3 * color;
        SetColor(ypxl2+1, xpxl2, col);
    }
    else {
    //else
    //    plot(xpxl2, ypxl2,  rfpart(yend) * xgap)
    //    plot(xpxl2, ypxl2+1, fpart(yend) * xgap)
        auto param3 = (1 - fract_part(yend)) * xgap;
        auto col = param3 * color;
        SetColor(xpxl2, ypxl2,   col);

        param3 = fract_part(yend) * xgap;
        col = param3 * color;
        SetColor(xpxl2, ypxl2+1, col);
    }
    //end if

    //// main loop

    //for x from xpxl1 + 1 to xpxl2 - 1 do
    for (auto x = xpxl1 + 1; x < xpxl2; ++x) {
    //     if  steep then
        if (steep) {
    //        plot(ipart(intery)  , x, rfpart(intery))
    //        plot(ipart(intery)+1, x,  fpart(intery))
            auto param3 = (1 - fract_part(intery)) * xgap;
            auto col = param3 * color;
            SetColor(int_part(intery),   x, col);

            param3 = fract_part(intery) * xgap;
            col = param3 * color;
            SetColor(int_part(intery)+1, x, col);
        }
        else {
    //    else
    //        plot(x, ipart (intery),  rfpart(intery))
    //        plot(x, ipart (intery)+1, fpart(intery))
            auto param3 = (1 - fract_part(intery)) * xgap;
            auto col = param3 * color;
            SetColor(x, int_part(intery),   col);

            param3 = fract_part(intery) * xgap;
            col = param3 * color;
            SetColor(x, int_part(intery)+1, col);
        }
    //    end if
    //    intery := intery + gradient
        intery = intery + gradient;
    }
}

void DrawingAlgoApp::DrawLineMidpoint(Point2D p0, Point2D p1, const Color& color) {
    int x = p0.x;
    int y = p0.y;
    int dx = p1.x - p0.x;
    int dy = p1.y - p0.y;
    int f = dy - dx/2;

    // immer Schritt in schnelle Richtung, gelegentlich in langsame Richtung
    for (int i = 1; i <= dx; ++i) { 
        SetColor(x, y, color);
        x++;
        if (f > 0) { 
            y += 1;
            f -= dx;
        }
        f += dy;
    }
}

void DrawingAlgoApp::DrawCircle(Point2D center, uint radius, const Color& color) {
    int x1 = 0;
    int y1 = radius;
    int f  = 1 - radius;
    int dx = 3;
    int dy = 2 - 2*radius;

    while(x1 <= y1) {
        // drawing each possible arc
        SetColor(center.x+x1, center.y-y1, color);
        SetColor(center.x+x1, center.y+y1, color);
        SetColor(center.x-x1, center.y-y1, color);
        SetColor(center.x-x1, center.y+y1, color);
        SetColor(center.x+y1, center.y-x1, color);
        SetColor(center.x+y1, center.y+x1, color);
        SetColor(center.x-y1, center.y-x1, color);
        SetColor(center.x-y1, center.y+x1, color);

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

    // render support points in red
    DrawPoints(support_points, Color(0xFF0000));

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

    // render curve
    auto p0 = *points.cbegin();
    for (auto it = points.cbegin() + 1; it != points.cend(); ++it) {
        auto& p1 = *it;

        DrawLineBresenham(p0, p1);

        p0 = p1;
    }
}

void DrawingAlgoApp::DrawBSpline(const std::vector<Point2D>& support_points) {
    const auto support_points_size = support_points.size(); // p
    const auto& polynom_degree     = _bspline_poly_degree; // n
    const auto& knot_vector        = _bspline_knot_vector;

    // m = p - 1
    const auto m = support_points_size - 1;

    // knot_vector content:
    // n + 1   0en
    // n + 1   1en
    // rest: frei wählbar

    // größe: n + m + 2

    // mit n = 4, m = 7
    // größe = 4 + 7 + 2 = 13

    // render support points in red
    DrawPoints(support_points, Color(0xFF0000));

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
            auto size = static_cast<int>(support_points_size);
            while (i < size && knot_vector[i] <= t)
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

        DrawLineBresenham(p0, p1, Color(0xFFFF00));

        p0 = p1;
    }
}

void DrawingAlgoApp::DrawBSplineClosed(const std::vector<Point2D>& support_points) {
    const auto support_points_size = support_points.size(); // p
    const auto& polynom_degree     = _bspline_poly_degree; // n
    auto& knot_vector        = _bspline_knot_vector;

    const auto& n = polynom_degree;
    const int m   = support_points_size - 1; // m

    // knotenvektorgröße: n + m + 1 + 1
    //                  = n + p + 1

    // m = 3, n = 3
    // => knotenvektorgröße = 3 + 3 + 2 = 8

    // [0, 1, 2, 3, 4, 5, 6, 7]

    // bis idx m+2 frei wählbar
    // --> ersten m+2 frei wählbar! // hier: 5
    // t_(m+2) = t_(m+1) + (t_1-t_0)
    // ...
    // t_(n+m+1) = t_(n+m) + (t_n-t_(n-1))

    // t_5 = t-4 + (t_1 - t_0)  // hier: 4 + (1 - 0) = 5
    // ...                      // hier: 5 + (2 - 1) = 6
    // t_7 = t-6 + (t_3 - t_2)  // hier: 6 + (3 - 2) = 7

    // render support points in red
    DrawPoints(support_points, Color(0xFF0000));

    if (m <= 2 || knot_vector.size() != m + 2) {
        return;
    }

    for (auto i = m+2; i <= n+m+1; ++i) {
        knot_vector.push_back(knot_vector[i-1] + (knot_vector[i-m-1] - knot_vector[i-m-2]));
    }

    std::vector<std::vector<Point2D>> d(m + 1, std::vector<Point2D>(n + 1));
    std::vector<Point2D> points;
    
    // m = Anzahl Kontrollpunkte - 1
    // n = Polynomgrad

    auto dt = _delta_t;

    for (float t = .0f; t <= 1.f + dt/2.f; t += dt) {
        // Berechne i so, dass t_i ≤ t < t_i+1, 0 ≤ i ≤ m
        const int i = [&](float t) {
            int i = 0;
            auto size = static_cast<int>(support_points_size);
            while (i < size && knot_vector[i] <= t)
                ++i;
            return --i;
        }(t);

        auto tt = t;

        // FOR j := 0 TO n DO
        for (int j = 0; j <= n; ++j) {
            // l ≔ i-n+j-1
            auto l = i - n + j - 1;
            
            // REPEAT UNTIL l == i
            do {
                // l ∶= l+1
                ++l;

                // IF l < 0 THEN
                if (l < 0) {
                    // l ∶= l+m+1
                    l = l + m + 1;
                    // t^* ≔ t^*-t_0+t_(m+1)
                    tt = tt - knot_vector[0] + knot_vector[m+1];
                }
                else {
                    // IF l ≥ m+1 THEN
                    if (l >= m+1) {
                        // l ≔ l-m-1
                        l = l - m - 1;
                        // t^* ≔ t^*-t_(m+1)+t_0
                        tt = tt - knot_vector[m+1] + knot_vector[0];
                    }
                }

        //      IF j == 0 THEN
                if (j == 0) {
        //          d_l^0 ≔ d_l
                    d[l][0] = support_points[l];
                }
        //      ELSE 
                else {
        //          t_l^j ≔ (t^*-t_l)/(t_(l+n+1-j)-t_l)
                    auto _t = (tt - knot_vector[l]) / (knot_vector[l+n+1-j] - knot_vector[l]);
        //          d_l^j ≔ (1-t_l^j )⋅d_((l-1)  mod (m+1))^(j-1)+t_l^j⋅d_l^(j-1)

                    auto idx = (l-1)%(m+1);
                    if (idx < 0)
                        idx += m+1;
                    auto& d1 = d[idx][j-1];
                    auto& d2 = d[l][j-1];

                    d[l][j] = (1 - _t) * d1 + _t * d2;
                }
            } while (l != i);
        }
        // b(t^* )≔d_i^n
        points.push_back(d[i][n]);
    }

    // render curve
    auto p0 = *points.cbegin();
    for (auto it = points.cbegin() + 1; it != points.cend(); ++it) {
        auto& p1 = *it;

        DrawLineBresenham(p0, p1, Color(0xFFFF00));

        p0 = p1;
    }
}

void DrawingAlgoApp::DrawCatmulRomSpline(const std::vector<Point2D>& input_control_points) { 
    auto num_control_points = input_control_points.size();

    std::vector<Point2D> control_points;

    // render control points in red
    DrawPoints(input_control_points, Color(0xFF0000));

    if (num_control_points >= 2) {
        // render the curve!

        // add one additional point at the beginning of the control_points
        auto p_begin = input_control_points[0] - input_control_points[1];
        p_begin = input_control_points[0] + p_begin;

        // add one additional point at the end of the control_points
        auto p_end = *(input_control_points.end()-1) - *(input_control_points.end()-2);
        p_end = *(input_control_points.end()-1) + p_end;

        control_points.push_back(p_begin);
        control_points.insert(control_points.end(), input_control_points.begin(), input_control_points.end());
        control_points.push_back(p_end);
        num_control_points += 2;

        std::vector<Point2D> points;
        for (auto i = 0U; i < num_control_points - 3; ++i) {
            for (float t = .0f; t <= 1.f + _delta_t/2.f; t += _delta_t) {
                float t2 = t * t;
                float t3 = t2 * t;

                auto p0 = control_points[i];
                auto p1 = control_points[i+1];
                auto p2 = control_points[i+2];
                auto p3 = control_points[i+3];
            
                auto out = 0.5f * ( ( 2.0f * p1 ) +
                            ( -p0 + p2 ) * t +
                            ( 2.0f * p0 - 5.0f * p1 + 4 * p2 - p3 ) * t2 +
                            ( -p0 + 3.0f * p1 - 3.0f * p2 + p3 ) * t3 );

                points.emplace_back(out);
            }
        }

        // render curve
        auto p0 = *points.cbegin();
        for (auto it = points.cbegin() + 1; it != points.cend(); ++it) {
            auto& p1 = *it;

            DrawLineBresenham(p0, p1, Color(0xFFFF00));

            p0 = p1;
        }
    }
}

void DrawingAlgoApp::FillRectangle(const Point2D p0, const Point2D p1) {
    // get bounding box
    auto xmin = std::min(p0.x, p1.x);
    auto xmax = std::max(p0.x, p1.x);
    auto ymin = std::min(p0.y, p1.y);
    auto ymax = std::max(p0.y, p1.y);

    float xlen = static_cast<float>(xmax - xmin);
    float ylen = static_cast<float>(ymax - ymin);

    auto p0_color = Color(0xFF0000);
    auto p1_color = Color(0x00FF00);
    auto p2_color = Color(0x0000FF);
    auto p3_color = Color(0xFFFFFF);

    // fill rectangle
    for (auto y = ymin; y <= ymax; ++y) {
        for (auto x = xmin; x <= xmax; ++x) {
            // bilinear interpolate colors
            auto r1 = (xmax-x)/xlen * p0_color + (x-xmin)/xlen * p1_color;
            auto r2 = (xmax-x)/xlen * p2_color + (x-xmin)/xlen * p3_color;
            auto p = (ymax-y)/ylen * r1 + (y-ymin)/ylen * r2;
            SetColor(x, y, p);
        }
    }
}

void DrawingAlgoApp::FillTriangle(std::vector<Point2D> vertices) {
    DrawPoints(vertices, Color(0xFFFF00));

    if (vertices.size() >= 3) {
        for (auto i = 0U; i < vertices.size() - vertices.size() % 3; i += 3) {
            auto& v0 = vertices[i];
            auto& v1 = vertices[i+1];
            auto& v2 = vertices[i+2];

            using std::max;
            using std::min;

            // CCW - counter clockwise
            auto x0 = static_cast<int>(v0.x);
            auto x1 = static_cast<int>(v2.x);
            auto x2 = static_cast<int>(v1.x);
            auto y0 = static_cast<int>(v0.y);
            auto y1 = static_cast<int>(v2.y);
            auto y2 = static_cast<int>(v1.y);

            auto p0_color = Color(0xFF0000);
            auto p1_color = Color(0x00FF00);
            auto p2_color = Color(0x0000FF);

            // get bounding box
            int xmin = min(x0, min(x1, x2));
            int ymin = min(y0, min(y1, y2));
            int xmax = max(x0, max(x1, x2));
            int ymax = max(y0, max(y1 ,y2));
        
            int f0 = (y0 - y1)*(xmin - x0) + (x1 - x0)*(ymin - y0);
            int f1 = (y1 - y2)*(xmin - x1) + (x2 - x1)*(ymin - y1);
            int f2 = (y2 - y0)*(xmin - x2) + (x0 - x2)*(ymin - y2);

            // fill triangle
            for (int y = ymin; y <= ymax; y++) {
                int ff0 = f0;
                int ff1 = f1;
                int ff2 = f2;
                float c = static_cast<float>(f0 + f1 + f2);
            
                for (int x = xmin; x <= xmax; x++) {
                    if(ff0 >= 0 && ff1 >=0 && ff2 >= 0) {
                        // interpolate color
                        auto Color = ff0/c * p0_color + ff1/c * p1_color + ff2/c * p2_color;
                        SetColor(x, y, Color);
                    }
                
                    ff0 = ff0 + (y0-y1);
                    ff1 = ff1 + (y1-y2);
                    ff2 = ff2 + (y2-y0);
                }
            
                f0 = f0 + (x1-x0);
                f1 = f1 + (x2-x1);
                f2 = f2 + (x0-x2);
            }
        }
    }
}

void DrawingAlgoApp::FillPolygon(std::vector<Point2D> vertices, const Color& color) {
    vertices = ClipPoly(vertices);
    
    DrawPoints(vertices, Color(0, 255, 255));

    if (vertices.size() >= 3) {
        // passive edge struct
        struct PassiveEdge {
            Point2D p0, p1;
            int x0, y0;
            int x1, y1;

            PassiveEdge(Point2D first, Point2D second) {
                // edges always rise in y (positive slope?)

                x0 = static_cast<int>(first.x);
                y0 = static_cast<int>(first.y);
                p0 = first;

                x1 = static_cast<int>(second.x);
                y1 = static_cast<int>(second.y);
                p1 = second;

                if (!Point2D::less_y(first, second)) {
                    std::swap(x0, x1);
                    std::swap(y0, y1);
                    std::swap(p0, p1);
                }
            }
        };
        struct less_passive_edge {
            bool operator() (const PassiveEdge& first, const PassiveEdge& second) {
                return first.y0 < second.y0;
            }
        };
        
        // active edge struct
        struct ActiveEdge {
            float xs;
            float dx;
            int ymax;

            ActiveEdge(float xs, float dx, int ymax)
                : xs(xs), dx(dx), ymax(ymax)
            {}
        };
        struct less_active_edge {
            bool operator() (const ActiveEdge& first, const ActiveEdge& second) {
                return first.xs < second.xs;
            }
        };

        // adding all edges to passives set
        std::multiset<PassiveEdge, less_passive_edge> passives;
        for (auto first_it = vertices.cbegin(), second_it = vertices.cbegin()+1; first_it != vertices.cend(); ++first_it, ++second_it) {
            // last vertex is the first one to close the polygon (last edge should close the polygon)
            if (second_it == vertices.cend())
                second_it = vertices.cbegin();

            passives.emplace(*first_it, *second_it);
        }

        auto edges = passives;

        // lesser compare for Point2Ds y-values
        auto miny = static_cast<int>(std::min_element(vertices.cbegin(), vertices.cend(), Point2D::less_y)->y);
        auto maxy = static_cast<int>(std::max_element(vertices.cbegin(), vertices.cend(), Point2D::less_y)->y);

        std::vector<ActiveEdge> actives;
        for (auto y = miny; y <= maxy; ++y) {
            // push all passive edges with ymin == y into actives set
            for (auto iter = passives.begin(); iter != passives.end(); ) {
                const auto& passive_edge = *iter;

                if (passive_edge.y0 == y) {
                    // initialise ActiveEdge::xs == xmin
                    auto xs    = static_cast<float>(passive_edge.x0);
                    auto ymax  = std::max(passive_edge.y0, passive_edge.y1);
                    auto dx    = (passive_edge.x1 - passive_edge.x0) / (float)(passive_edge.y1 - passive_edge.y0);

                    actives.emplace_back(xs, dx, ymax);
                    passives.erase(iter++); // pass old iterator to erase but jump one further before!
                }
                else {
                    ++iter;
                }
            }
                
            //std::remove_if(actives.begin(), actives.end(), [&y](ActiveEdge e1) {
            //    return e1.ymax == y ? true : false;
            //});

            // erase all edges with ymax == y from actives
            for (auto iter = actives.begin(); iter != actives.end(); ) {
                const auto& active_edge = *iter;

                if (active_edge.ymax == y)
                    iter = actives.erase(iter); // pass old iterator to erase but jump one further before!
                else
                    ++iter;
            }

            // set Color within the spans of actives xs
            assert(actives.size() % 2 == 0);
            for (auto first_it = actives.cbegin(); first_it != actives.cend(); ++++first_it) {
                auto second_it = first_it + 1;

                auto& intersect1 = first_it->xs;
                auto& intersect2 = second_it->xs;

                for (auto x = intersect1; x < intersect2; ++x)
                    SetColor(x, y, Color(0, 100, 100));
            }

            // increment all xs with dx (of active edges)
            for (auto& edge : actives)
                edge.xs += edge.dx;

            std::sort(actives.begin(), actives.end(), less_active_edge());
        }

        // draw all edges
        for (auto& edge : edges)
            DrawLineBresenham(edge.p0, edge.p1, Color(0, 255, 255));
    }
}

void DrawingAlgoApp::DrawPythagoras() {
    using glm::mat3;
    
    vector<Point2D> quad;
    quad.emplace_back(0, 0);
    quad.emplace_back(1, 0);
    quad.emplace_back(1, 1);
    quad.emplace_back(0, 1);

    vector<Point2D> tri;
    tri.emplace_back(0, 0);
    tri.emplace_back(0, 3);
    tri.emplace_back(4, 3);

    mat3 local_transform = translate(Point2D(300, 200)) * scale(50, 50);
    ApplyMatrix(_transform_mat * local_transform, tri);

    // draw triangle
    FillTriangle(tri);

    // draw quads
    vector<mat3> quad_matrices;
    quad_matrices.push_back(translate(Point2D(-3, 0)) * scale(3, 3)); // small
    quad_matrices.push_back(translate(Point2D(0, 3))  * scale(4, 4)); // medium
    quad_matrices.push_back(rotate(-glm::degrees(std::asin(4.f/5.f))) * scale(5, 5)); // large

    for (auto& mat : quad_matrices) {
        auto vtx_cpy = quad;
        ApplyMatrix(_transform_mat * local_transform * mat, vtx_cpy);

        for (auto it0 = vtx_cpy.begin(), it1 = it0 + 1; it0 != vtx_cpy.end(); ++it0, ++it1) {
            if (it1 == vtx_cpy.end())
                it1 = vtx_cpy.begin();

            DrawLineBresenham(*it0, *it1, Color(0xFFFFFF));
        }
    }
}

void DrawingAlgoApp::RenderBouncingRect() {
    auto& x = _bouncing_rect.x;
    auto& y = _bouncing_rect.y;
    auto& xstep = _bouncing_rect.xstep;
    auto& ystep = _bouncing_rect.ystep;
    auto& scale = _bouncing_rect.scale;
    auto& scale_step = _bouncing_rect.scale_step;

    // translating
    if (x > 1 - .25 || x < -1 + .25)
        xstep = -xstep;
    if (y > 1 - .25 || y < -1 + .25)
        ystep = -ystep;

    x += xstep;
    y += ystep;

    // scaling
    if (scale < .2f || scale > 1.4f)
        scale_step = -scale_step;

    scale += scale_step;

    // rotating
    _bouncing_rect.rotate_angle += _bouncing_rect.rotate_step;

    // drawing
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0f, 0.0f, 0.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(x, y, 0);

    glScalef(scale, scale, 1);

    glRotatef(_bouncing_rect.rotate_angle, 0, 0, 1);

    glRectf(-0.25f, -0.25f, 0.25f, 0.25f);

    glFlush();
}

int DrawingAlgoApp::ClippingOutcodeFor(Point2D p) const {
    const auto epsi = 0.001;

    int c = 0;
    if (p.x < _clipping_rect.xmin - epsi) c |= 8; // p is left of clipping rect
    if (p.x > _clipping_rect.xmax + epsi) c |= 4; // p is right of clipping rect
    if (p.y < _clipping_rect.ymin - epsi) c |= 2; // p is above clipping rect
    if (p.y > _clipping_rect.ymax + epsi) c |= 1; // p is beneath clipping rect
    return c;
}

// returns true  if the line should be painted
// returns false if the line is completely outside of the clipping rectangle
bool DrawingAlgoApp::ClipLine(Point2D &start, Point2D &end) const {
    while (true) {
        auto startcode = ClippingOutcodeFor(start);
        auto endcode   = ClippingOutcodeFor(end);

        if ((startcode == 0) && (endcode == 0)) {
            // line completely in rectangle: no clipping
            return true;
        }
        else if (startcode & endcode) {
            // line completely outside clipping rect: no painting
            return false;
        }
        else {
            // clip line
            auto& clip_code = startcode ? startcode : endcode;
            auto& clip_point = startcode ? start : end;

            if (clip_code & static_cast<int>(Side::Left))
                clip_point = ClipLine(start, end, Side::Left);
            else if (clip_code & static_cast<int>(Side::Right))
                clip_point = ClipLine(start, end, Side::Right);
            else if (clip_code & static_cast<int>(Side::Top))
                clip_point = ClipLine(start, end, Side::Top);
            else if (clip_code & static_cast<int>(Side::Bottom))
                clip_point = ClipLine(start, end, Side::Bottom);
            else
                assert(!"clipping a line that is completely inside clipping rect?");
        }
    }
    return true;
}

Point2D DrawingAlgoApp::ClipLine(const Point2D &start, const Point2D &end, Side side) const {
    auto& xmin = _clipping_rect.xmin;
    auto& xmax = _clipping_rect.xmax;
    auto& ymin = _clipping_rect.ymin;
    auto& ymax = _clipping_rect.ymax;

    // points for the line to intersect
    Point2D p0, p1;

    switch (side) {
    case Side::Left:
        // left line
        p0 = Point2D(xmin, ymin);
        p1 = Point2D(xmin, ymax);
        break;
    case Side::Right:
        // right line
        p0 = Point2D(xmax, ymin);
        p1 = Point2D(xmax, ymax);
        break;
    case Side::Top:
        // upper line
        p0 = Point2D(xmin, ymin);
        p1 = Point2D(xmax, ymin);
        break;
    case Side::Bottom:
        // lower line
        p0 = Point2D(xmin, ymax);
        p1 = Point2D(xmax, ymax);
        break;
    }

    return GetLineIntersection(start, end, p0, p1);
}

std::vector<Point2D> DrawingAlgoApp::ClipPoly(std::vector<Point2D> vertices) const {
    std::vector<Side> sides;
    sides.push_back(Side::Bottom);
    sides.push_back(Side::Right);
    sides.push_back(Side::Top);
    sides.push_back(Side::Left);

    // clip for all sides
    for (const auto& side : sides) {
        int side_code = static_cast<int>(side);
        std::vector<Point2D> out_verts;

        if (vertices.size() < 2)
            return out_verts;

        assert(vertices.size() > 1);
        for (auto it0 = begin(vertices), it1 = begin(vertices) + 1; it0 != end(vertices); ++it0, ++it1) {
            if (it1 == end(vertices))
                it1 = begin(vertices);

            auto start = *it0;
            auto end   = *it1;

            auto outcode0 = ClippingOutcodeFor(start);
            auto outcode1 = ClippingOutcodeFor(end);

            // is a point outside the current clipping side?
            if ((outcode0 & side_code) 
                || (outcode1 & side_code)) {

                // both outside? discard this edge
                if ((outcode0 & side_code) == (outcode1 & side_code))
                    continue;

                auto intersection = ClipLine(start, end, side);
                out_verts.push_back(intersection);

                // going from outside in
                if ((outcode0 & side_code) != 0
                    && (outcode1 & side_code) == 0) {
                    out_verts.push_back(end);
                }
            }
            else {
                out_verts.push_back(end);
            }
        }

        vertices = out_verts;
    }

    return vertices;
}

//
// event handler
void DrawingAlgoApp::OnKeyPressed(sf::Keyboard::Key key, bool ctrl, bool alt, bool shift, bool system) {
    typedef sf::Keyboard::Key Key;
    
    int d_translate = 4;
    float d_scale = .05f;
    float d_rotate = 2.f;
    float d_shear = .1f;

    // translate to origin
    _transform_mat = translate(-_transform_origin) * _transform_mat;

    switch (key) {
    // Transformations control
    case Key::Left:
        switch (_transform_type) {
        case TransformationType::Translate:
            _transform_mat = translate(glm::vec2(-d_translate, 0)) * _transform_mat;
            break;
        case TransformationType::Scale:
            _transform_mat = scale(1.f - d_scale, 1.f) * _transform_mat;
            break;
        case TransformationType::Rotate:
            _transform_mat = rotate(d_rotate) * _transform_mat;
            break;
        case TransformationType::Shear:
            _transform_mat = shear(d_shear, 0) * _transform_mat;
            break;
        }
        break;
    case Key::Right:
        switch (_transform_type) {
        case TransformationType::Translate:
            _transform_mat = translate(glm::vec2(d_translate, 0)) * _transform_mat;
            break;
        case TransformationType::Scale:
            _transform_mat = scale(1.f + d_scale, 1.f) * _transform_mat;
            break;
        case TransformationType::Rotate:
            _transform_mat = rotate(-d_rotate) * _transform_mat;
            break;
        case TransformationType::Shear:
            _transform_mat = shear(-d_shear, 0) * _transform_mat;
            break;
        }
        break;
    case Key::Up:
        switch (_transform_type) {
        case TransformationType::Translate:
            _transform_mat = translate(glm::vec2(0, -d_translate)) * _transform_mat;
            break;
        case TransformationType::Scale:
            _transform_mat = scale(1.f, 1.f + d_scale) * _transform_mat;
            break;
        case TransformationType::Rotate:
            break;
        case TransformationType::Shear:
            _transform_mat = shear(0, -d_shear) * _transform_mat;
            break;
        }
        break;
    case Key::Down:
        switch (_transform_type) {
        case TransformationType::Translate:
            _transform_mat = translate(glm::vec2(0, d_translate)) * _transform_mat;
            break;
        case TransformationType::Scale:
            _transform_mat = scale(1.f, 1.f - d_scale) * _transform_mat;
            break;
        case TransformationType::Rotate:
            break;
        case TransformationType::Shear:
            _transform_mat = shear(0, d_shear) * _transform_mat;
            break;
        }
        break;
    }

    // translate back
    _transform_mat = translate(_transform_origin) * _transform_mat;

    DrawCurrentMode();
}

void DrawingAlgoApp::OnKeyReleased(sf::Keyboard::Key key, bool ctrl, bool alt, bool shift, bool system) {
    typedef sf::Keyboard::Key Key;
    
    Super::OnKeyReleased(key, ctrl, alt, shift, system);

    switch (key) {
    case Key::C:
        ClearColorData();
        
        // clear vertices and transform matrix
        _transform_mat = glm::mat3(1.f);
        _vertices.clear();
        break;
    case Key::X:
        SaveAsPPM();
        break;
    case Key::V:
        _show_current_mode = !_show_current_mode;
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
        updateConfigData();
        break;
    case Key::Num6:
        _draw_type = DrawingType::BSplineClosed;
        updateConfigData();
        break;
    case Key::Num7:
        _draw_type = DrawingType::CatmulRomSpline;
        break;
    case Key::Num8:
        _draw_type = DrawingType::FillRectangle;
        break;
    case Key::Num9:
        _draw_type = DrawingType::FillTriangle;
        break;
    case Key::Num0:
        _draw_type = DrawingType::FillPolygon;
        break;
    case Key::O:
        _draw_type = DrawingType::SetClippingRect;
        break;
    case Key::P:
        _draw_type = DrawingType::Pythagoras;
        break;
    case Key::B:
        _draw_type = DrawingType::BouncingRect;
        break;
    case Key::F1:
        _antialiase = !_antialiase;
        break;
    // Transformations selection
    case Key::Q:
        _transform_type = TransformationType::None;
        break;
    case Key::W:
        _transform_type = TransformationType::Translate;
        break;
    case Key::E:
        _transform_type = TransformationType::Scale;
        break;
    case Key::R:
        _transform_type = TransformationType::Rotate;
        break;
    case Key::T:
        _transform_type = TransformationType::Shear;
        break;
    case Key::Z:
        _transform_type = TransformationType::SetOrigin;
        break;
    }

    DrawCurrentMode();
}

void DrawingAlgoApp::OnMouseButtonPressed(sf::Mouse::Button button, int x, int y) {
    
    // add button down location to vertices
    switch (_draw_type) {
    case DrawingType::Line:
    case DrawingType::Circle:
    case DrawingType::FillRectangle:
        _vertices.emplace_back(static_cast<float>(x), static_cast<float>(y));
        break;
    }
    
    _mouse_pos_cache.x = static_cast<float>(x);
    _mouse_pos_cache.y = static_cast<float>(y);
}

void DrawingAlgoApp::OnMouseButtonReleased(sf::Mouse::Button button, int x, int y) {
    
    if (_transform_type == TransformationType::SetOrigin) {
        _transform_origin = Point2D(x, y);
        _transform_type = TransformationType::None;
    }
    else {
        _vertices.emplace_back(x, y);

        switch (_draw_type) {
         // update config for B-spline mode!
        case DrawingType::BSpline:
        case DrawingType::BSplineClosed:
            updateConfigData();
            break;
        case DrawingType::SetClippingRect:
            // update clipping rect
            Point2D corner1 = _mouse_pos_cache; // 200|101 -> 300|201
            Point2D corner2 = _vertices.back();
            _vertices.pop_back();

            _clipping_rect.xmin = corner1.x < corner2.x ? corner1.x : corner2.x;
            _clipping_rect.xmax = corner1.x < corner2.x ? corner2.x : corner1.x;
            _clipping_rect.ymin = corner1.y < corner2.y ? corner1.y : corner2.y;
            _clipping_rect.ymax = corner1.y < corner2.y ? corner2.y : corner1.y;

            break;
        }
    }

    DrawCurrentMode();
}

void DrawingAlgoApp::DrawCurrentMode() {
    // clear pixel buffer
    ClearColorData();
    
    // draw transformation origin
    DrawCircle(_transform_origin, 1, Color(0xFFFFFF));

    // apply transformation to all vertices
    auto _transformed_vertices = _vertices;
    ApplyMatrix(_transform_mat, _transformed_vertices);    
    auto& vertices = _transformed_vertices;

    switch (_draw_type) {
    case DrawingType::None:
    case DrawingType::BouncingRect: // see OnRender()
        break;
    case DrawingType::Line:
        // draw every vertex pair
        for (auto i = 0U; i < vertices.size(); i += 2) {
            if (i+1 < vertices.size())
                DrawLineBresenham(vertices[i], vertices[i+1]);
        }
        break;
    case DrawingType::Circle:
        {
            // draw every vertex pair
            for (auto i = 0U; i < vertices.size(); i += 2) {
                if (i+1 < vertices.size()) {
                    // calc distance between cache and current point
                    auto dx = vertices[i].x - vertices[i+1].x;
                    auto dy = vertices[i].y - vertices[i+1].y;
                    auto radius = std::sqrt(dx*dx + dy*dy);

                    DrawCircle(vertices[i], static_cast<int>(radius));
                }
            }
        }
        break;
    case DrawingType::Bezier:
        // render bezier
        DrawBezier(vertices);
        break;
    case DrawingType::BSpline:
        // render bspline
        DrawBSpline(vertices);
        break;
    case DrawingType::BSplineClosed:
        // render bezier
        DrawBSplineClosed(vertices);
        break;
    case DrawingType::CatmulRomSpline:
        // render catmul rom spline
        DrawCatmulRomSpline(vertices);
        break;
    case DrawingType::FillRectangle:
        // draw every vertex pair
        for (auto i = 0U; i < vertices.size(); i += 2) {
            if (i+1 < vertices.size())
                FillRectangle(vertices[i], vertices[i+1]);
        }
        
        break;
    case DrawingType::FillTriangle:
        FillTriangle(vertices);
        break;
    case DrawingType::FillPolygon:
        FillPolygon(vertices, Color(0x00FFFF));
        break;
    case DrawingType::Pythagoras:
        DrawPythagoras();
        break;
    case DrawingType::SetClippingRect:
        break;
    default:
        assert(false);
        break;
    }

    // always draw clipping rect
    auto& xmin = _clipping_rect.xmin;
    auto& xmax = _clipping_rect.xmax;
    auto& ymin = _clipping_rect.ymin;
    auto& ymax = _clipping_rect.ymax;

    auto lu = Point2D(xmin, ymin);
    auto ru = Point2D(xmax, ymin);
    auto rl = Point2D(xmax, ymax);
    auto ll = Point2D(xmin, ymax);

    DrawLineBresenham(lu, ru, Color(0x888888));
    DrawLineBresenham(ru, rl, Color(0x888888));
    DrawLineBresenham(rl, ll, Color(0x888888));
    DrawLineBresenham(ll, lu, Color(0x888888));
}

void DrawingAlgoApp::ApplyMatrix(glm::mat3 mat, std::vector<Point2D> &vertices) const {
    for (auto& vtx : vertices)
        vtx = mat * vtx;
}