#include <SFMLApp.h>

#include <memory>

#include <Point2D.h>
#include <Config/ConfigFile.h>


struct Pixel {
    BYTE R;
    BYTE G;
    BYTE B;

    Pixel();
    Pixel(uint R, uint G, uint B);
    Pixel(uint hex); // 0xRRGGBB
    Pixel operator *(float b);
    Pixel operator +(Pixel other);
};
Pixel operator *(float a, Pixel b);

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
    void RenderPixelArray();
    void SetPixel(uint x, uint y, const Pixel &pix);
    void ClearPixelData();
    void DrawPoints(const std::vector<Point2D>& points, const Pixel& pix = Pixel(0, 255, 0));

    void SaveAsPPM(const char* filename = "Screen_capture.ppm");

    //
    // drawing algorithms
private:
    void DrawLineBresenham(uint x1, uint y1, uint x2, uint y2, const Pixel& pix = Pixel(0, 255, 0));
    void DrawLineMidpoint (uint x1, uint y1, uint x2, uint y2, const Pixel& pix = Pixel(0, 255, 0));
    void DrawCircle(uint posx, uint posy, uint radius, const Pixel& pix = Pixel(0, 255, 0));
    void DrawBezier(const std::vector<Point2D>& support_points);
    void DrawBSpline(const std::vector<Point2D>& support_points, const std::vector<float> _knot_vector);

    void FillRectangle(uint x1, uint y1, uint x2, uint y2);
    void FillTriangle(std::vector<Point2D>& vertices);
    void FillPolygon(const std::vector<Point2D>& vertices, const Pixel& pix = Pixel(0, 255, 0));

    //
    // data
private:
    std::vector<Point2D> _vertices;

    std::vector<Point2D> _bezier_points;

    std::vector<Point2D> _bspline_points;
    std::vector<float>   _bspline_knot_vector;
    int                  _bspline_poly_degree;

    sf::Vector2i _mouse_pos_cache;
    DrawingType  _draw_type;
    const uint   _num_pixels;
    std::unique_ptr<Pixel[]> _pixel_data; // _width * _height pixels

private:
    ConfigFile _config;
    float      _delta_t;
};