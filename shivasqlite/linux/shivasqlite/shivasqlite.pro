SOURCES =shivasqlite.cpp \
../../src/sqlite/sqlite3.c \
../../src/sqlitestatementimpl.cpp \
../../src/sqlitewrapperimpl.cpp
TARGET =shivasqlite
DESTDIR =../bin

LIBS += -L../../../shivalib/linux/libshiva \
  -ldl \
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

HEADERS += ../../../include/sqlite/shvstringsqlite.h \
../../../include/sqlite/sqlitestatement.h \
../../../include/sqlite/sqlitewrapper.h \
../../include/sqlitestatementimpl.h \
../../include/sqlitewrapperimpl.h
QMAKE_CXXFLAGS_DEBUG += -DDEBUG \
  -DSHIVASQLITE_EXPORTS

QMAKE_CXXFLAGS_RELEASE += -DSHIVASQLITE_EXPORTS

!isEmpty(ANDROID_PLATFORM) {
  QMAKE_CFLAGS += -fno-exceptions
  QMAKE_CXXFLAGS += -fno-exceptions -fno-rtti
  QMAKE_LIBS = -lstdc++ -lsupc++ -llog
  QMAKE_INCDIR -= $$ANDROID_SOURCES_CXX_STL_INCDIR
  QMAKE_LIBDIR -= $$ANDROID_SOURCES_CXX_STL_LIBDIR
}
