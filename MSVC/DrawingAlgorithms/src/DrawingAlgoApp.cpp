#include <DrawingAlgoApp.h>

#include <cassert>
#include <algorithm>
#include <set>
#include <fstream>
#include <sstream>
#include <regex>
#include <string>
using std::string;
using std::ostringstream;

#include <glm/gtx/transform.hpp>
#include <SFML/OpenGL.hpp>
#include <2d_transforms.h>

#include <boost/heap/fibonacci_heap.hpp>

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
    : Super(800u, 600u, "Drawing Algorithms", sf::Style::Close),
    _num_Colors(_width*_height),
    _draw_type(DrawingType::None),
    _mouse_pos_cache(0, 0),
    _config("DrawingAlgoApp.cfg"),
    _transform_vec(1.0f),
    _transform_type(TransformationType::None)
{
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
    //_transform_vec = translate(vec2(10, -5));
    //_transform_vec = scale(1.5f, 2) * _transform_vec;
    //_transform_vec = rotate(glm::radians(90.0f)) * _transform_vec;

    Point2D p(10, 2);
    auto p1 = _transform_vec * p;
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
C = Clear Colorbuffer
X = Capture Screen

Modes:
1 = None
2 = Line
3 = Circle
4 = Bezier
5 = B-Spline
6 = Catmul-Rom-Spline
7 = Fill Rectangle
8 = Fill Triangle
9 = Fill Polygon

Transformations:
(control with Arrow Keys)
Q = None
W = Translate
E = Scale
R = Rotate
T = Shear
)";

    auto help = _help_text.getString();
    help.insert(help.getSize(), append_text);
    _help_text.setString(help);

    // Set the screen color for clearing
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // rgba

    return true;
}

void DrawingAlgoApp::OnRender() {
    // render the Color buffer 
    RenderColorArray();

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

        if (_bspline_poly_degree != poly_deg_tmp || _vertices.size() != num_supp_points_tmp) {
            // TODO: recalc knot vector and print out to config file
            auto new_knot_vec_len = _vertices.size() + poly_deg_tmp + 1;
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
void DrawingAlgoApp::DrawLineBresenham(const Point2D p0, const Point2D p1, const Color& color) {
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

void DrawingAlgoApp::DrawLineMidpoint(const Point2D p0, const Point2D p1, const Color& color) {
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
            f -= dx; }
        f += dy;
    }
}

void DrawingAlgoApp::DrawCircle(const Point2D center, uint radius, const Color& color) {
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

void DrawingAlgoApp::DrawBSpline(const std::vector<Point2D>& support_points, const std::vector<float> knot_vector) {
    const auto support_points_size = support_points.size(); // p
    const auto& polynom_degree = _bspline_poly_degree; // n

    // knot_vector content:
    // n + 1   0en
    // n + 1   1en
    // rest: frei wählbar

    // größe: n + p + 1

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

void DrawingAlgoApp::FillTriangle(std::vector<Point2D>& vertices) {
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

void DrawingAlgoApp::FillPolygon(const std::vector<Point2D>& vertices, const Color& color) {
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

//
// event handler
void DrawingAlgoApp::OnKeyPressed(sf::Keyboard::Key key, bool ctrl, bool alt, bool shift, bool system) {
    typedef sf::Keyboard::Key Key;
    
    switch (key) {
    // Transformations control
    case Key::Left:
        switch (_transform_type) {
        case TransformationType::Translate:
            _transform_vec = translate(glm::vec2(-1, 0)) * _transform_vec;
            break;
        case TransformationType::Scale:
            break;
        case TransformationType::Rotate:
            break;
        case TransformationType::Shear:
            break;
        }
        break;
    case Key::Right:
        switch (_transform_type) {
        case TransformationType::Translate:
            _transform_vec = translate(glm::vec2(1, 0)) * _transform_vec;
            break;
        case TransformationType::Scale:
            break;
        case TransformationType::Rotate:
            break;
        case TransformationType::Shear:
            break;
        }
        break;
    case Key::Up:
        switch (_transform_type) {
        case TransformationType::Translate:
            _transform_vec = translate(glm::vec2(0, -1)) * _transform_vec;
            break;
        case TransformationType::Scale:
            break;
        case TransformationType::Rotate:
            break;
        case TransformationType::Shear:
            break;
        }
        break;
    case Key::Down:
        switch (_transform_type) {
        case TransformationType::Translate:
            _transform_vec = translate(glm::vec2(0, 1)) * _transform_vec;
            break;
        case TransformationType::Scale:
            break;
        case TransformationType::Rotate:
            break;
        case TransformationType::Shear:
            break;
        }
        break;
    }

    DrawCurrentMode();
}

void DrawingAlgoApp::OnKeyReleased(sf::Keyboard::Key key, bool ctrl, bool alt, bool shift, bool system) {
    typedef sf::Keyboard::Key Key;
    
    Super::OnKeyReleased(key, ctrl, alt, shift, system);

    switch (key) {
    case Key::C:
        ClearColorData();
        
        // clear vertices
        _vertices.clear();
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
        updateConfigData();
        break;
    case Key::Num6:
        _draw_type = DrawingType::CatmulRomSpline;
        break;
    case Key::Num7:
        _draw_type = DrawingType::FillRectangle;
        break;
    case Key::Num8:
        _draw_type = DrawingType::FillTriangle;
        break;
    case Key::Num9:
        _draw_type = DrawingType::FillPolygon;
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
    _vertices.emplace_back(x, y);

    // update config for B-spline mode!
    switch (_draw_type) {
     case DrawingType::BSpline:
        updateConfigData();
        break;
    }

    DrawCurrentMode();
}

void DrawingAlgoApp::DrawCurrentMode() {
    // apply transformation to all vertices
    auto _transformed_vertices = _vertices;
    for (auto& vtx : _transformed_vertices)
        vtx = _transform_vec * vtx;
    
    auto& vertices = _transformed_vertices;

    ClearColorData();
    switch (_draw_type) {
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
        // render bezier
        DrawBSpline(vertices, _bspline_knot_vector);
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
    }
}