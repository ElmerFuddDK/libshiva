include(../../../../shiva/include/shiva.pri)

SOURCES += main.cpp

TEMPLATE = app

shivastaticlib {
  LIBS += -lpthread
  LIBS += -lmysqlclient
  LIBS += -L../bin -lshivamysql
}
LIBS += $$QMAKE_LIBS_DYNLOAD \
  -L../../../shivalib/linux/libshiva -lshiva

DESTDIR = ../bin

CONFIG -= release \
 qt

CONFIG += debug

