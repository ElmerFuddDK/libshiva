SOURCES = ../../src/tester/main.cpp \
../../src/tester/shvluaenginetester.cpp
HEADERS += ../../src/tester/shvluaenginetester.h
TARGET =../bin/luaenginetester
TEMPLATE = app
LIBS += -L../../../../../shiva/shivalib/linux/libshiva \
$$QMAKE_LIBS_DYNLOAD \
-lshiva
QMAKE_CXXFLAGS_DEBUG += -DDEBUG

CONFIG -= release \
 qt

CONFIG += debug
QMAKE_POST_LINK = ../bin/copydeps.sh

INCLUDEPATH += ../../../../..

QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN\''
