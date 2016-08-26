include(../../../shiva/include/shiva.pri)

TEMPLATE = lib

CONFIG = dll \
debug \
 warn_on
LIBS += $$QMAKE_LIBS_DYNLOAD \
 -lshiva \
 -L.
QMAKE_PRE_LINK = copydeps.bat

SOURCES += \
 ../src/shvshivafreetdsexports.cpp \
 ../src/shvfreetdswrapperimpl.cpp \
 ../src/shvfreetdsconnectionimpl.cpp \
 ../src/shvfreetdstransactionimpl.cpp
HEADERS += \
 ../src/shvfreetdswrapperimpl.h \
 ../src/shvfreetdsconnectionimpl.h \
 ../src/shvfreetdstransactionimpl.h \
 ../src/shvfreetdscommon.h

INCLUDEPATH += ../../.. freetds/include

LIBS += -lsybdb -Lfreetds/lib

DEF_FILE = shivafreetds.def
