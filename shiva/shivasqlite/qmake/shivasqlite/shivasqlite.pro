include(../../../../shiva/include/shiva.pri)

TEMPLATE = lib
QT -= core gui qt
CONFIG -= qt

DESTDIR = $$_PRO_FILE_PWD_/../../../$${SHVBUILDFOLDER}
QMAKE_LIBDIR += $$_PRO_FILE_PWD_/../../../$${SHVBUILDFOLDER}

shivastaticlib {
  CONFIG += staticlib
}

LIBS += $$QMAKE_LIBS_DYNLOAD \
  -lshiva

include(../../../../shiva/include/shiva-postconfig.pri)

DEF_FILE = $$_PRO_FILE_PWD_/exports.def

SOURCES += ../../src/shvshivasqliteexports.cpp \
  ../../src/sqlite/sqlite3.c \
  ../../src/sqlitestatementimpl.cpp \
  ../../src/sqlitewrapperimpl.cpp

HEADERS += ../../../include/sqlite/shvstringsqlite.h \
  ../../../include/sqlite/sqlitestatement.h \
  ../../../include/sqlite/sqlitewrapper.h \
  ../../include/sqlitestatementimpl.h \
  ../../include/sqlitewrapperimpl.h

SQLITE_FTS = -DSQLITE_ENABLE_FTS3 -DSQLITE_ENABLE_FTS3_PARENTHESIS

QMAKE_CFLAGS += $$SQLITE_FTS
QMAKE_CXXFLAGS += $$SQLITE_FTS -DSHIVASQLITE_EXPORTS
