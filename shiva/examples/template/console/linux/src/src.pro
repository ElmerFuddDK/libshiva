SOURCES = ../../src/main.cpp \
../../src/shvshvtemplateshv.cpp
HEADERS += ../../src/shvshvtemplateshv.h
TARGET =../bin/shvtemplateshv
TEMPLATE = app
LIBS += -L../../../SHVRelPathSHV/shiva/shivalib/linux/libshiva \
$$QMAKE_LIBS_DYNLOAD \
-lshiva
QMAKE_CXXFLAGS_DEBUG += -DDEBUG

CONFIG -= release \
 qt

CONFIG += debug
QMAKE_POST_LINK = ../bin/copydeps.sh

INCLUDEPATH += ../../../SHVRelPathSHV
