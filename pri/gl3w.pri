SOURCES += \
	$$PWD/../extern/gl3w/src/gl3w.c

QMAKE_CFLAGS   += -isystem $$PWD/../extern/gl3w/include
QMAKE_CXXFLAGS += -isystem $$PWD/../extern/gl3w/include
DEPENDPATH     += $$PWD/../extern/gl3w/include

LIBS += -ldl -lGL
