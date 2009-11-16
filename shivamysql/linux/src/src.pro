TEMPLATE = lib

CONFIG += dll \
 debug
CONFIG -= qt \
 release

INCLUDEPATH += /usr/include/mysql/

LIBS += -L/usr/lib \
-L../../../shivalib/linux/libshiva/ \
-lmysqlclient \
-lshiva
TARGET = shivamysql

SOURCES += ../../src/shvmysqlconnectionimpl.cpp \
../../src/shvmysqlresultsetimpl.cpp \
shivamysql.cpp
HEADERS += ../../include/shvmysqlconnectionimpl.h \
../../include/shvmysqlresultsetimpl.h \
../../../include/mysql/shvmysqlconnection.h \
../../../include/mysql/shvmysqlresultset.h
DESTDIR = ../bin

QMAKE_CXXFLAGS_DEBUG += -DDEBUG

