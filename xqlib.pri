WARNINGS= \
	-Weverything \
	-Wno-c++98-compat -Wno-c++98-compat-pedantic \
	-Wno-padded \
	-pedantic


# Setup all warnings
QMAKE_CFLAGS   += $$WARNINGS
QMAKE_CXXFLAGS += $$WARNINGS
QMAKE_LFLAGS   += $$WARNINGS

# Install C++17
QMAKE_CXXFLAGS += -std=c++17

include($$PWD/pri/sdl2.pri)
include($$PWD/pri/gsl.pri)
include($$PWD/pri/stb.pri)
include($$PWD/pri/gl3w.pri)

DEFINES += XGL_ENABLE_GLM

INCLUDEPATH += $$PWD/include
DEPENDPATH  += $$PWD/include

HEADERS += \
	$$PWD/include/xm/2d/line \
	$$PWD/include/xm/2d/circle \
	$$PWD/include/xm/2d/ray \
	$$PWD/include/xgl/shader \
	$$PWD/include/xgl/texture \
	$$PWD/include/xgl/vertexarray \
	$$PWD/include/xgl/program \
	$$PWD/include/xgl/buffer \
	$$PWD/include/xgl/all \
	$$PWD/include/xgl/detail/program_uniforms.h \
	$$PWD/include/xapp \
	$$PWD/include/xcs \
	$$PWD/include/xlog \
	$$PWD/include/x2d/sprite_batch \
	$$PWD/include/x2d/texture_loader

SOURCES += \
	$$PWD/src/xlog.cpp \
	$$PWD/src/xapp.cpp\
	$$PWD/src/xcs.cpp \
    $$PWD/src/sprite_batch.cpp \
    $$PWD/src/texture_loader.cpp
