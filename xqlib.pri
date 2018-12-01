
CONFIG += xgl_use_glm

QMAKE_CXXFLAGS += -std=c++17

include($$PWD/pri/sdl2.pri)
include($$PWD/pri/gsl.pri)
include($$PWD/XGL/XGL.pri)
include($$PWD/XM/XM.pri)
include($$PWD/XCS/XCS.pri)
include($$PWD/XAPP/XAPP.pri)
include($$PWD/XLOG/XLOG.pri)
