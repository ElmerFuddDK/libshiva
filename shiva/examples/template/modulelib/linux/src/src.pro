include(../../../SHVRelPathSHV/shiva/include/shiva.pri)

SOURCES = ../../src/tester/main.cpp \
../../src/tester/shvshvtemplateshvtester.cpp

HEADERS += ../../src/tester/shvshvtemplateshvtester.h

TARGET =../bin/shvtemplateshvtester
TEMPLATE = app
LIBS += -L../../../SHVRelPathSHV/shiva/shivalib/linux/libshiva \
$$QMAKE_LIBS_DYNLOAD \
-lshiva

CONFIG -= release \
 qt

CONFIG += debug
QMAKE_POST_LINK = ../bin/copydeps.sh

INCLUDEPATH += ../../../SHVRelPathSHV
