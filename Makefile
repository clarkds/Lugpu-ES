SOURCES  = context.cpp shaderprog.cpp ludecomp.cpp example2.cpp stopwatch.cpp

LIBPATHS = 
LIBS     = -lX11 -lGLESv2 -lEGL

INCLUDES = 

CFLAGS   = -Wall 
DEBUG    = -g -DDEBUG

debug: $(SOURCES)
	g++ $(CFLAGS) $(DEBUG) -o example2 $(INCLUDES) $(LIBPATHS) $(LIBS) $(SOURCES)

example2: $(SOURCES)
	g++ $(CFLAGS) -o example2 $(INCLUDES) $(LIBPATHS) $(LIBS) $(SOURCES)

clean:
	rm -rf example2

all: debug

