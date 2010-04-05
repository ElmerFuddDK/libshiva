SOURCES = ../../src/tester/main.cpp \
../../src/tester/shvsubprocesstester.cpp
HEADERS += ../../src/tester/shvsubprocesstester.h
TARGET =../bin/subprocesstester
TEMPLATE = app
LIBS += -L../../../../shivalib/linux/libshiva \
-ldl \
-lshiva
QMAKE_CXXFLAGS_DEBUG += -DDEBUG

CONFIG -= release \
 qt

CONFIG += debug
QMAKE_POST_LINK = ../bin/copydeps.sh

INCLUDEPATH += ../../../../..
