include(../../../../../shiva/include/shiva.pri)

TEMPLATE = lib

CONFIG = dll \
debug \
 warn_on
LIBS += $$QMAKE_LIBS_DYNLOAD \
 -lshiva \
 -L../../../../shivalib/linux/libshiva

TARGET =../bin/subprocess

SOURCES += ../../src/modules/shvsubprocessimpl.cpp \
../../src/modules/shvsubprocessserverimpl.cpp \
 ../../src/shvsubprocessfactory.cpp \
 ../../src/shvsubprocessexports.cpp
HEADERS += ../../../../include/modules/subprocess/shvsubprocess.h \
 ../../src/modules/shvsubprocessimpl.h \
 ../../src/modules/shvsubprocessserverimpl.h \
 ../../src/shvsubprocessfactory.h

INCLUDEPATH += ../../../../..

!isEmpty(ANDROID_PLATFORM) {
  QMAKE_CFLAGS += -fno-exceptions -fno-rtti
  QMAKE_CXXFLAGS += -fno-exceptions -fno-rtti
  QMAKE_LIBS = -lstdc++ -lsupc++ -llog
  QMAKE_INCDIR -= $$ANDROID_SOURCES_CXX_STL_INCDIR
  QMAKE_LIBDIR -= $$ANDROID_SOURCES_CXX_STL_LIBDIR
}
