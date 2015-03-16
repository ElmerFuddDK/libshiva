include(../../../SHVRelPathSHV/shiva/include/shiva.pri)

TEMPLATE = lib

CONFIG = dll \
debug \
 warn_on
LIBS += $$QMAKE_LIBS_DYNLOAD \
 -lshiva \
 -L../../../SHVRelPathSHV/shiva/shivalib/linux/libshiva

TARGET =../bin/shvtemplateshv

SOURCES += ../../src/modules/shvshvtemplateshvimpl.cpp \
 ../../src/shvshvtemplateshvfactory.cpp \
 ../../src/shvshvtemplateshvexports.cpp
HEADERS += ../../include/modules/shvshvtemplateshv.h \
 ../../src/modules/shvshvtemplateshvimpl.h \
 ../../src/shvshvtemplateshvfactory.h

INCLUDEPATH += ../../../SHVRelPathSHV
