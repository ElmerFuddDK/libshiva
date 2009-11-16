TEMPLATE = lib

CONFIG = dll \
debug \
 warn_on
LIBS += -ldl \
-lshiva \
 -L../../../../shivalib/linux/libshiva

QMAKE_CXXFLAGS_DEBUG += -DDEBUG

SOURCES += ../../src/shvsocketimpl.cpp \
../../src/shvsocketserverimpl.cpp \
 ../../src/shvsocketserverthread.cpp \
 ../../src/shvsocketserverfactory.cpp \
 ../../src/shvsocketserverexports.cpp
HEADERS += ../../include/shvsocketimpl.h \
../../include/shvsocketserverimpl.h \
../../../../include/modules/socketserver/shvsocket.h \
../../../../include/modules/socketserver/shvsocketserver.h \
 ../../include/shvsocketserverthread.h \
 ../../src/shvsocketserverfactory.h
