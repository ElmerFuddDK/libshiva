SOURCES = ../../src/tester/main.cpp \
../../src/tester/shvjsonstreamtester.cpp
HEADERS += ../../src/tester/shvjsonstreamtester.h
TARGET =../bin/jsonstreamtester
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
