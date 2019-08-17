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

SOURCES += ../../src/modules/shvsubprocessimpl.cpp \
  ../../src/modules/shvsubprocessserverimpl.cpp \
  ../../src/shvsubprocessfactory.cpp \
  ../../src/shvsubprocessexports.cpp
HEADERS += ../../../../include/modules/subprocess/shvsubprocess.h \
  ../../src/modules/shvsubprocessimpl.h \
  ../../src/modules/shvsubprocessserverimpl.h \
  ../../src/shvsubprocessfactory.h
