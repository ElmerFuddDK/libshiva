include(../../../../shiva/include/shiva.pri)

SOURCES = ../../src/shvshivasqliteexports.cpp \
../../src/sqlite/sqlite3.c \
../../src/sqlitestatementimpl.cpp \
../../src/sqlitewrapperimpl.cpp
TARGET =shivasqlite
DESTDIR =../bin

LIBS += -L../../../shivalib/linux/libshiva \
  $$QMAKE_LIBS_DYNLOAD \
  -lshiva \
  -lpthread

!isEmpty(ANDROID_PLATFORM) {
  LIBS -= -lpthread
}

CONFIG -= release \
qt
CONFIG += debug \
dll
TEMPLATE = lib

shivastaticlib { CONFIG += staticlib }

SQLITE_FTS = -DSQLITE_ENABLE_FTS3 -DSQLITE_ENABLE_FTS3_PARENTHESIS

HEADERS += ../../../include/sqlite/shvstringsqlite.h \
../../../include/sqlite/sqlitestatement.h \
../../../include/sqlite/sqlitewrapper.h \
../../include/sqlitestatementimpl.h \
../../include/sqlitewrapperimpl.h

QMAKE_CFLAGS += $$SQLITE_FTS
QMAKE_CXXFLAGS += $$SQLITE_FTS -DSHIVASQLITE_EXPORTS

!isEmpty(ANDROID_PLATFORM) {
  QMAKE_CFLAGS += -fno-exceptions
  QMAKE_CXXFLAGS += -fno-exceptions -fno-rtti
  QMAKE_LIBS += -llog
  QMAKE_LIBS_PRIVATE -= -lgnustl_shared
  QMAKE_INCDIR -= $$ANDROID_SOURCES_CXX_STL_INCDIR
  QMAKE_LIBDIR -= $$ANDROID_SOURCES_CXX_STL_LIBDIR
}
