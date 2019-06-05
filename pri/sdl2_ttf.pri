QMAKE_CFLAGS   += $$system("pkg-config --cflags SDL2_ttf | sed 's|-I|-isystem |g'")
QMAKE_CXXFLAGS += $$system("pkg-config --cflags SDL2_ttf | sed 's|-I|-isystem |g'")
QMAKE_LFLAGS   += $$system("pkg-config --libs   SDL2_ttf")
