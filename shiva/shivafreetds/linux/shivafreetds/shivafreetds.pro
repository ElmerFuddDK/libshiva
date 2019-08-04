include(../../../../shiva/include/shiva.pri)

TEMPLATE = lib

CONFIG -= release \
qt
CONFIG += debug \
dll

shivastaticlib { CONFIG += staticlib }

LIBS += $$QMAKE_LIBS_DYNLOAD \
  -L../../../../shiva/shivalib/linux/libshiva -lshiva

TARGET =../bin/shivafreetds

SOURCES += \
 ../../src/shvshivafreetdsexports.cpp \
 ../../src/shvfreetdswrapperimpl.cpp \
 ../../src/shvfreetdsconnectionimpl.cpp \
 ../../src/shvfreetdstransactionimpl.cpp
HEADERS += \
 ../../src/shvfreetdswrapperimpl.h \
 ../../src/shvfreetdsconnectionimpl.h \
 ../../src/shvfreetdstransactionimpl.h \
 ../../src/shvfreetdscommon.h

INCLUDEPATH += ../../../..

LIBS += -lsybdb
