QMAKE_CFLAGS   += $$system("ode-config --cflags")
QMAKE_CXXFLAGS += $$system("ode-config --cflags")
QMAKE_LFLAGS   += $$system("ode-config --libs")
