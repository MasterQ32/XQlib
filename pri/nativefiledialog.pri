QMAKE_CXXFLAGS += -isystem $$PWD/../extern/nativefiledialog/src/include
DEPENDPATH     += $$PWD/../extern/nativefiledialog/src/include
LIBS           += $$PWD/../extern/nativefiledialog/build/lib/Release/x64/libnfd.a

QMAKE_CFLAGS   += $$system("pkg-config --cflags gtk+-3.0 | sed 's|-I|-isystem |g'")
QMAKE_CXXFLAGS += $$system("pkg-config --cflags gtk+-3.0 | sed 's|-I|-isystem |g'")
QMAKE_LFLAGS   += $$system("pkg-config --libs   gtk+-3.0")
