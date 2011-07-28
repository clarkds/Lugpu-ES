SOURCES  = window.c shaderprog.cpp ludecomp.cpp example2.cpp stopwatch.cpp

LIBPATHS = -L/usr/lib32 -L/usr/X11R6/lib -L/usr/local/lib
LIBS     = -lX11 -lGLESv2 -lEGL
DEBUG    = -g -DDEBUG
INCLUDES = -I/usr/local/include

example2:
	g++ -o example2 $(LIBPATHS) $(LIBS) $(SOURCES) -m32

debug:
	g++ -o example2 -I$(INCLUDES) $(LIBPATHS) $(LIBS) $(SOURCES) $(DEBUG) -m32

clean:
	rm -rf example2

all:

