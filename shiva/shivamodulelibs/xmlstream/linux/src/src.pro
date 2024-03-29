include(../../../../../shiva/include/shiva.pri)

SOURCES =xmlstream.cpp \
../../src/shvxmlreaderimpl.cpp \
../../src/shvxmlstreamfactory.cpp \
../../src/shvxmlstreamimpl.cpp \
../../src/shvxmlwriterimpl.cpp \
../../src/expat/xmlparse.c \
../../src/expat/xmlrole.c \
../../src/expat/xmltok.c \
../../src/expat/xmltok_ns.c
TARGET =../bin/xmlstream
TEMPLATE = lib

CONFIG += dll \
debug
CONFIG -= qt \
thread \
release
shivastaticlib {
  CONFIG += staticlib shivastaticlib
}
LIBS += $$QMAKE_LIBS_DYNLOAD \
  -L../../../../shivalib/linux/libshiva -lshiva
HEADERS += ../../include/shvxmlreaderimpl.h \
../../include/shvxmlstreamimpl.h \
../../include/shvxmlwriterimpl.h \
../../src/shvxmlstreamfactory.h \
../../src/expat/ascii.h \
../../src/expat/asciitab.h \
../../src/expat/expat.h \
../../src/expat/iasciitab.h \
../../src/expat/internal.h \
../../src/expat/latin1tab.h \
../../src/expat/nametab.h \
../../src/expat/utf8tab.h \
../../src/expat/winconfig.h \
../../src/expat/xmlrole.h \
../../src/expat/xmltok.h \
../../src/expat/xmltok_impl.h

DEFINES += _STATIC
SOURCES -= ../../src/expat/xmltok_impl.c \
../../src/expat/xmltok_ns.c

!isEmpty(ANDROID_PLATFORM) {
  QMAKE_CFLAGS += -fno-exceptions
  QMAKE_CXXFLAGS += -fno-exceptions -fno-rtti
  QMAKE_LIBS += -llog
  QMAKE_LIBS_PRIVATE -= -lgnustl_shared
  QMAKE_INCDIR -= $$ANDROID_SOURCES_CXX_STL_INCDIR
  QMAKE_LIBDIR -= $$ANDROID_SOURCES_CXX_STL_LIBDIR
}
