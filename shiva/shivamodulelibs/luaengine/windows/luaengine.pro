QT       -= core gui

LIBS += -L. \
        -lshiva
QMAKE_PRE_LINK = copydeps.bat

TARGET = luaengine
TEMPLATE = lib

SOURCES += ../src/modules/shvluaengineimpl.cpp \
 ../src/shvluaenginefactory.cpp \
 ../src/shvluaengineexports.cpp \
    ../src/lua/lzio.c \
    ../src/lua/lvm.c \
    ../src/lua/lundump.c \
    ../src/lua/ltm.c \
    ../src/lua/ltablib.c \
    ../src/lua/ltable.c \
    ../src/lua/lstrlib.c \
    ../src/lua/lstring.c \
    ../src/lua/lstate.c \
    ../src/lua/lparser.c \
    ../src/lua/loslib.c \
    ../src/lua/lopcodes.c \
    ../src/lua/lobject.c \
    ../src/lua/loadlib.c \
    ../src/lua/lmem.c \
    ../src/lua/lmathlib.c \
    ../src/lua/llex.c \
    ../src/lua/liolib.c \
    ../src/lua/linit.c \
    ../src/lua/lgc.c \
    ../src/lua/lfunc.c \
    ../src/lua/ldump.c \
    ../src/lua/ldo.c \
    ../src/lua/ldebug.c \
    ../src/lua/ldblib.c \
    ../src/lua/lctype.c \
    ../src/lua/lcorolib.c \
    ../src/lua/lcode.c \
    ../src/lua/lbitlib.c \
    ../src/lua/lbaselib.c \
    ../src/lua/lauxlib.c \
    ../src/lua/lapi.c \
    ../src/modules/shvluascriptimpl.cpp \
    ../src/modules/shvluafuncargsimpl.cpp \
    ../src/modules/shvluavalueimpl.cpp \
    ../src/modules/shvluarefobjecttype.cpp
HEADERS += \
 ../src/modules/shvluaengineimpl.h \
 ../src/shvluaenginefactory.h \
    ../src/lua/lzio.h \
    ../src/lua/lvm.h \
    ../src/lua/lundump.h \
    ../src/lua/lualib.h \
    ../src/lua/luaconf.h \
    ../src/lua/lua.hpp \
    ../src/lua/lua.h \
    ../src/lua/ltm.h \
    ../src/lua/ltable.h \
    ../src/lua/lstring.h \
    ../src/lua/lstate.h \
    ../src/lua/lparser.h \
    ../src/lua/lopcodes.h \
    ../src/lua/lobject.h \
    ../src/lua/lmem.h \
    ../src/lua/llimits.h \
    ../src/lua/llex.h \
    ../src/lua/lgc.h \
    ../src/lua/lfunc.h \
    ../src/lua/ldo.h \
    ../src/lua/ldebug.h \
    ../src/lua/lctype.h \
    ../src/lua/lcode.h \
    ../src/lua/lauxlib.h \
    ../src/lua/lapi.h \
    ../../../include/modules/luaengine/shvluascript.h \
    ../../../include/modules/luaengine/shvluaengine.h \
    ../src/modules/shvluascriptimpl.h \
    ../../../include/modules/luaengine/shvluafunc.h \
    ../src/modules/shvluafuncargsimpl.h \
    ../../../include/modules/luaengine/shvluavalue.h \
    ../src/modules/shvluavalueimpl.h \
    ../src/modules/shvluarefobjecttype.h

INCLUDEPATH += ../../../..

DEF_FILE = luaengine.def

QMAKE_CXXFLAGS_DEBUG += -DDEBUG 
