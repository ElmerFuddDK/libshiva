include(../../../SHVRelPathSHV/shiva/include/shiva.pri)

TEMPLATE = app
QT -= core gui qt
isEmpty(CE_SDK) { CONFIG += console }
CONFIG -= qt

DESTDIR = $$_PRO_FILE_PWD_/../../../SHVRelPathSHV/shiva/$${SHVBUILDFOLDER}
QMAKE_LIBDIR += $$_PRO_FILE_PWD_/../../../SHVRelPathSHV/shiva/$${SHVBUILDFOLDER}

shivastaticlib {
  LIBS += -lpthread
} else {
  unix {
    QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN\''
  }
}

LIBS += $$QMAKE_LIBS_DYNLOAD \
        -lshiva

include(../../../SHVRelPathSHV/shiva/include/shiva-postconfig.pri)

INCLUDEPATH += ../../../SHVRelPathSHV

SOURCES = ../../src/main.cpp \
  ../../src/shvshvtemplateshv.cpp
HEADERS += ../../src/shvshvtemplateshv.h
