

!exists($$PWD/../extern/zstd/lib/libzstd.a):{
	error("REQUIRES ZSTD BUILD...")
}

QMAKE_CFLAGS   += -isystem $$quote($$PWD/../extern/zstd/lib)
QMAKE_CXXFLAGS += -isystem $$quote($$PWD/../extern/zstd/lib)
LIBS           += $$quote($$PWD/../extern/zstd/lib/libzstd.a)
