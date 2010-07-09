#-------------------------------------------------
#
# Project created by QtCreator 2010-06-29T15:14:45
#
#-------------------------------------------------

QT       -= core gui

LIBS += -L. \
        -lshiva
QMAKE_PRE_LINK = copydeps.bat

TARGET = xmlstream
TEMPLATE = lib

HEADERS = ../include/shvxmlreaderimpl.h \
          ../include/shvxmlstreamimpl.h \
          ../include/shvxmlwriterimpl.h \
          ../src/shvxmlstreamfactory.h \
          ../src/expat/ascii.h \
          ../src/expat/asciitab.h \
          ../src/expat/expat.h \
          ../src/expat/iasciitab.h \
          ../src/expat/internal.h \
          ../src/expat/latin1tab.h \
          ../src/expat/nametab.h \
          ../src/expat/utf8tab.h \
          ../src/expat/winconfig.h \
          ../src/expat/xmlrole.h \
          ../src/expat/xmltok.h \
          ../src/expat/xmltok_impl.h
SOURCES = xmlstream.cpp \
          ../src/shvxmlreaderimpl.cpp \
          ../src/shvxmlstreamfactory.cpp \
          ../src/shvxmlstreamimpl.cpp \
          ../src/shvxmlwriterimpl.cpp \
          ../src/expat/xmlparse.c \
          ../src/expat/xmlrole.c \
          ../src/expat/xmltok.c \
          ../src/expat/xmltok_ns.c


DEF_FILE = xmlstream.def

QMAKE_CXXFLAGS_DEBUG += -DDEBUG
QMAKE_CXXFLAGS += -D_STATIC -D_UNICODE
QMAKE_CFLAGS += -D_STATIC -D_UNICODE
