#include <SFMLApp.h>

#include <memory>

#include <Point2D.h>
#include <Config/ConfigFile.h>


struct Color {
    BYTE R;
    BYTE G;
    BYTE B;

    Color();
    Color(uint R, uint G, uint B);
    Color(uint hex); // 0xRRGGBB

    Color operator *(float b);
    Color operator +(Color other);
};
Color operator *(float a, Color b);

class DrawingAlgoApp : public SFMLApp {
    typedef SFMLApp Super;
    typedef unsigned char BYTE;

    //
    // additional type declarations
    enum class DrawingType {
        None,
        Line,
        Circle,
        Bezier,
        BSpline,
        FillRectangle,
        FillTriangle,
        FillPolygon
    };

    // DrawingAlgoApp declaration
public:
    DrawingAlgoApp();
    ~DrawingAlgoApp();

    //
    // render loop events
protected:
    bool OnInit() override;
    void OnRender() override;

    //
    // event handler
protected:
    virtual void OnKeyReleased(sf::Keyboard::Key key, bool ctrl, bool alt, bool shift, bool system) override;
    virtual void OnMouseButtonPressed(sf::Mouse::Button button, int x, int y) override;
    virtual void OnMouseButtonReleased(sf::Mouse::Button button, int x, int y) override;

    //
    // Config loader helper
private:
    void updateConfigData();

    //
    // drawing helper
private:
    void RenderColorArray();
    void SetColor(uint x, uint y, const Color &color);
    void SetColor(const Point2D pixel, const Color &color);
    void ClearColorData();
    void DrawPoints(const std::vector<Point2D>& points, const Color& color = Color(0, 255, 0));

    void SaveAsPPM(const char* filename = "Screen_capture.ppm");

    //
    // drawing algorithms
private:
    void DrawLineBresenham(const Point2D p0, const Point2D p1, const Color& color = Color(0, 255, 0));
    void DrawLineMidpoint (const Point2D p0, const Point2D p1, const Color& color = Color(0, 255, 0));
    void DrawCircle(const Point2D center, uint radius, const Color& color = Color(0, 255, 0));
    void DrawBezier(const std::vector<Point2D>& support_points);
    void DrawBSpline(const std::vector<Point2D>& support_points, const std::vector<float> _knot_vector);

    void FillRectangle(const Point2D p0, const Point2D p1);
    void FillTriangle(std::vector<Point2D>& vertices);
    void FillPolygon(const std::vector<Point2D>& vertices, const Color& color = Color(0, 255, 0));

    //
    // data
private:
    std::vector<Point2D> _vertices;

    std::vector<Point2D> _bezier_points;

    std::vector<Point2D> _bspline_points;
    std::vector<float>   _bspline_knot_vector;
    int                  _bspline_poly_degree;

    sf::Vector2f _mouse_pos_cache;
    DrawingType  _draw_type;
    const uint   _num_Colors;
    std::unique_ptr<Color[]> _Color_data; // _width * _height Colors

private:
    ConfigFile _config;
    float      _delta_t;
};