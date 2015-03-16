include(../../../../../shiva/include/shiva.pri)

SOURCES = ../../src/tester/main.cpp \
../../src/tester/shvjsonstreamtester.cpp
HEADERS += ../../src/tester/shvjsonstreamtester.h
TARGET =../bin/jsonstreamtester
TEMPLATE = app
LIBS += -L../../../../shivalib/linux/libshiva \
$$QMAKE_LIBS_DYNLOAD \
-lshiva

CONFIG -= release \
 qt

CONFIG += debug
QMAKE_POST_LINK = ../bin/copydeps.sh

INCLUDEPATH += ../../../../..
