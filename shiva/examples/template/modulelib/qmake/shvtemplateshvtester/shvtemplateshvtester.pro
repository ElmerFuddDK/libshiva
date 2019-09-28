include(../../../SHVRelPathSHV/shiva/include/shiva.pri)

TEMPLATE = app
QT -= core gui qt
isEmpty(CE_SDK) { CONFIG += console }
CONFIG -= qt

DESTDIR = $$_PRO_FILE_PWD_/../../../SHVRelPathSHV/shiva/$${SHVBUILDFOLDER}
QMAKE_LIBDIR += $$_PRO_FILE_PWD_/../../../SHVRelPathSHV/shiva/$${SHVBUILDFOLDER}

shivastaticlib {
  LIBS += -lpthread
  LIBS += -L$$_PRO_FILE_PWD_/../../../SHVRelPathSHV/shiva/$${SHVBUILDFOLDER}/modules -lshvtemplateshv
} else {
  unix {
    QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN\''
  }
}

LIBS += $$QMAKE_LIBS_DYNLOAD \
        -lshiva

include(../../../SHVRelPathSHV/shiva/include/shiva-postconfig.pri)

INCLUDEPATH += ../../../SHVRelPathSHV

SOURCES = ../../src/tester/main.cpp \
  ../../src/tester/shvshvtemplateshvtester.cpp
HEADERS += ../../src/tester/shvshvtemplateshvtester.h
