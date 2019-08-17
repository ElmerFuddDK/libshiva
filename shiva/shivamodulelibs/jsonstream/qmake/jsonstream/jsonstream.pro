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

SOURCES += ../../src/jsonparser/JSON_parser.c \
  ../../src/modules/shvjsonwriterimpl.cpp \
  ../../src/modules/shvjsonstreamimpl.cpp \
  ../../src/shvjsonstreamfactory.cpp \
  ../../src/shvjsonstreamexports.cpp \
  ../../src/modules/shvjsonreaderimpl.cpp
HEADERS +=  ../../../../include/modules/jsonstream/shvjsonstream.h \
  ../../../../include/modules/jsonstream/shvjsonwriter.h \
  ../../../../include/modules/jsonstream/shvjsonreader.h \
  ../../src/jsonparser/JSON_parser.h \
  ../../src/modules/shvjsonwriterimpl.h \
  ../../src/modules/shvjsonstreamimpl.h \
  ../../src/shvjsonstreamfactory.h \
  ../../src/modules/shvjsonreaderimpl.h
