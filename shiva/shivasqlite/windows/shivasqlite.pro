#-------------------------------------------------
#
# Project created by QtCreator 2010-06-29T15:14:45
#
#-------------------------------------------------

QT       -= core gui

LIBS += -L. \
        -lshiva
QMAKE_PRE_LINK = copydeps.bat

TARGET = shivasqlite
TEMPLATE = lib

SQLITE_FTS = -DSQLITE_ENABLE_FTS3 -DSQLITE_ENABLE_FTS3_PARENTHESIS
QMAKE_CFLAGS += $$SQLITE_FTS
QMAKE_CXXFLAGS += $$SQLITE_FTS

HEADERS += ../../include/sqlite/shvstringsqlite.h \
           ../../include/sqlite/sqlitestatement.h \
           ../../include/sqlite/sqlitewrapper.h \
           ../include/sqlitestatementimpl.h \
           ../include/sqlitewrapperimpl.h
SOURCES += shivasqlite.cpp \
		   ../src/sqlite/sqlite3.c \
           ../src/sqlitestatementimpl.cpp \
           ../src/sqlitewrapperimpl.cpp
DEF_FILE = shivasqlite.def

QMAKE_CXXFLAGS_DEBUG += -DDEBUG 
