WARNINGS= \
	-pedantic \
	-Weverything \
	-Wno-c++98-compat -Wno-c++98-compat-pedantic \
	-Wno-padded \
	-Wno-exit-time-destructors \
	-Wno-global-constructors \
	-Werror=return-type \
	-Wno-return-std-move-in-c++11

contains(XQLIB, app):     XQLIB *= sdl2 gl3w input imgui
contains(XQLIB, imgui):   XQLIB *= gl3w
contains(XQLIB, http):    XQLIB *= network curl
contains(XQLIB, network): XQLIB *= gsl io
contains(XQLIB, gl3w):    XQLIB *= gsl
contains(XQLIB, gl3w):    XQLIB *= optional
contains(XQLIB, math):    XQLIB *= gsl
contains(XQLIB, io):      XQLIB *= gsl
contains(XQLIB, io):      XQLIB *= optional

CONFIG(debug, debug|release) {
	DEFINES += DEBUG
}

# Setup all warnings
QMAKE_CFLAGS   += $$WARNINGS
QMAKE_CXXFLAGS += $$WARNINGS
QMAKE_LFLAGS   += $$WARNINGS

# Install C++17
include($$PWD/pri/c++17.pri)

## XQLib Module List:
# builtin packages:
#     app
#     network
#     math
#     io
#     input
# system packages:
#     sdl2
#     sdl2_image
#     sdl2_ttf
#     gsl
#     stb
#     gl3w
#     curl
#     enet
#     imgui
#     optional
#     json
#     guid
#     sqlite3
#     openssl
#     lzma
#     nfd
#     zstd
#     ode

#include external modules
contains(XQLIB, sdl2):       include($$PWD/pri/sdl2.pri)
contains(XQLIB, sdl2_image): include($$PWD/pri/sdl2_image.pri)
contains(XQLIB, sdl2_ttf):   include($$PWD/pri/sdl2_ttf.pri)
contains(XQLIB, gsl):        include($$PWD/pri/gsl.pri)
contains(XQLIB, stb):        include($$PWD/pri/stb.pri)
contains(XQLIB, gl3w):       include($$PWD/pri/gl3w.pri)
contains(XQLIB, curl):       include($$PWD/pri/curl.pri)
contains(XQLIB, enet):       include($$PWD/pri/enet.pri)
contains(XQLIB, imgui):      include($$PWD/pri/imgui.pri)
contains(XQLIB, optional):   include($$PWD/pri/optional.pri)
contains(XQLIB, json):       include($$PWD/pri/json.pri)
contains(XQLIB, guid):       include($$PWD/pri/stduuid.pri)
contains(XQLIB, sqlite3):    include($$PWD/pri/sqlite3.pri)
contains(XQLIB, openssl):    include($$PWD/pri/openssl.pri)
contains(XQLIB, lzma):       include($$PWD/pri/lzma.pri)
contains(XQLIB, nfd):        include($$PWD/pri/nativefiledialog.pri)
contains(XQLIB, zstd):       include($$PWD/pri/zstd.pri)
contains(XQLIB, ode):        include($$PWD/pri/ode.pri)

DEFINES += XGL_ENABLE_GLM

INCLUDEPATH += $$PWD/include
DEPENDPATH  += $$PWD/include

HEADERS += \
	$$PWD/include/glm \
	$$PWD/include/ode \
  $$PWD/include/sdl2++/exception \
  $$PWD/include/sdl2++/renderer \
  $$PWD/include/sdl2++/texture \
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
	$$PWD/include/xm/3d/frustum \
	$$PWD/include/xm/3d/sphere \
	$$PWD/include/xnet/http/client \
	$$PWD/include/xnet/socket \
	$$PWD/include/xnet/http/server \
	$$PWD/include/xnet/dns \
	$$PWD/include/xnet/ip \
	$$PWD/include/xio/ostream \
	$$PWD/include/xio/istream \
	$$PWD/include/xnet/socket_stream \
	$$PWD/include/xio/iostream \
  $$PWD/include/xstd/flexref \
  $$PWD/include/xstd/locked_value \
  $$PWD/include/xstd/modern_integers \
	$$PWD/include/xstd/multi_array \
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
	$$PWD/include/xutility/debug_camera \
	$$PWD/include/xgraphics/shader_preprocessor \
	$$PWD/include/xgl/query \
	$$PWD/include/xgraphics/color_literals

!contains(XQLIB,reference_extern): {
	message("include xqlib source")

	contains(XQLIB, app) {
		SOURCES += $$PWD/src/xapp.cpp
	}

	contains(XQLIB, gl3w): {
		SOURCES += \
			$$PWD/src/shader_preprocessor.cpp \
			$$PWD/src/sprite_batch.cpp \
			$$PWD/src/debug_draw.cpp \
	}

	contains(XQLIB, imgui): {
		SOURCES += \
			$$PWD/src/imgui_gl3.cpp \
			$$PWD/src/imgui_sdl.cpp
	}

	contains(XQLIB, input): {
		SOURCES += $$PWD/src/xinput.cpp
		contains(XQLIB, sdl2):  SOURCES += $$PWD/src/xinput_sdl.cpp
	}

	contains(XQLIB, gl3w):contains(XQLIB, stb) {
		SOURCES += $$PWD/src/texture_loader.cpp
	}

	contains(XQLIB, stb): {
		SOURCES += \
			$$PWD/src/stb.cpp
	}

	contains(XQLIB, io): {
		SOURCES += \
			$$PWD/src/xio.cpp
	}

	contains(XQLIB, math): {
		SOURCES += \
			$$PWD/src/xm3d.cpp
	}

	contains(XQLIB, sdl2): {
    SOURCES += \
      $$PWD/src/sdl2++.cpp
  }

	SOURCES += \
		$$PWD/src/xlog.cpp \
		$$PWD/src/xcs.cpp \
		$$PWD/src/xception.cpp \
		$$PWD/src/xstd_format.cpp \
		$$PWD/src/debug_camera.cpp \
		$$PWD/src/xstd_fixed.cpp

	contains(XQLIB, zstd):    SOURCES += $$PWD/src/zstd.cpp
	contains(XQLIB, lzma):    SOURCES += $$PWD/src/lzma.cpp
	contains(XQLIB, sqlite3): SOURCES += $$PWD/src/sqlite3.cpp
	contains(XQLIB, guid):    SOURCES += $$PWD/src/guid.cpp
	contains(XQLIB, network): SOURCES += $$PWD/src/xnet.cpp
	contains(XQLIB, http):    SOURCES += $$PWD/src/xnet_http.cpp
}
