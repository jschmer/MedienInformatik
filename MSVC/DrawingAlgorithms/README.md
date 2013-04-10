# Implementation of basic 2D drawing algorithms 

Attention: Uses new features of C++11! See [README](https://github.com/jschmer/MedienInformatik) for details.

You need to build 'SFMLApp' before building this project!

### Supports
* Bresenham
* Midpoint
* de Casteljau
* de Boor

### Modes
* Line
    * Draw a line with the mouse
    * Start point will be the MouseDown location
    * End point will be the MouseUp location
* Circle
    * Draw a line with the mouse
    * Center will be the MouseDown location
    * Radius will be the distance between MouseUp location and center
* Bezier
    * Click to set support points
    * The curve will be rendered if there are more than 2 support points
* B-Spline
    * Click to set support points
    * The parameters for the B-Spline can be modified on-the-fly in the config file "DrawingAlgoApp.cfg"
* Fill Rectangle
    * Draw a line with the mouse
    * Rectangle corners will be MouseDown and MouseUp location
    * Color is interpolated between RGB and White (bilinear)
* Fill Triangle
    * Click to set vertices
    * Vertices have to be set CCW (counter clockwise)
    * Color is interpolated between RGB
    