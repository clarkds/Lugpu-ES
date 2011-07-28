/*
 * This proprietary software may be used only as
 * authorised by a licensing agreement from ARM Limited
 * (C) COPYRIGHT 2009 - 2010 ARM Limited
 * ALL RIGHTS RESERVED
 * The entire notice above must be reproduced on all authorised
 * copies and copies may only be made to the extent permitted
 * by a licensing agreement from ARM Limited.
 */

/* 
 * window.c
 * Platform-specific window setup.
 */

#include "window.h"


/* Waits for map notification */
Bool wait_for_map(Display *d, XEvent *e, char *win_ptr) {
	return (e->type == MapNotify && e->xmap.window == (*((Window*)win_ptr)));
}

/* Creates an X window */
Window create_window(const char *title, int width, int height, Display* display, 
	EGLDisplay sEGLDisplay, EGLConfig FBConfig, Colormap *pColormap, 
	XVisualInfo **ppVisual) 
{
    XSetWindowAttributes wa;
    XSizeHints sh;
    XEvent e;
    XVisualInfo *visual, templt;

    unsigned long mask;
    long screen;

    Colormap colormap;

    int vID, n;

    Window window;

    screen = DefaultScreen(display);
    eglGetConfigAttrib(sEGLDisplay, FBConfig, EGL_NATIVE_VISUAL_ID, &vID);
    templt.visualid = vID;
    visual = XGetVisualInfo(display, VisualIDMask, &templt, &n);

	if (!visual) {
		printf("Couldn't get X visual info\n");
		exit(-1);
	}

    colormap = XCreateColormap(display, RootWindow(display, screen), visual->visual, AllocNone);

    wa.colormap = colormap;
    wa.background_pixel = 0xFFFFFFFF;
    wa.border_pixel = 0;
    wa.event_mask = StructureNotifyMask | ExposureMask;

    mask = CWBackPixel | CWBorderPixel | CWEventMask | CWColormap;

    window = XCreateWindow(display, RootWindow(display, screen), 0, 0, width, height, 
		0, visual->depth, InputOutput, visual->visual, mask, &wa);

    sh.flags = USPosition;
    sh.x = 10;
    sh.y = 10;

    XSetStandardProperties(display, window, title, title, None, 0, 0, &sh);
    XMapWindow(display, window);
    XIfEvent(display, &e, wait_for_map, (char*)&window);
    XSetWMColormapWindows(display, window, &window, 1);
    XFlush(display);

    *pColormap = colormap;
    *ppVisual = visual;

    return window;
}



