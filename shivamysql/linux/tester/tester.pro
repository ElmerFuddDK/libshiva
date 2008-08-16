SOURCES += main.cpp

TEMPLATE = app

INCLUDEPATH += /usr/include/mysql/

LIBS += -L/usr/lib \
  -L../../../shivalib/linux/libshiva \
  -lmysqlclient \
  -lshiva \
  -ldl

DESTDIR = ../bin

QMAKE_CXXFLAGS_DEBUG += -DDEBUG

DEFINES += __SHVSTRING_INCLUDE_UNICODE

CONFIG -= release \
 qt

CONFIG += debug

