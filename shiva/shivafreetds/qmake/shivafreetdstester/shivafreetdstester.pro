include(../../../../shiva/include/shiva.pri)

TEMPLATE = app
QT -= core gui qt
isEmpty(CE_SDK) { CONFIG += console }
CONFIG -= qt

DESTDIR = $$_PRO_FILE_PWD_/../../../$${SHVBUILDFOLDER}
QMAKE_LIBDIR += $$_PRO_FILE_PWD_/../../../$${SHVBUILDFOLDER}

shivastaticlib {
  LIBS += -lshivafreetds -lsybdb
  !msvc { LIBS += -lpthread }
} else {
  unix {
    QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN\''
  }
}

LIBS += $$QMAKE_LIBS_DYNLOAD \
        -lshiva

include(../../../../shiva/include/shiva-postconfig.pri)

INCLUDEPATH += ../../../..

SOURCES += ../../src/tester/main.cpp \
  ../../src/tester/shvfreetdstester.cpp
