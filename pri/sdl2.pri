
QMAKE_CFLAGS   += $$system(pkg-config --cflags sdl2)
QMAKE_CXXFLAGS += $$system(pkg-config --cflags sdl2)
QMAKE_LFLAGS   += $$system(pkg-config --libs   sdl2)
