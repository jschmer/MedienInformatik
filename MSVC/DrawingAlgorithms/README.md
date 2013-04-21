# Implementation of basic 2D drawing algorithms 

Attention: Uses new features of C++11! See [README](https://github.com/jschmer/MedienInformatik) for details.

You need to build 'SFMLApp' before building this project!

### Modes
* Line
    * Draw a line with the mouse
    * Start point will be the MouseDown location
    * End point will be the MouseUp location
    * Algorithm: Bresenham
* Circle
    * Draw a line with the mouse
    * Center will be the MouseDown location
    * Radius will be the distance between MouseUp location and center
    * Algorithm: Midpoint
* Bezier
    * Click to set support points
    * The curve will be rendered if there are more than 2 support points
    * Algorithm: de Casteljau
* B-Spline
    * Click to set support points
    * The parameters for the B-Spline can be modified on-the-fly in the config file "DrawingAlgoApp.cfg"
    * Algorithm: de Boor
* Catmul-Rom Spline
    * Click to set support points
* Fill Rectangle
    * Draw a line with the mouse
    * Rectangle corners will be MouseDown and MouseUp location
    * Color is interpolated between RGB and White (bilinear)
* Fill Triangle
    * Click to set vertices
    * Vertices have to be set CCW (counter clockwise)
    * Color is interpolated between RGB
* Fill Polygon
    * Click to set vertices
    
### Transformations
* Translate
* Scale
    * Relative to origin (0,0) left upper corner
* Rotate
    * Relative to origin (0,0)