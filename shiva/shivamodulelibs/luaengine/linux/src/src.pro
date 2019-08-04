include(../../../../../shiva/include/shiva.pri)

SOURCES = ../../src/tester/main.cpp \
../../src/tester/shvluaenginetester.cpp
HEADERS += ../../src/tester/shvluaenginetester.h
TARGET =../bin/luaenginetester
TEMPLATE = app

shivastaticlib {
  LIBS += -lpthread
  LIBS += -L../bin -lluaengine
}

LIBS += $$QMAKE_LIBS_DYNLOAD \
  -L../../../../../shiva/shivalib/linux/libshiva -lshiva

CONFIG -= release \
 qt

CONFIG += debug
!shivastaticlib {
  QMAKE_POST_LINK = ../bin/copydeps.sh
}

INCLUDEPATH += ../../../../..

QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN\''

!isEmpty(ANDROID_PLATFORM) {
  QMAKE_CFLAGS += -fno-exceptions
  QMAKE_CXXFLAGS += -fno-exceptions -fno-rtti
  QMAKE_LIBS += -llog
  QMAKE_LIBS_PRIVATE -= -lgnustl_shared
  QMAKE_INCDIR -= $$ANDROID_SOURCES_CXX_STL_INCDIR
  QMAKE_LIBDIR -= $$ANDROID_SOURCES_CXX_STL_LIBDIR
}
