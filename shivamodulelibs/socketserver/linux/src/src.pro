SOURCES =socketserver.cpp \
shveventstdin.cpp
TARGET =../bin/socketserver
TEMPLATE = app
LIBS += -L../../../../shivalib/linux/libshiva \
-ldl \
-lshiva
QMAKE_CXXFLAGS_DEBUG += -DDEBUG

CONFIG -= release \
 qt

CONFIG += debug

SOURCES -= shvmainthreadeventdispatcherconsole.cpp \
shveventstdin.cpp
