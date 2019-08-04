include(../../../../../shiva/include/shiva.pri)

SOURCES = ../../src/tester/main.cpp \
../../src/tester/shvsubprocesstester.cpp
HEADERS += ../../src/tester/shvsubprocesstester.h
TARGET =../bin/subprocesstester
TEMPLATE = app
LIBS += -L../../../../shivalib/linux/libshiva \
$$QMAKE_LIBS_DYNLOAD \
-lshiva

CONFIG -= release \
 qt
CONFIG += debug
shivastaticlib {
  LIBS += -lpthread
  LIBS += -L../bin -lsubprocess
}

!shivastaticlib {
  QMAKE_POST_LINK = ../bin/copydeps.sh
}

LIBS += $$QMAKE_LIBS_DYNLOAD \
  -L../../../../shivalib/linux/libshiva -lshiva
INCLUDEPATH += ../../../../..

!isEmpty(ANDROID_PLATFORM) {
  QMAKE_CFLAGS += -fno-exceptions
  QMAKE_CXXFLAGS += -fno-exceptions -fno-rtti
  QMAKE_LIBS += -llog
  QMAKE_LIBS_PRIVATE -= -lgnustl_shared
  QMAKE_INCDIR -= $$ANDROID_SOURCES_CXX_STL_INCDIR
  QMAKE_LIBDIR -= $$ANDROID_SOURCES_CXX_STL_LIBDIR
}
