TEMPLATE = lib

CONFIG = dll \
debug \
 warn_on
LIBS += -ldl \
 -lshiva \
 -L../../../../shivalib/linux/libshiva

TARGET =../bin/subprocess

QMAKE_CXXFLAGS_DEBUG += -DDEBUG

SOURCES += ../../src/modules/shvsubprocessimpl.cpp \
../../src/modules/shvsubprocessserverimpl.cpp \
 ../../src/shvsubprocessfactory.cpp \
 ../../src/shvsubprocessexports.cpp
HEADERS += ../../../../include/modules/subprocess/shvsubprocess.h \
 ../../src/modules/shvsubprocessimpl.h \
 ../../src/modules/shvsubprocessserverimpl.h \
 ../../src/shvsubprocessfactory.h

INCLUDEPATH += ../../../../..
