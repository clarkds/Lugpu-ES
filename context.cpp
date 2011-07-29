#include <stdio.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <GLUT/glut.h>

#else

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#endif

#include <stdlib.h>
#include <stdio.h>

#include "context.h"
#include "common.h"

Context::Context() {
}

Context::~Context() {
}

void Context::initialize() {
  //X11 Varialbes
  long		        x11Screen	= 0;
  Visual*           x11Visual	= 0;

  //EGL Variables
  EGLConfig			eglConfig	= 0;
  EGLSurface		eglSurface	= 0;
  EGLContext		eglContext	= 0;

  //EGL Config Variables

  EGLint  aEGLAttributes[] = {
    EGL_RED_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_BLUE_SIZE, 8,
    EGL_LUMINANCE_SIZE, 32,
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
    EGL_NONE
  };
	
  EGLint ai32ContextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
	
  //create Native WindowType for OpenGL ES output

  Window			sRootWindow;
  XSetWindowAttributes	        sWA;
  unsigned int			ui32Mask;
  int				i32Depth;
  int 				i32Width, i32Height;
	
  // Initializes the display and screen
  x11Display = XOpenDisplay( 0 );
  if (!x11Display)
    {
      printf("Error: Unable to open X display\n");
      exit(-1);
    }
  x11Screen = XDefaultScreen( x11Display );

  // Gets the window parameters
  sRootWindow = RootWindow(x11Display, x11Screen);
  i32Depth = DefaultDepth(x11Display, x11Screen);
  x11Visual = DefaultVisual(x11Display, x11Screen);

  x11Colormap = XCreateColormap( x11Display, sRootWindow,x11Visual, AllocNone );
  sWA.colormap = x11Colormap;
  
  // Add to these for handling other events
    sWA.event_mask = StructureNotifyMask | ExposureMask | ButtonPressMask | ButtonReleaseMask | KeyPressMask | KeyReleaseMask;
    ui32Mask = CWBackPixel | CWBorderPixel | CWEventMask | CWColormap;

  i32Width  = WINDOW_WIDTH  < XDisplayWidth(x11Display, x11Screen) ? WINDOW_WIDTH : XDisplayWidth(x11Display, x11Screen);
  i32Height = WINDOW_HEIGHT < XDisplayHeight(x11Display,x11Screen) ? WINDOW_HEIGHT: XDisplayHeight(x11Display,x11Screen);

  // Creates the X11 window
  x11Window = XCreateWindow( x11Display, RootWindow(x11Display, x11Screen), 0, 0, i32Width, i32Height, 0, CopyFromParent, InputOutput, x11Visual, ui32Mask, &sWA);
  
  XMapWindow(x11Display, x11Window);
  XFlush(x11Display);

  GLint iErr;
  //get the default display type for EGL
  eglDisplay = eglGetDisplay((EGLNativeDisplayType)x11Display);
  iErr = eglGetError();
  fprintf(stderr,"getDisplay Failed: %i", iErr);

  //Initialize the EGL display
  eglInitialize(eglDisplay, NULL, NULL);
  iErr = eglGetError();
  fprintf(stderr,"Init failed: %i", iErr);

  //Make OpenGL ES the current API
  eglBindAPI(EGL_OPENGL_ES_API);
  
  int iConfigs;
  eglChooseConfig(eglDisplay, aEGLAttributes, &eglConfig, 1, &iConfigs);
  iErr = eglGetError();
  fprintf(stderr, "chooseConfig: %i",iErr);
	if (iConfigs == 0) {
        printf("No EGL configurations were returned.\n");
		exit(-1);
    }

	eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, (EGLNativeWindowType)x11Window, NULL);

	if (eglSurface == EGL_NO_SURFACE) {
	  printf("Failed to create EGL surface.\n");
	  exit(-1);
	}
	
 	eglContext = eglCreateContext(eglDisplay, eglConfig, NULL, ai32ContextAttribs);

	if (eglContext == EGL_NO_CONTEXT) {
	  printf("Failed to create EGL context.\n");
	  exit(-1);
	}
	
	EGL_CHECK(eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext));
}

void Context::destroy() {
    eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT) ;
	eglTerminate(eglDisplay);
	
	XDestroyWindow(x11Display, x11Window);
	XFreeColormap(x11Display, x11Colormap);
	XCloseDisplay(x11Display);
}

