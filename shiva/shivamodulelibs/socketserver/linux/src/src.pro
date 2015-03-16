include(../../../../../shiva/include/shiva.pri)

SOURCES =socketserver.cpp \
shveventstdin.cpp
TARGET =../bin/socketserver
TEMPLATE = app
LIBS += -L../../../../shivalib/linux/libshiva \
$$QMAKE_LIBS_DYNLOAD \
-lshiva

CONFIG -= release \
 qt

CONFIG += debug

SOURCES -= shvmainthreadeventdispatcherconsole.cpp \
shveventstdin.cpp

!isEmpty(ANDROID_PLATFORM) {
  QMAKE_CFLAGS += -fno-exceptions
  QMAKE_CXXFLAGS += -fno-exceptions -fno-rtti
  QMAKE_LIBS = -lstdc++ -lsupc++ -llog
  QMAKE_INCDIR -= $$ANDROID_SOURCES_CXX_STL_INCDIR
  QMAKE_LIBDIR -= $$ANDROID_SOURCES_CXX_STL_LIBDIR
}
