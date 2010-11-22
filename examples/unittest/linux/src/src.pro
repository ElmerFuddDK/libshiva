SOURCES = ../../src/main.cpp \
    ../../src/shvunittest.cpp \
    ../../src/forms/shvformmain.cpp \
    ../../src/logger/shvtestloggerconsole.cpp \
    ../../src/logger/shvtestloggergui.cpp \
    ../../src/tests/shvtestbase.cpp \
    ../../src/tests/shvtestqueued.cpp \
    ../../src/tests/threadutils/shvmutextester.cpp \
    ../../src/tests/threadutils/shvthreadtester.cpp \
    ../../src/tests/utils/shvbooltester.cpp \
    ../../src/tests/utils/shvdoubletester.cpp \
    ../../src/tests/utils/shvdynarraytester.cpp \
    ../../src/tests/utils/shvvectortester.cpp \
    ../../src/tests/utils/shvtimetester.cpp \
    ../../src/tests/utils/shvstringtester.cpp \
    ../../src/tests/utils/shvlisttester.cpp \
    ../../src/tests/utils/shvinttester.cpp \
    ../../src/tests/utils/shvhashtabletester.cpp \
    ../../src/tests/utils/shvfiletester.cpp \
    ../../src/tests/utils/shvbuffertester.cpp
HEADERS += ../../src/shvunittest.h \
    ../../src/forms/shvformmain.h \
    ../../src/logger/shvtestloggerconsole.h \
    ../../src/logger/shvtestloggergui.h \
    ../../src/logger/shvtestlogger.h \
    ../../src/tests/shvtestbase.h \
    ../../src/tests/shvtestqueued.h \
    ../../src/tests/threadutils/shvmutextester.h \
    ../../src/tests/threadutils/shvthreadtester.h \
    ../../src/tests/utils/shvbooltester.h \
    ../../src/tests/utils/shvdoubletester.h \
    ../../src/tests/utils/shvdynarraytester.h \
    ../../src/tests/utils/shvvectortester.h \
    ../../src/tests/utils/shvtimetester.h \
    ../../src/tests/utils/shvstringtester.h \
    ../../src/tests/utils/shvlisttester.h \
    ../../src/tests/utils/shvinttester.h \
    ../../src/tests/utils/shvhashtabletester.h \
    ../../src/tests/utils/shvfiletester.h \
    ../../src/tests/utils/shvbuffertester.h
TARGET =../bin/unittest
TEMPLATE = app
LIBS += -L../../../../shivalib/linux/libshiva \
-ldl \
-lshiva
QMAKE_CXXFLAGS_DEBUG += -DDEBUG

CONFIG -= release \
 qt

CONFIG += debug
QMAKE_POST_LINK = ../bin/copydeps.sh

INCLUDEPATH += ../../../../..
