include(../../../shiva/include/shiva.pri)

QT       -= core gui

# Use Visual studio compiled libs
CONFIG += shivafreetdsvslib

LIBS += -L. \
        -lshiva
QMAKE_PRE_LINK = copydeps.bat

TARGET = shivafreetds
TEMPLATE = lib

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


shivafreetdsvslib {
  INCLUDEPATH += ../../.. freetds-vs/include
  LIBS += -lsybdb -Lfreetds-vs/lib
  DEFINES += __SHIVA_FREETDSVSLIBHACK
} else {
  INCLUDEPATH += ../../.. freetds/include
  LIBS += -lsybdb -Lfreetds/lib
}

DEF_FILE = shivafreetds.def
