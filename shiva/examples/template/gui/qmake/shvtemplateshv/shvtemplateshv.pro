include(../../../SHVRelPathSHV/shiva/include/shiva.pri)

TEMPLATE = app
QT -= core gui qt
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

RC_FILE = ../../src/win32/shvtemplateshv.rc

SOURCES = ../../src/main.cpp \
  ../../src/shvshvtemplateshv.cpp \
  ../../src/forms/shvformmain.cpp
HEADERS += ../../src/shvshvtemplateshv.h \
  ../../src/forms/shvformmain.h 
