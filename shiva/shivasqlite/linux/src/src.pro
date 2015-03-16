include(../../../../shiva/include/shiva.pri)

SOURCES += tester.cpp

TEMPLATE = app

TARGET = sqlitetester

LIBS += -L../../../shivalib/linux/libshiva \
  -lshiva \
  -ldl

DESTDIR = ../bin

CONFIG -= release \
 qt

CONFIG += debug

!isEmpty(ANDROID_PLATFORM) {
  QMAKE_CFLAGS += -fno-exceptions
  QMAKE_CXXFLAGS += -fno-exceptions -fno-rtti
  QMAKE_LIBS = -lstdc++ -lsupc++ -llog
  QMAKE_INCDIR -= $$ANDROID_SOURCES_CXX_STL_INCDIR
  QMAKE_LIBDIR -= $$ANDROID_SOURCES_CXX_STL_LIBDIR
}
