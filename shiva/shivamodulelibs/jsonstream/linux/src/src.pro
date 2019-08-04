include(../../../../../shiva/include/shiva.pri)

shivastaticlib { LIBS += -lpthread
  LIBS += -L../bin -ljsonstream
}

SOURCES = ../../src/tester/main.cpp \
../../src/tester/shvjsonstreamtester.cpp
HEADERS += ../../src/tester/shvjsonstreamtester.h
TARGET =../bin/jsonstreamtester
TEMPLATE = app
LIBS += $$QMAKE_LIBS_DYNLOAD \
  -L../../../../shivalib/linux/libshiva -lshiva

CONFIG -= release \
 qt

CONFIG += debug
!shivastaticlib {
  QMAKE_POST_LINK = ../bin/copydeps.sh
}

INCLUDEPATH += ../../../../..
