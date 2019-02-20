WARNINGS= \
	-pedantic \
	-Weverything \
	-Wno-c++98-compat -Wno-c++98-compat-pedantic \
	-Wno-padded \
	-Wno-exit-time-destructors \
	-Wno-global-constructors

CONFIG(debug, debug|release) {
	DEFINES += DEBUG
}

# Setup all warnings
QMAKE_CFLAGS   += $$WARNINGS
QMAKE_CXXFLAGS += $$WARNINGS
QMAKE_LFLAGS   += $$WARNINGS

# Install C++17
include($$PWD/pri/c++17.pri)

#include external modules
include($$PWD/pri/sdl2.pri)
include($$PWD/pri/gsl.pri)
include($$PWD/pri/stb.pri)
include($$PWD/pri/gl3w.pri)
include($$PWD/pri/curl.pri)
include($$PWD/pri/enet.pri)
include($$PWD/pri/imgui.pri)
include($$PWD/pri/optional.pri)
include($$PWD/pri/json.pri)
include($$PWD/pri/stduuid.pri)
include($$PWD/pri/sqlite3.pri)
include($$PWD/pri/openssl.pri)
include($$PWD/pri/lzma.pri)
include($$PWD/pri/nativefiledialog.pri)
include($$PWD/pri/zstd.pri)

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
	$$PWD/include/xcept \
	$$PWD/include/xgraphics/shader_compiler \
	$$PWD/include/xnet/http/client \
	$$PWD/include/xnet/socket \
	$$PWD/include/xnet/http/server \
	$$PWD/include/xnet/dns \
	$$PWD/include/xnet/ip \
	$$PWD/include/xio/ostream \
	$$PWD/include/xio/istream \
	$$PWD/include/xnet/socket_stream \
	$$PWD/include/xio/iostream \
    $$PWD/include/xstd/unique_id \
    $$PWD/include/xstd/dynamic_array \
    $$PWD/include/xgraphics/turtle_font.hpp \
    $$PWD/include/imgui \
    $$PWD/include/xgraphics/colors \
    $$PWD/include/xgl/framebuffer \
    $$PWD/include/xstd/reference_equals \
    $$PWD/include/xstd/random \
    $$PWD/include/xstd/optional \
    $$PWD/include/xm/3d/plane \
    $$PWD/include/xm/3d/ray \
    $$PWD/include/xm/3d/intersect \
    $$PWD/include/xm/3d/aabb \
    $$PWD/include/xstd/resource \
    $$PWD/include/xstd/guid \
    $$PWD/include/xnet/http/utility \
    $$PWD/include/xdb/sqlite3 \
    $$PWD/include/xio/compression/lzma \
    $$PWD/include/xio/memory_stream \
    $$PWD/include/xio/file_stream \
    $$PWD/include/xio/utility \
    $$PWD/include/xgl/program_pipeline \
    $$PWD/include/xio/seekable_stream \
    $$PWD/include/xutility/perf_counter \
    $$PWD/include/xio/compression/zstd \
    $$PWD/include/xstd/format \
    $$PWD/include/xgraphics/hsv \
    $$PWD/include/xio/simple \
    $$PWD/include/xstd/fixed_string \
    $$PWD/include/xstd/fixed \
    $$PWD/include/xstd/type_traits/integer \
    $$PWD/include/xutility/debug_camera

!contains(CONFIG,xqlib_extern):{
		message("include xqlib source")
		SOURCES += \
		$$PWD/src/xlog.cpp \
		$$PWD/src/xapp.cpp\
		$$PWD/src/xcs.cpp \
		$$PWD/src/sprite_batch.cpp \
		$$PWD/src/texture_loader.cpp \
		$$PWD/src/debug_draw.cpp \
		$$PWD/src/xinput.cpp \
		$$PWD/src/xinput_sdl.cpp \
		$$PWD/src/xnet.cpp \
    $$PWD/src/xception.cpp \
    $$PWD/src/xio.cpp \
    $$PWD/src/imgui_gl3.cpp \
    $$PWD/src/imgui_sdl.cpp \
    $$PWD/src/xm3d.cpp \
    $$PWD/src/guid.cpp \
    $$PWD/src/sqlite3.cpp \
    $$PWD/src/lzma.cpp
}

SOURCES += $$PWD/src/zstd.cpp \
    $$PWD/src/xstd_format.cpp \
    $$PWD/src/xstd_fixed.cpp \
    $$PWD/src/debug_camera.cpp
