TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

CONFIG(release, debug|release): {
	QMAKE_CFLAGS += -Werror
	QMAKE_CXXFLAGS += -Werror
}

CONFIG(debug, debug|release): {
	QMAKE_CXXFLAGS += -fsanitize=address,undefined
	QMAKE_LFLAGS   += -fsanitize=address,undefined
}

include($$PWD/../../xqlib.pri)

SOURCES += \
        main.cpp
