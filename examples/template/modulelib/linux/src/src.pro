SOURCES = ../../src/tester/main.cpp \
../../src/tester/shvshvtemplateshvtester.cpp
HEADERS += ../../src/tester/shvshvtemplateshvtester.h
TARGET =../bin/shvtemplateshvtester
TEMPLATE = app
LIBS += -L../../SHVRelPathSHV/shivalib/linux/libshiva \
-ldl \
-lshiva
QMAKE_CXXFLAGS_DEBUG += -DDEBUG

CONFIG -= release \
 qt

CONFIG += debug
QMAKE_POST_LINK = ../bin/copydeps.sh

INCLUDEPATH += ../../../SHVRelPathSHV
