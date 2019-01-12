

QMAKE_CXXFLAGS += -isystem $$PWD/../extern/stduuid/include
DEPENDPATH     += $$PWD/../extern/stduuid/include

QMAKE_CFLAGS   += $$system("pkg-config --cflags uuid | sed 's|-I|-isystem |g'")
QMAKE_CXXFLAGS += $$system("pkg-config --cflags uuid | sed 's|-I|-isystem |g'")
QMAKE_LFLAGS   += $$system("pkg-config --libs   uuid")
