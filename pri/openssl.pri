QMAKE_CFLAGS   += $$system("pkg-config --cflags openssl | sed 's|-I|-isystem |g'")
QMAKE_CXXFLAGS += $$system("pkg-config --cflags openssl | sed 's|-I|-isystem |g'")
QMAKE_LFLAGS   += $$system(pkg-config --libs    openssl)
