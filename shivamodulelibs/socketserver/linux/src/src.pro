SOURCES =socketserver.cpp \
shveventstdin.cpp
TARGET =../bin/socketserver
TEMPLATE = app
LIBS += -L../../../../shivalib/linux/libshiva \
-ldl \
-lshiva
QMAKE_CXXFLAGS_DEBUG += -DDEBUG

DEFINES += __SHVSTRING_INCLUDE_UNICODE

CONFIG -= release \
 qt

CONFIG += debug

SOURCES -= shvmainthreadeventdispatcherconsole.cpp \
shveventstdin.cpp
