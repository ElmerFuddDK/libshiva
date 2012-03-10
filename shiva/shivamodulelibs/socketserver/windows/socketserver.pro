#-------------------------------------------------
#
# Project created by QtCreator 2010-06-29T15:14:45
#
#-------------------------------------------------

QT       -= core gui

LIBS += -L. \
        -lshiva
QMAKE_PRE_LINK = copydeps.bat

TARGET = socketserver
TEMPLATE = lib

HEADERS += ../include/shvsocketimpl.h \
           ../include/shvsocketserverimpl.h \
           ../../../include/modules/socketserver/shvsocket.h \
           ../../../include/modules/socketserver/shvsocketserver.h \
           ../include/shvsocketserverthread.h \
           ../src/shvsocketserverfactory.h
SOURCES += ../src/shvsocketimpl.cpp \
           ../src/shvsocketserverimpl.cpp \
           ../src/shvsocketserverthread.cpp \
           ../src/shvsocketserverfactory.cpp \
           ../src/shvsocketserverexports.cpp \
           ../src/shvsslsocket.cpp


DEF_FILE = socketserver.def

LIBS += -lws2_32

QMAKE_CXXFLAGS_DEBUG += -DDEBUG 
