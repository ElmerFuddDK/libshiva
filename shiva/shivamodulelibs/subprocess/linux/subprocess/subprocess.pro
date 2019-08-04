include(../../../../../shiva/include/shiva.pri)

TEMPLATE = lib

ios {
  CONFIG -= qt
  CONFIG += dll staticlib
} else {
  shivastaticlib {
    CONFIG = $$QMAKE_PLATFORM \
           debug \
           warn_on \
           dll staticlib shivastaticlib
  } else {
    CONFIG = $$QMAKE_PLATFORM \
           debug \
           warn_on \
           dll
  }
}

LIBS += $$QMAKE_LIBS_DYNLOAD \
  -L../../../../shivalib/linux/libshiva -lshiva

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
  QMAKE_CFLAGS += -fno-exceptions
  QMAKE_CXXFLAGS += -fno-exceptions -fno-rtti
  QMAKE_LIBS += -llog
  QMAKE_LIBS_PRIVATE -= -lgnustl_shared
  QMAKE_INCDIR -= $$ANDROID_SOURCES_CXX_STL_INCDIR
  QMAKE_LIBDIR -= $$ANDROID_SOURCES_CXX_STL_LIBDIR
}
