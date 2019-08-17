include(../../../SHVRelPathSHV/shiva/include/shiva.pri)

TEMPLATE = lib
QT -= core gui qt
CONFIG -= qt

DESTDIR = $$_PRO_FILE_PWD_/../../../SHVRelPathSHV/shiva/$${SHVBUILDFOLDER}/modules
QMAKE_LIBDIR += $$_PRO_FILE_PWD_/../../../SHVRelPathSHV/shiva/$${SHVBUILDFOLDER}

shivastaticlib {
  CONFIG += staticlib
}

LIBS += $$QMAKE_LIBS_DYNLOAD \
  -lshiva

include(../../../SHVRelPathSHV/shiva/include/shiva-postconfig.pri)

INCLUDEPATH += ../../../SHVRelPathSHV

DEF_FILE = $$_PRO_FILE_PWD_/exports.def

SOURCES += ../../src/modules/shvshvtemplateshvimpl.cpp \
  ../../src/shvshvtemplateshvfactory.cpp \
  ../../src/shvshvtemplateshvexports.cpp
HEADERS +=  ../../include/modules/shvshvtemplateshv.h \
  ../../src/modules/shvshvtemplateshvimpl.h \
  ../../src/shvshvtemplateshvfactory.h
