SOURCES += main.cpp

TEMPLATE = app

INCLUDEPATH += /usr/include/mysql/

LIBS += -L/usr/lib \
  -L../../../shivalib/linux/libshiva \
  -lmysqlclient \
  -lshiva \
  $$QMAKE_LIBS_DYNLOAD

DESTDIR = ../bin

QMAKE_CXXFLAGS_DEBUG += -DDEBUG

CONFIG -= release \
 qt

CONFIG += debug

