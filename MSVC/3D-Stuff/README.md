# Implementation of some basic 3D OpenGL stuff

Attention: Uses new features of C++11! See [README](https://github.com/jschmer/MedienInformatik) for details.

You need to build 'SFMLApp' before building this project!

### Modes
* Teapot
  * Standard teapot with object transformations and lighting
* Robot
  * Custom built "robot" that can rotate its arms (used to demonstrate matrix stacks)
* ObjViewer
  * Load and display *.obj files (vertex and face definitions only)
  * 2 modes for normal calculation at each vertex (flat and averaged [Gouraud])
  * Notice: not really flexible and hardcoded bunny file! :)