include(../../../../shiva/include/shiva.pri)

TEMPLATE = lib
QT -= core gui qt
CONFIG -= qt

DESTDIR = $$_PRO_FILE_PWD_/../../../$${SHVBUILDFOLDER}

shivastaticlib {
  CONFIG += staticlib
} else {
  DEFINES += LIBSHIVA_EXPORTS
}

win32 {
  isEmpty(CE_SDK) {
    LIBS += -lWs2_32
    LIBS *= -luser32 -lshell32
  } else {
    LIBS += -lws2 -laygshell
  }
} else {
  LIBS += -lm \
    -lrt \
    $$QMAKE_LIBS_DYNLOAD \
    -lpthread
}

include(../../../../shiva/include/shiva-postconfig.pri)

HEADERS += ../../../include/platformspc.h \
  ../../../include/shvlogger.h \
  ../../../include/shvtypes.h \
  ../../../include/shvversion.h \
  ../../../include/framework/shvconfig.h \
  ../../../include/framework/shvconsole.h \
  ../../../include/framework/shveventdata.h \
  ../../../include/framework/shveventemitter.h \
  ../../../include/framework/shvevent.h \
  ../../../include/framework/shveventqueuelist.h \
  ../../../include/framework/shveventstring.h \
  ../../../include/framework/shveventstructs.h \
  ../../../include/framework/shvgui.h \
  ../../../include/framework/shvmainthreadeventdispatcher.h \
  ../../../include/framework/shvmodulebase.h \
  ../../../include/framework/shvmodulefactory.h \
  ../../../include/framework/shvmodule.h \
  ../../../include/framework/shvmodulelist.h \
  ../../../include/framework/shvmoduleloader.h \
  ../../../include/framework/shvtimer.h \
  ../../../include/threadutils/shvmutexbase.h \
  ../../../include/threadutils/shvmutex.h \
  ../../../include/threadutils/shvmutexlocker.h \
  ../../../include/threadutils/shvsemaphore.h \
  ../../../include/threadutils/shvsemaphorelocker.h \
  ../../../include/threadutils/shvthreadbase.h \
  ../../../include/threadutils/shvthread.h \
  ../../../include/utils/shvassert.h \
  ../../../include/utils/shvbufferc.h \
  ../../../include/utils/shvbuffercstream.h \
  ../../../include/utils/shvbuffer.h \
  ../../../include/utils/shvdir.h \
  ../../../include/utils/shvdllbase.h \
  ../../../include/utils/shvdll.h \
  ../../../include/utils/shvdynarraybase.h \
  ../../../include/utils/shvdynarray.h \
  ../../../include/utils/shvdynarrayref.h \
  ../../../include/utils/shvfilebase.h \
  ../../../include/utils/shvfile.h \
  ../../../include/utils/shvhashtablebase.h \
  ../../../include/utils/shvhashtable.h \
  ../../../include/utils/shvhashvalue.h \
  ../../../include/utils/shvlistbase.h \
  ../../../include/utils/shvlist.h \
  ../../../include/utils/shvptrcontainer.h \
  ../../../include/utils/shvrefobject.h \
  ../../../include/utils/shvstring16c.h \
  ../../../include/utils/shvstring16.h \
  ../../../include/utils/shvstring8c.h \
  ../../../include/utils/shvstring8.h \
  ../../../include/utils/shvstringc.h \
  ../../../include/utils/shvstringc.inl \
  ../../../include/utils/shvstring.h \
  ../../../include/utils/shvstringconv.h \
  ../../../include/utils/shvstringdefaults.h \
  ../../../include/utils/shvstringconv.h \
  ../../../include/utils/shvstringutf8c.h \
  ../../../include/utils/shvstringutf8.h \
  ../../../include/utils/shvtime.h \
  ../../../include/utils/shvvectorbase.h \
  ../../../include/utils/shvvector.h \
  ../../../include/utils/shvvectorref.h \
  ../../../include/framework/shveventstdin.h \
  ../../../include/framework/shveventsubscriberdynamic.h \
  ../../../include/framework/shveventsubscriberbase.h \
  ../../../include/framework/shveventsubscriber.h \
  ../../../include/framework/shveventsubscriptionlist.h \
  ../../../include/framework/shveventtargetdynamic.h \
  ../../../include/framework/shveventthreadpool.h \
  ../../../include/framework/shveventthread.h \
  ../../../include/frameworkimpl/shvconfigimpl.h \
  ../../../include/frameworkimpl/shvmainthreadeventdispatchergeneric.h \
  ../../../include/frameworkimpl/shvmainthreadeventqueue.h \
  ../../../include/frameworkimpl/shvmodulelistimpl.h \
  ../../../include/frameworkimpl/shvmoduleloaderimpl.h \
  ../../../include/frameworkimpl/shvtimerimpl.h  \
  ../../../include/frameworkimpl/shvmainthreadeventdispatcherconsole.h \
  ../../../include/modules/shvmodulefactories.h \
  ../../../include/utils/shvmath.h \
  ../../../include/utils/shvuuid.h \
  ../../../include/utils/shvmd5sum.h \
  ../../../include/utils/shvmathtokenmap.h \
  ../../../include/utils/shvmathtokenhashmap.h \
  ../../../include/threadutils/shvthreadpoolbase.h \
  ../../../include/threadutils/shvthreadpool.h \
  ../../../include/utils/shvbase64.h
SOURCES += ../../src/framework/shvconsole.cpp \
  ../../src/framework/shvevent.cpp \
  ../../src/framework/shveventemitter.cpp \
  ../../src/framework/shveventqueuelist.cpp \
  ../../src/framework/shveventstdin.cpp \
  ../../src/framework/shveventstring.cpp \
  ../../src/framework/shveventsubscriberbase.cpp \
  ../../src/framework/shveventsubscriber.cpp \
  ../../src/framework/shveventsubscriberdynamic.cpp \
  ../../src/framework/shveventsubscriptionlist.cpp \
  ../../src/framework/shveventthreadpool.cpp \
  ../../src/framework/shveventthread.cpp \
  ../../src/framework/shvgui.cpp \
  ../../src/framework/shvmodule.cpp \
  ../../src/framework/shvmodulelist.cpp \
  ../../src/frameworkimpl/shvconfigimpl.cpp \
  ../../src/frameworkimpl/shvmainthreadeventdispatchergeneric.cpp \
  ../../src/frameworkimpl/shvmainthreadeventqueue.cpp \
  ../../src/frameworkimpl/shvmodulelistimpl.cpp \
  ../../src/frameworkimpl/shvmoduleloaderimpl.cpp \
  ../../src/frameworkimpl/shvtimerimpl.cpp \
  ../../src/threadutils/shvmutexbase.cpp \
  ../../src/threadutils/shvmutex.cpp \
  ../../src/threadutils/shvsemaphore.cpp \
  ../../src/threadutils/shvreadwritelock.cpp \
  ../../src/threadutils/shvthreadbase.cpp \
  ../../src/utils/shvassert.cpp \
  ../../src/utils/shvbufferc.cpp \
  ../../src/utils/shvbuffer.cpp \
  ../../src/utils/shvbuffercstream.cpp \
  ../../src/utils/shvdir.cpp \
  ../../src/utils/shvrefobject.cpp \
  ../../src/utils/shvstringconv.cpp \
  ../../src/utils/shvstringstream.cpp \
  ../../src/utils/shvstring.cpp \
  ../../src/utils/shvstringutf8.cpp \
  ../../src/utils/shvtime.cpp \
  ../../src/utils/shvvectorbase.cpp \
  ../../src/utils/shvstring16.cpp \
  ../../src/utils/shvdllbase.cpp \
  ../../src/utils/shvdll.cpp \
  ../../src/utils/shvdynarraybase.cpp \
  ../../src/utils/shvfilebase.cpp \
  ../../src/utils/shvfile.cpp \
  ../../src/utils/shvhashtablebase.cpp \
  ../../src/utils/shvlistbase.cpp  \
  ../../src/frameworkimpl/shvmainthreadeventdispatcherconsole.cpp \
  ../../src/utils/shvbufferstream.cpp \
  ../../src/utils/shvstreambufferin.cpp \
  ../../src/utils/shvstreambufferout.cpp \
  ../../src/utils/shvmath.cpp \
  ../../src/uuid/sysdep.c \
  ../../src/uuid/uuid.c \
  ../../src/utils/shvuuid.cpp \
  ../../src/md5/md5.c \
  ../../src/utils/shvmd5sum.cpp \
  ../../src/gui/utils/shvrect.cpp \
  ../../src/utils/shvmathtokenhashmap.cpp \
  ../../src/threadutils/shvthreadpoolbase.cpp \
  ../../src/utils/shvbase64.cpp

mac {
  DEFINES += _POSIX_C_SOURCE=200112L _XOPEN_SOURCE=500 _REENTRANT _THREAD_SAFE
  DEFINES += _DARWIN_C_SOURCE
  LIBS -= -lrt
  CONFIG *= shivastaticunicode
}

!isEmpty(ANDROID_PLATFORM){
  DEFINES += __SHIVA_NO_TIMEGM __SHIVA_SYSINFOSYSCALL
  CONFIG *= shivastaticunicode
  DEFINES += HAVE_MEMMOVE
  LIBS -= -llog
#  QMAKE_LIBS += -lgnustl_static
} else {
  unix {
    system(test "`uname`" = "FreeBSD") {
      LIBS += -lkvm
      CONFIG *= shivastaticunicode
    }
  }
}

shivastaticunicode {
  SOURCES += ../../src/libunicode/convert.c \
    ../../src/libunicode/decomp.c \
    ../../src/libunicode/init.c \
    ../../src/libunicode/iso8859.c \
    ../../src/libunicode/latin1.c \
    ../../src/libunicode/prop.c \
    ../../src/libunicode/sjis.c \
    ../../src/libunicode/ucs2.c \
    ../../src/libunicode/ucs4.c \
    ../../src/libunicode/utf8.c \
    ../../src/libunicode/utf8conv.c
    DEFINES += __SHIVA_LIBUNICODESTATIC
}

win32 {
  system("echo n | copy /-Y ..\\..\\..\\include\\shvconfig.h.template ..\\..\\..\\include\\shvconfig.h")
}
else {
  system("test -e ../../../include/shvconfig.h || cp ../../../include/shvconfig.h.template ../../../include/shvconfig.h")
}
