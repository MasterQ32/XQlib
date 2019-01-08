

!exists($$PWD/../extern/enet/.libs/libenet.a):{
	error("REQUIRES ENET BUILD...")
}

QMAKE_CFLAGS   += -isystem $$quote($$PWD/../extern/enet/include)
QMAKE_CXXFLAGS += -isystem $$quote($$PWD/../extern/enet/include)
LIBS           += $$quote($$PWD/../extern/enet/.libs/libenet.a)
