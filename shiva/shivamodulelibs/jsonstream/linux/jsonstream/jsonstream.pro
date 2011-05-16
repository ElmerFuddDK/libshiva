TEMPLATE = lib

CONFIG = dll \
debug \
 warn_on
LIBS += $$QMAKE_LIBS_DYNLOAD \
 -lshiva \
 -L../../../../shivalib/linux/libshiva

TARGET =../bin/jsonstream

QMAKE_CXXFLAGS_DEBUG += -DDEBUG

SOURCES += \
    ../../src/jsonparser/JSON_parser.c \
    ../../src/modules/shvjsonwriterimpl.cpp \
    ../../src/modules/shvjsonstreamimpl.cpp \
	../../src/shvjsonstreamfactory.cpp \
	../../src/shvjsonstreamexports.cpp \
    ../../src/modules/shvjsonreaderimpl.cpp

HEADERS += \
    ../../../../include/modules/jsonstream/shvjsonstream.h \
    ../../../../include/modules/jsonstream/shvjsonwriter.h \
    ../../../../include/modules/jsonstream/shvjsonreader.h \
    ../../src/jsonparser/JSON_parser.h \
    ../../src/modules/shvjsonwriterimpl.h \
    ../../src/modules/shvjsonstreamimpl.h \
	../../src/shvjsonstreamfactory.h \
    ../../src/modules/shvjsonreaderimpl.h

INCLUDEPATH += ../../../../..
