include(../../../../../shiva/include/shiva.pri)

TEMPLATE = lib
QT -= core gui qt
CONFIG -= qt

DESTDIR = $$_PRO_FILE_PWD_/../../../../$${SHVBUILDFOLDER}/modules
QMAKE_LIBDIR += $$_PRO_FILE_PWD_/../../../../$${SHVBUILDFOLDER}

shivastaticlib {
  CONFIG += staticlib
}

LIBS += $$QMAKE_LIBS_DYNLOAD \
  -lshiva

include(../../../../../shiva/include/shiva-postconfig.pri)

INCLUDEPATH += ../..

DEF_FILE = $$_PRO_FILE_PWD_/exports.def

SOURCES += ../../src/shvdataengineexports.cpp \
  ../../src/shvdataenginefactory.cpp \
  ../../src/shvdataengineimpl.cpp \
  ../../src/shvdatafactoryimpl.cpp \
  ../../src/shvdatarowchangecacheimpl.cpp \
  ../../src/shvdatarowcsqlite.cpp \
  ../../src/shvdatarowimpl.cpp \
  ../../src/shvdatarowkeyimpl.cpp \
  ../../src/shvdatarowlistcindexed.cpp \
  ../../src/shvdatarowlistcsqlite.cpp \
  ../../src/shvdatarowlistsqlite.cpp \
  ../../src/shvdatasessionsqlite.cpp \
  ../../src/shvdatastructcsqlite.cpp \
  ../../src/shvdatastructimpl.cpp \
  ../../src/shvdatavariantimpl.cpp \
  ../../src/shvdatafunctionimpl.cpp \
  ../../src/shvdatastatementimpl.cpp
HEADERS += ../../../../include/modules/dataengine/shvdataengine.h \
  ../../../../include/modules/dataengine/shvdatafactory.h \
  ../../../../include/modules/dataengine/shvdatarowc.h \
  ../../../../include/modules/dataengine/shvdatarowchangecache.h \
  ../../../../include/modules/dataengine/shvdatarow.h \
  ../../../../include/modules/dataengine/shvdatarowkey.h \
  ../../../../include/modules/dataengine/shvdatarowlistc.h \
  ../../../../include/modules/dataengine/shvdatarowlist.h \
  ../../../../include/modules/dataengine/shvdatasession.h \
  ../../../../include/modules/dataengine/shvdatastructc.h \
  ../../../../include/modules/dataengine/shvdatastruct.h \
  ../../../../include/modules/dataengine/shvdatavariant.h \
  ../../include/shvdataenginefactory.h \
  ../../include/shvdataengineimpl.h \
  ../../include/shvdatafactoryimpl.h \
  ../../include/shvdatarowchangecacheimpl.h \
  ../../include/shvdatarowcsqlite.h \
  ../../include/shvdatarowimpl.h \
  ../../include/shvdatarowkeyimpl.h \
  ../../include/shvdatarowlistcindexed.h \
  ../../include/shvdatarowlistcsqlite.h \
  ../../include/shvdatarowlistsqlite.h \
  ../../include/shvdatasessionsqlite.h \
  ../../include/shvdatastructcsqlite.h \
  ../../include/shvdatastructimpl.h \
  ../../include/shvdatavariantimpl.h \
  ../../../../include/modules/dataengine/shvdatafunction.h \
  ../../include/shvdatafunctionimpl.h \
  ../../../../include/modules/dataengine/shvdatastatement.h \
  ../../include/shvdatastatementimpl.h
