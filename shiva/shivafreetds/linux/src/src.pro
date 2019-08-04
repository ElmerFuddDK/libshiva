include(../../../../shiva/include/shiva.pri)

SOURCES = ../../src/tester/main.cpp \
    ../../src/tester/shvfreetdstester.cpp

HEADERS += \
    ../../src/tester/shvfreetdstester.h

TARGET =../bin/shivafreetdstester
TEMPLATE = app

shivastaticlib {
  LIBS += -lpthread -lsybdb
  LIBS += -L../bin -lshivafreetds
}
LIBS += $$QMAKE_LIBS_DYNLOAD \
  -L../../../../shiva/shivalib/linux/libshiva -lshiva

CONFIG -= release \
 qt

CONFIG += debug

!shivastaticlib {
  QMAKE_POST_LINK = ../bin/copydeps.sh
}

QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN\''

INCLUDEPATH += ../../../..
