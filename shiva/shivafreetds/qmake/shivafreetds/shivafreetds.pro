include(../../../../shiva/include/shiva.pri)

TEMPLATE = lib
QT -= core gui qt
CONFIG -= qt

DESTDIR = $$_PRO_FILE_PWD_/../../../$${SHVBUILDFOLDER}
QMAKE_LIBDIR += $$_PRO_FILE_PWD_/../../../$${SHVBUILDFOLDER}

shivastaticlib {
  CONFIG += staticlib
}

LIBS += $$QMAKE_LIBS_DYNLOAD \
  -lshiva \
  -lpthread

include(../../../../shiva/include/shiva-postconfig.pri)

INCLUDEPATH += ../../../..

DEF_FILE = $$_PRO_FILE_PWD_/exports.def

LIBS += -lsybdb

DEF_FILE = shivafreetds.def

SOURCES += ../../src/shvshivafreetdsexports.cpp \
  ../../src/shvfreetdswrapperimpl.cpp \
  ../../src/shvfreetdsconnectionimpl.cpp \
  ../../src/shvfreetdstransactionimpl.cpp
HEADERS += ../../src/shvfreetdswrapperimpl.h \
  ../../src/shvfreetdsconnectionimpl.h \
  ../../src/shvfreetdstransactionimpl.h \
  ../../src/shvfreetdscommon.h

win32 {
  INCLUDEPATH += freetds/include
  LIBS += -Lfreetds/lib
}
