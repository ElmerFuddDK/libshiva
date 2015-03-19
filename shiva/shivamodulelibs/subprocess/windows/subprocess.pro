include(../../../../shiva/include/shiva.pri)

QT       -= core gui

LIBS += -L. \
        -lshiva \
        -lpsapi
QMAKE_PRE_LINK = copydeps.bat

TARGET = subprocess
TEMPLATE = lib

SOURCES += ../src/modules/shvsubprocessimpl.cpp \
 ../src/modules/shvsubprocessserverimpl.cpp \
 ../src/shvsubprocessfactory.cpp \
 ../src/shvsubprocessexports.cpp
HEADERS += \
 ../src/modules/shvsubprocessimpl.h \
 ../src/modules/shvsubprocessserverimpl.h \
 ../src/shvsubprocessfactory.h

INCLUDEPATH += ../../../..

DEF_FILE = subprocess.def

QMAKE_CXXFLAGS += -DPSAPI_VERSION=1
