SOURCES += tester.cpp

TEMPLATE = app

TARGET = sqlitetester

LIBS += -L../../../shivalib/linux/libshiva \
  -lshiva \
  -ldl

DESTDIR = ../bin

QMAKE_CXXFLAGS_DEBUG += -DDEBUG

CONFIG -= release \
 qt

CONFIG += debug

