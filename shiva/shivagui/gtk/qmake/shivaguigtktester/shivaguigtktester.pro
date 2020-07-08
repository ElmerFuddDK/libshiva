include(../../../../../shiva/include/shiva.pri)

TEMPLATE = app
QT -= core gui qt
CONFIG -= qt

DESTDIR = $$_PRO_FILE_PWD_/../../../../$${SHVBUILDFOLDER}
QMAKE_LIBDIR += $$_PRO_FILE_PWD_/../../../../$${SHVBUILDFOLDER}

shivastaticlib {
  !msvc { LIBS += -lpthread }
  LIBS += -lshivaguigtk
} else {
  unix {
    QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN\''
  }
}

LIBS += $$QMAKE_LIBS_DYNLOAD \
        -lshiva

include(../../../../../shiva/include/shiva-postconfig.pri)

INCLUDEPATH += ../../../../..

SOURCES = ../../../src/tester/main.cpp \
  ../../../src/tester/modules/shvcontroltester.cpp \
  ../../../src/tester/modules/shvformtest.cpp \
  ../../../src/tester/modules/shvtabtestform.cpp \
  ../../../src/tester/modules/shvtabtestpageform.cpp
HEADERS += ../../../src/tester/modules/shvcontroltester.h \
  ../../../src/tester/modules/shvformtest.h \
  ../../../src/tester/modules/shvtabtestform.h \
  ../../../src/tester/modules/shvtabtestpageform.h

DEFINES += __SHIVA_GTK

shivastaticlib {
  unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += gtk+-x11-2.0
  }
}
