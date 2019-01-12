QMAKE_CFLAGS   += $$system("pkg-config --cflags sqlite3 | sed 's|-I|-isystem |g'")
QMAKE_CXXFLAGS += $$system("pkg-config --cflags sqlite3 | sed 's|-I|-isystem |g'")
QMAKE_LFLAGS   += $$system(pkg-config --libs    sqlite3)
