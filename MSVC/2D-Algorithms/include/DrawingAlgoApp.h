/* 
 * Copyright (c) 2013 Jens Schmer
 * This file is distributed under the MIT License.
 * Consult COPYING within this package for further information. 
 */
#pragma once

#include <memory>

#include <glm/glm.hpp>

#include <SFMLApp.h>

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
        BSplineClosed,
        CatmulRomSpline,
        FillRectangle,
        FillTriangle,
        FillPolygon,
        SetClippingRect
    };

    enum class TransformationType {
        None,
        Translate,
        Scale,
        Rotate,
        Shear,
        SetOrigin
    };

    struct ClippingRectangle {
        int xmin, xmax;
        int ymin, ymax;
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
    void RenderCurrentMode();

    //
    // event handler
protected:
    virtual void OnKeyPressed(sf::Keyboard::Key key, bool ctrl, bool alt, bool shift, bool system) override;
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
    void DrawCurrentMode();
    void DrawLineBresenham(Point2D p0, Point2D p1, const Color& color = Color(0, 255, 0));
    void DrawLineMidpoint (Point2D p0, Point2D p1, const Color& color = Color(0, 255, 0));
    void DrawCircle(Point2D center, uint radius, const Color& color = Color(0, 255, 0));
    void DrawBezier(const std::vector<Point2D>& support_points);
    void DrawBSpline(const std::vector<Point2D>& support_points, const std::vector<float> _knot_vector);
    void DrawBSplineClosed(const std::vector<Point2D>& support_points, std::vector<float> _knot_vector);
    void DrawCatmulRomSpline(const std::vector<Point2D>& support_points);

    void FillRectangle(const Point2D p0, const Point2D p1);
    void FillTriangle(std::vector<Point2D>& vertices);
    void FillPolygon(const std::vector<Point2D>& vertices, const Color& color = Color(0, 255, 0));

    bool ClipLine(Point2D &start, Point2D &end) const;

    int ClippingOutcodeFor(Point2D p) const;

    //
    // data
private:
    static const int width  = 800;
    static const int height = 600;

    ClippingRectangle _clipping_rect;
    std::vector<Point2D> _vertices;

    std::vector<float>   _bspline_knot_vector;
    int                  _bspline_poly_degree;

    sf::Vector2f _mouse_pos_cache;
    DrawingType  _draw_type;
    const uint   _num_Colors;
    std::unique_ptr<Color[]> _Color_data; // _width * _height Colors

    glm::mat3          _transform_vec;
    Point2D            _transform_origin;
    TransformationType _transform_type;

private:
    ConfigFile _config;
    float      _delta_t;
    sf::Text   _current_mode_text;
    bool       _show_current_mode;
};