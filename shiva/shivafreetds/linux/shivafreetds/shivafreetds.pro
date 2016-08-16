include(../../../shiva/include/shiva.pri)

TEMPLATE = lib

CONFIG = dll \
debug \
 warn_on
LIBS += $$QMAKE_LIBS_DYNLOAD \
 -lshiva \
 -L../../../../shiva/shiva/shivalib/linux/libshiva

TARGET =../bin/shivafreetds

SOURCES += \
 ../../src/shvshivafreetdsexports.cpp \
 ../../src/shvfreetdswrapperimpl.cpp \
 ../../src/shvfreetdsconnectionimpl.cpp
HEADERS += \
 ../../src/shvfreetdswrapperimpl.h \
 ../../src/shvfreetdsconnectionimpl.h \
 ../../src/shvfreetdscommon.h

INCLUDEPATH += ../../.. ../..

LIBS += -lsybdb
