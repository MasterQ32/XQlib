

!exists($$PWD/../extern/enet/.libs/libenet.a):{
	error("REQUIRES ENET BUILD...")
}

QMAKE_CFLAGS   += -isystem $$quote($$PWD/../extern/enet/include)
QMAKE_CXXFLAGS += -isystem $$quote($$PWD/../extern/enet/include)
QMAKE_LFLAGS   += $$quote($$PWD/../extern/enet/.libs/libenet.a)
