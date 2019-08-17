include(../../../../../shiva/include/shiva.pri)

TEMPLATE = lib
QT -= core gui qt
CONFIG -= qt

DESTDIR = $$_PRO_FILE_PWD_/../../../../$${SHVBUILDFOLDER}/modules
QMAKE_LIBDIR += $$_PRO_FILE_PWD_/../../../../$${SHVBUILDFOLDER}

shivastaticlib {
  CONFIG += staticlib
}

LIBS += $$QMAKE_LIBS_DYNLOAD \
  -lshiva

include(../../../../../shiva/include/shiva-postconfig.pri)

INCLUDEPATH += ../../../../..

DEF_FILE = $$_PRO_FILE_PWD_/exports.def

SOURCES = ../../src/shvxmlstreamexports.cpp \
  ../../src/shvxmlreaderimpl.cpp \
  ../../src/shvxmlstreamfactory.cpp \
  ../../src/shvxmlstreamimpl.cpp \
  ../../src/shvxmlwriterimpl.cpp \
  ../../src/expat/xmlparse.c \
  ../../src/expat/xmlrole.c \
  ../../src/expat/xmltok.c
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
