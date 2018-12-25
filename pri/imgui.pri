QMAKE_CFLAGS   += -isystem $$PWD/../extern/imgui
QMAKE_CXXFLAGS += -isystem $$PWD/../extern/imgui
DEPENDPATH     += $$PWD/../extern/imgui

SOURCES += \
	$$PWD/../extern/imgui/imgui_demo.cpp \
	$$PWD/../extern/imgui/imgui_draw.cpp \
	$$PWD/../extern/imgui/imgui_widgets.cpp \
	$$PWD/../extern/imgui/imgui.cpp \
