SOURCES =shivasqlite.cpp \
../../src/sqlite/alter.c \
../../src/sqlite/analyze.c \
../../src/sqlite/attach.c \
../../src/sqlite/auth.c \
../../src/sqlite/btmutex.c \
../../src/sqlite/btree.c \
../../src/sqlite/build.c \
../../src/sqlite/callback.c \
../../src/sqlite/complete.c \
../../src/sqlite/date.c \
../../src/sqlite/delete.c \
../../src/sqlite/expr.c \
../../src/sqlite/func.c \
../../src/sqlite/hash.c \
../../src/sqlite/insert.c \
../../src/sqlite/journal.c \
../../src/sqlite/legacy.c \
../../src/sqlite/loadext.c \
../../src/sqlite/main.c \
../../src/sqlite/malloc.c \
../../src/sqlite/mem1.c \
../../src/sqlite/mem2.c \
../../src/sqlite/mem3.c \
../../src/sqlite/mem4.c \
../../src/sqlite/mutex.c \
../../src/sqlite/mutex_os2.c \
../../src/sqlite/mutex_unix.c \
../../src/sqlite/mutex_w32.c \
../../src/sqlite/opcodes.c \
../../src/sqlite/os.c \
../../src/sqlite/os_os2.c \
../../src/sqlite/os_unix.c \
../../src/sqlite/os_win.c \
../../src/sqlite/pager.c \
../../src/sqlite/parse.c \
../../src/sqlite/pragma.c \
../../src/sqlite/prepare.c \
../../src/sqlite/printf.c \
../../src/sqlite/random.c \
../../src/sqlite/select.c \
../../src/sqlite/table.c \
../../src/sqlite/tokenize.c \
../../src/sqlite/trigger.c \
../../src/sqlite/update.c \
../../src/sqlite/utf.c \
../../src/sqlite/util.c \
../../src/sqlite/vacuum.c \
../../src/sqlite/vdbeapi.c \
../../src/sqlite/vdbeaux.c \
../../src/sqlite/vdbeblob.c \
../../src/sqlite/vdbe.c \
../../src/sqlite/vdbefifo.c \
../../src/sqlite/vdbemem.c \
../../src/sqlite/vtab.c \
../../src/sqlite/where.c \
../../src/sqlitestatementimpl.cpp \
../../src/sqlitewrapperimpl.cpp
TARGET =shivasqlite
DESTDIR =../bin

LIBS += -L../../../shivalib/linux/libshiva \
  -ldl \
  -lshiva \
  -lpthread

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


SOURCES -= ../../src/sqlite/tclsqlite.c

