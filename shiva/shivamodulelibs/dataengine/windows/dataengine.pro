#-------------------------------------------------
#
# Project created by QtCreator 2010-06-29T15:14:45
#
#-------------------------------------------------
include(../../../../shiva/include/shiva.pri)

QT       -= core gui

LIBS += -L. \
        -lshiva
QMAKE_PRE_LINK = copydeps.bat

TARGET = dataengine
TEMPLATE = lib

HEADERS += ../../../include/modules/dataengine/shvdataengine.h \
           ../../../include/modules/dataengine/shvdatafactory.h \
           ../../../include/modules/dataengine/shvdatafunction.h \
           ../../../include/modules/dataengine/shvdatarowc.h \
           ../../../include/modules/dataengine/shvdatarowchangecache.h \
           ../../../include/modules/dataengine/shvdatarow.h \
           ../../../include/modules/dataengine/shvdatarowkey.h \
           ../../../include/modules/dataengine/shvdatarowlistc.h \
           ../../../include/modules/dataengine/shvdatarowlist.h \
           ../../../include/modules/dataengine/shvdatasession.h \
           ../../../include/modules/dataengine/shvdatastatement.h \
           ../../../include/modules/dataengine/shvdatastructc.h \
           ../../../include/modules/dataengine/shvdatastruct.h \
           ../../../include/modules/dataengine/shvdatavariant.h \
           ../include/shvdataenginefactory.h \
           ../include/shvdataengineimpl.h \
           ../include/shvdatafactoryimpl.h \
           ../include/shvdatafunctionimpl.h \
           ../include/shvdatarowchangecacheimpl.h \
           ../include/shvdatarowcsqlite.h \
           ../include/shvdatarowimpl.h \
           ../include/shvdatarowkeyimpl.h \
           ../include/shvdatarowlistcindexed.h \
           ../include/shvdatarowlistcsqlite.h \
           ../include/shvdatarowlistsqlite.h \
           ../include/shvdatasessionsqlite.h \
           ../include/shvdatastatementimpl.h \
           ../include/shvdatastructcsqlite.h \
           ../include/shvdatastructimpl.h \
           ../include/shvdatavariantimpl.h
SOURCES += dataengine.cpp \
           ../src/shvdataenginefactory.cpp \
           ../src/shvdataengineimpl.cpp \
           ../src/shvdatafactoryimpl.cpp \
           ../src/shvdatafunctionimpl.cpp \
           ../src/shvdatarowchangecacheimpl.cpp \
           ../src/shvdatarowcsqlite.cpp \
           ../src/shvdatarowimpl.cpp \
           ../src/shvdatarowkeyimpl.cpp \
           ../src/shvdatarowlistcindexed.cpp \
           ../src/shvdatarowlistcsqlite.cpp \
           ../src/shvdatarowlistsqlite.cpp \
           ../src/shvdatasessionsqlite.cpp \
           ../src/shvdatastatementimpl.cpp \
           ../src/shvdatastructcsqlite.cpp \
           ../src/shvdatastructimpl.cpp \
           ../src/shvdatavariantimpl.cpp


DEF_FILE = dataengine.def
