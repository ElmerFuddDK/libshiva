include(../../../../shiva/include/shiva.pri)

TEMPLATE = lib

CONFIG += dll \
 debug
CONFIG -= qt \
 release

shivastaticlib { CONFIG += staticlib }

LIBS += $$QMAKE_LIBS_DYNLOAD \
  -L../../../shivalib/linux/libshiva -lshiva

INCLUDEPATH += /usr/include/mysql/

LIBS += -lmysqlclient
TARGET = shivamysql

SOURCES += ../../src/shvmysqlconnectionimpl.cpp \
../../src/shvmysqlresultsetimpl.cpp \
shivamysql.cpp
HEADERS += ../../include/shvmysqlconnectionimpl.h \
../../include/shvmysqlresultsetimpl.h \
../../../include/mysql/shvmysqlconnection.h \
../../../include/mysql/shvmysqlresultset.h
DESTDIR = ../bin
