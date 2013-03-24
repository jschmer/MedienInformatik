#include <SFMLApp.h>

#include <memory>

class DrawingAlgoApp : public SFMLApp {
    typedef SFMLApp Super;
    typedef unsigned char BYTE;

    //
    // additional type declarations
    struct Pixel {
        BYTE R, G, B;

        Pixel();
        Pixel(uint R, uint G, uint B);
        Pixel(uint hex); // 0xRRGGBB
    };

    enum class DrawingType {
        None,
        Line,
        Circle
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
    virtual void OnMouseButtonPressed(sf::Mouse::Button button, int x, int y);
    virtual void OnMouseButtonReleased(sf::Mouse::Button button, int x, int y);

    //
    // drawing helper
private:
    void RenderPixelArray();
    void setPixel(uint x, uint y, Pixel &pix);

    //
    // drawing algorithms
private:
    void DrawLineBresenham(uint x1, uint y1, uint x2, uint y2);
    void DrawLineMidpoint (uint x1, uint y1, uint x2, uint y2);
    void DrawCircle(uint posx, uint posy, uint radius);

    //
    // data
private:
    sf::Vector2i _mouse_pos_cache;
    DrawingType  _draw_type;
    const uint   _num_pixels;
    std::unique_ptr<Pixel[]> _pixel_data; // _width * _height pixels
};