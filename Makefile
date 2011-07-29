SOURCES  = context.cpp shaderprog.cpp ludecomp.cpp example2.cpp stopwatch.cpp

SDKPATH  = /home/merritt/Desktop/SDKPackage_OGLES2

LIBPATHS = -L$(SDKPATH)/Builds/OGLES2/LinuxX86/Lib
LIBS     = -lX11 -lGLESv2 -lEGL

INCLUDES = -I$(SDKPATH)/Builds/OGLES2/Include

CFLAGS   = -Wall -m32
DEBUG    = -g -DDEBUG

example2: $(SOURCES)
	g++ $(CFLAGS) -o example2 $(INCLUDES) $(LIBPATHS) $(LIBS) $(SOURCES)

debug: $(SOURCES)
	g++ $(CFLAGS) $(DEBUG) -o example2 $(INCLUDES) $(LIBPATHS) $(LIBS) $(SOURCES)

clean:
	rm -rf example2

all: debug

