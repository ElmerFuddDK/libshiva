SOURCES = ../../src/main.cpp \
../../src/shvshvtemplateshv.cpp
HEADERS += ../../src/shvshvtemplateshv.h
TARGET =../bin/shvtemplateshv
TEMPLATE = app
LIBS += -L../../SHVRelPathSHV/shivalib/linux/libshiva \
-ldl \
-lshiva
QMAKE_CXXFLAGS_DEBUG += -DDEBUG

CONFIG -= release \
 qt

CONFIG += debug
QMAKE_POST_LINK = ../bin/copydeps.sh

INCLUDEPATH += ../../SHVRelPathSHV
