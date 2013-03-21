# Simple SFML Application implementation

### Provides
* a Event class with
    * an event dispatcher (OnEvent())
    * event handler functions
* a SFMLApp that derives from the Event class with
    * A RenderWindow (usable with OpenGL)
    * A default simple application lifecycle
    * Also handles window resizing and window closing
    * It even loads a default Font for text rendering: geo_1.ttf
        * The font must be in the working directory of the application if you want to see a window ;-)
    * A little help text/menu to demonstrate text rendering and event handling

### How to set up a project that uses this lib
* Add includes:
    * for SFML
    * for SFMLApp
* Add preprocessor definitions:
    * SFML_STATIC
* Link to SFMLApp.lib (or SFMLApp-d.lib for debug builds)
* Provide a build step that copies the 'geo_1.ttf' font to the working directory
    * Optional: Change the debugging working directory of Visual Studio to $(OutDir)

### How to use provided classes
* Either derive directly from Event if you'll implement your own application lifecycle and event handlers
* Or derive from SFMLApp and customize this class (you can override some functions, see the header for details)