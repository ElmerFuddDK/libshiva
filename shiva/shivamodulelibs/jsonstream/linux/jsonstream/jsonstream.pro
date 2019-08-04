include(../../../../../shiva/include/shiva.pri)

ios {
  CONFIG -= qt
  CONFIG += dll staticlib
} else {
  shivastaticlib {
    CONFIG = $$QMAKE_PLATFORM \
           debug \
           warn_on \
           dll staticlib shivastaticlib
  } else {
    CONFIG = $$QMAKE_PLATFORM \
           debug \
           warn_on \
           dll
  }
}
TEMPLATE = lib
LIBS += $$QMAKE_LIBS_DYNLOAD \
  -L../../../../shivalib/linux/libshiva -lshiva

TARGET =../bin/jsonstream

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
