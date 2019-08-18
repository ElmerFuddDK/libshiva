include(../../../../../shiva/include/shiva.pri)

TEMPLATE = lib
QT -= core gui qt
CONFIG -= qt

DESTDIR = $$_PRO_FILE_PWD_/../../../../$${SHVBUILDFOLDER}/modules
QMAKE_LIBDIR += $$_PRO_FILE_PWD_/../../../../$${SHVBUILDFOLDER}

shivastaticlib {
  CONFIG += staticlib
}

LIBS += $$QMAKE_LIBS_DYNLOAD \
  -lshiva

include(../../../../../shiva/include/shiva-postconfig.pri)

DEF_FILE = $$_PRO_FILE_PWD_/exports.def

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

win32 {
  DEFINES += _WIN32_WINNT=0x0501
  LIBS += -lws2_32
}
