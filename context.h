#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <EGL/egl.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

class Context {
public:
    Context();
    ~Context();
    
    void initialize();
    void destroy();
    
    Window			x11Window;
    Display*		x11Display;
    Colormap	    x11Colormap;
    
    EGLDisplay eglDisplay;

};

#endif
