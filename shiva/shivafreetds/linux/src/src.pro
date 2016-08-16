include(../../../shiva/include/shiva.pri)

SOURCES = ../../src/tester/main.cpp \
    ../../src/tester/shvfreetdstester.cpp

HEADERS += \
    ../../src/tester/shvfreetdstester.h

TARGET =../bin/shivafreetdstester
TEMPLATE = app
LIBS += -L../../../../shiva/shiva/shivalib/linux/libshiva \
$$QMAKE_LIBS_DYNLOAD \
-lshiva

CONFIG -= release \
 qt

CONFIG += debug
QMAKE_POST_LINK = ../bin/copydeps.sh

QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN\''

INCLUDEPATH += ../../.. ../..
