include(../../../SHVRelPathSHV/shiva/include/shiva.pri)

SOURCES = ../../src/main.cpp \
../../src/shvshvtemplateshv.cpp \
../../src/forms/shvformmain.cpp 

HEADERS += ../../src/shvshvtemplateshv.h \
../../src/forms/shvformmain.h 

TARGET =../bin/shvtemplateshv
TEMPLATE = app
LIBS += -L../../../SHVRelPathSHV/shiva/shivalib/linux/libshiva \
$$QMAKE_LIBS_DYNLOAD \
-lshiva

CONFIG -= release \
 qt

CONFIG += debug
QMAKE_POST_LINK = ../bin/copydeps.sh

INCLUDEPATH += ../../../SHVRelPathSHV
