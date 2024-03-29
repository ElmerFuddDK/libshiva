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
LIBS += $$QMAKE_LIBS_DYNLOAD
!shivastaticlib { LIBS += -L../../../../shivalib/linux/libshiva -lshiva }

SOURCES += ../../src/shvsocketimpl.cpp \
../../src/shvsocketserverimpl.cpp \
 ../../src/shvsocketserverthread.cpp \
 ../../src/shvsocketserverfactory.cpp \
 ../../src/shvsocketserverexports.cpp \
    ../../src/shvsslsocket.cpp
HEADERS += ../../include/shvsocketimpl.h \
../../include/shvsocketserverimpl.h \
../../../../include/modules/socketserver/shvsocket.h \
../../../../include/modules/socketserver/shvsocketserver.h \
 ../../include/shvsocketserverthread.h \
 ../../src/shvsocketserverfactory.h \
	../../include/shvsslsocket.h

!isEmpty(ANDROID_PLATFORM) {
  QMAKE_CFLAGS += -fno-exceptions
  QMAKE_CXXFLAGS += -fno-exceptions -fno-rtti
  QMAKE_LIBS += -llog
  QMAKE_LIBS_PRIVATE -= -lgnustl_shared
  QMAKE_INCDIR -= $$ANDROID_SOURCES_CXX_STL_INCDIR
  QMAKE_LIBDIR -= $$ANDROID_SOURCES_CXX_STL_LIBDIR
}
