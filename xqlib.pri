WARNINGS= \
	-pedantic \
	-Weverything \
	-Wno-c++98-compat -Wno-c++98-compat-pedantic \
	-Wno-padded \
	-Wno-exit-time-destructors \
	-Wno-global-constructors

# Setup all warnings
QMAKE_CFLAGS   += $$WARNINGS
QMAKE_CXXFLAGS += $$WARNINGS
QMAKE_LFLAGS   += $$WARNINGS

# Install C++17
CONFIG -= c++11
CONFIG += c++17
QMAKE_CXXFLAGS += -std=c++17
QMAKE_LFLAGS += -std=c++17

#include external modules
include($$PWD/pri/sdl2.pri)
include($$PWD/pri/gsl.pri)
include($$PWD/pri/stb.pri)
include($$PWD/pri/gl3w.pri)
include($$PWD/pri/curl.pri)

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
	$$PWD/include/xgraphics/sprite_batch \
	$$PWD/include/xgraphics/texture_loader \
	$$PWD/include/xgraphics/debug_draw \
    $$PWD/include/xinput/xinput \
    $$PWD/include/xinput/sdl \
    $$PWD/include/xcommon \
    $$PWD/include/xcept \
    $$PWD/include/xgraphics/shader_compiler \
    $$PWD/include/xnet/http

SOURCES += \
	$$PWD/src/xlog.cpp \
	$$PWD/src/xapp.cpp\
	$$PWD/src/xcs.cpp \
    $$PWD/src/sprite_batch.cpp \
    $$PWD/src/texture_loader.cpp \
    $$PWD/src/debug_draw.cpp \
    $$PWD/src/xinput.cpp \
    $$PWD/src/xinput_sdl.cpp \
    $$PWD/src/xnet.cpp
