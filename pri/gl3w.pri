SOURCES += \
	$$PWD/../extern/gl3w/src/gl3w.c

INCLUDEPATH += $$PWD/../extern/gl3w/include
DEPENDPATH  += $$PWD/../extern/gl3w/include

LIBS += -ldl -lGL
