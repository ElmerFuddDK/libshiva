#-------------------------------------------------
#
# Project created by QtCreator 2010-06-29T15:14:45
#
#-------------------------------------------------
include(../../../shiva/include/shiva.pri)

QT       -= core gui

TARGET = libshiva
TEMPLATE = lib
QMAKE_POST_LINK = copylinkfile.bat

DEFINES += LIBSHIVA_EXPORTS

HEADERS += ../../include/platformspc.h \
           ../../include/shvlogger.h \
           ../../include/shvtypes.h \
           ../../include/shvversion.h \
           ../../include/framework/shvconfig.h \
           ../../include/framework/shveventdata.h \
           ../../include/framework/shveventemitter.h \
           ../../include/framework/shvevent.h \
           ../../include/framework/shveventqueuelist.h \
           ../../include/framework/shveventstring.h \
           ../../include/framework/shveventstructs.h \
           ../../include/framework/shvmainthreadeventdispatcher.h \
           ../../include/framework/shvmodulebase.h \
           ../../include/framework/shvmodulefactory.h \
           ../../include/framework/shvmodule.h \
           ../../include/framework/shvmodulelist.h \
           ../../include/framework/shvmoduleloader.h \
           ../../include/framework/shvtimer.h \
           ../../include/threadutils/shvmutexbase.h \
           ../../include/threadutils/shvmutex.h \
           ../../include/threadutils/shvmutexlocker.h \
           ../../include/threadutils/shvsemaphore.h \
           ../../include/threadutils/shvsemaphorelocker.h \
           ../../include/threadutils/shvthreadbase.h \
           ../../include/threadutils/shvthread.h \
           ../../include/utils/shvassert.h \
           ../../include/utils/shvbufferc.h \
           ../../include/utils/shvbuffercstream.h \
           ../../include/utils/shvbuffer.h \
           ../../include/utils/shvdir.h \
           ../../include/utils/shvdllbase.h \
           ../../include/utils/shvdll.h \
           ../../include/utils/shvdynarraybase.h \
           ../../include/utils/shvdynarray.h \
           ../../include/utils/shvdynarrayref.h \
           ../../include/utils/shvfilebase.h \
           ../../include/utils/shvfile.h \
           ../../include/utils/shvhashtablebase.h \
           ../../include/utils/shvhashtable.h \
           ../../include/utils/shvhashvalue.h \
           ../../include/utils/shvlistbase.h \
           ../../include/utils/shvlist.h \
           ../../include/utils/shvptrcontainer.h \
           ../../include/utils/shvrefobject.h \
           ../../include/utils/shvstring16c.h \
           ../../include/utils/shvstring16.h \
           ../../include/utils/shvstringc.h \
           ../../include/utils/shvstring.h \
           ../../include/utils/shvstringconv.h \
           ../../include/utils/shvstringstream.h \
           ../../include/utils/shvstringutf8c.h \
           ../../include/utils/shvstringutf8.h \
           ../../include/utils/shvtime.h \
           ../../include/utils/shvvectorbase.h \
           ../../include/utils/shvvector.h \
           ../../include/utils/shvvectorref.h \
           ../../include/framework/shveventstdin.h \
           ../../include/framework/shveventsubscriberdynamic.h \
           ../../include/framework/shveventsubscriberbase.h \
           ../../include/framework/shveventsubscriber.h \
           ../../include/framework/shveventsubscriptionlist.h \
           ../../include/framework/shveventtargetdynamic.h \
           ../../include/framework/shveventthreadpool.h \
           ../../include/framework/shveventthread.h \
           ../../include/frameworkimpl/shvconfigimpl.h \
           ../../include/frameworkimpl/shvmainthreadeventdispatchergeneric.h \
           ../../include/frameworkimpl/shvmainthreadeventqueue.h \
           ../../include/frameworkimpl/shvmodulelistimpl.h \
           ../../include/frameworkimpl/shvmoduleloaderimpl.h \
           ../../include/frameworkimpl/shvtimerimpl.h  \
           ../../include/frameworkimpl/shvmainthreadeventdispatcherconsole.h \
           ../../include/utils/shvmath.h \
           ../../include/utils/shvuuid.h \
           ../../include/utils/shvmd5sum.h \
           ../../include/utils/shvmathtokenmap.h \
           ../../include/utils/shvmathtokenhashmap.h \
           ../../include/threadutils/shvthreadpool.h \
           ../../include/threadutils/shvthreadpoolbase.h \
           ../../include/framework/shvconsole.h
SOURCES += ../src/framework/shvevent.cpp \
           ../src/framework/shveventemitter.cpp \
           ../src/framework/shveventqueuelist.cpp \
           ../src/framework/shveventstdin.cpp \
           ../src/framework/shveventstring.cpp \
           ../src/framework/shveventsubscriberbase.cpp \
           ../src/framework/shveventsubscriber.cpp \
           ../src/framework/shveventsubscriberdynamic.cpp \
           ../src/framework/shveventsubscriptionlist.cpp \
           ../src/framework/shveventthreadpool.cpp \
           ../src/framework/shveventthread.cpp \
           ../src/framework/shvgui.cpp \
           ../src/framework/shvmodule.cpp \
           ../src/framework/shvmodulelist.cpp \
           ../src/frameworkimpl/shvconfigimpl.cpp \
           ../src/frameworkimpl/shvmainthreadeventdispatchergeneric.cpp \
           ../src/frameworkimpl/shvmainthreadeventqueue.cpp \
           ../src/frameworkimpl/shvmodulelistimpl.cpp \
           ../src/frameworkimpl/shvmoduleloaderimpl.cpp \
           ../src/frameworkimpl/shvtimerimpl.cpp \
           ../src/threadutils/shvmutexbase.cpp \
           ../src/threadutils/shvmutex.cpp \
           ../src/threadutils/shvsemaphore.cpp \
           ../src/threadutils/shvreadwritelock.cpp \
           ../src/threadutils/shvthreadbase.cpp \
           ../src/utils/shvassert.cpp \
           ../src/utils/shvbufferc.cpp \
           ../src/utils/shvbuffer.cpp \
           ../src/utils/shvbuffercstream.cpp \
           ../src/utils/shvdir.cpp \
           ../src/utils/shvrefobject.cpp \
           ../src/utils/shvstringconv.cpp \
           ../src/utils/shvstringstream.cpp \
           ../src/utils/shvstring.cpp \
           ../src/utils/shvstringutf8.cpp \
           ../src/utils/shvtime.cpp \
           ../src/utils/shvvectorbase.cpp \
           ../src/utils/shvstring16.cpp \
           ../src/utils/shvdllbase.cpp \
           ../src/utils/shvdll.cpp \
           ../src/utils/shvdynarraybase.cpp \
           ../src/utils/shvfilebase.cpp \
           ../src/utils/shvfile.cpp \
           ../src/utils/shvhashtablebase.cpp \
           ../src/utils/shvlistbase.cpp  \
           ../src/frameworkimpl/shvmainthreadeventdispatcherconsole.cpp \
           ../src/utils/shvbufferstream.cpp \
           ../src/utils/shvstreambufferin.cpp \
           ../src/utils/shvstreambufferout.cpp \
           ../src/utils/shvmath.cpp \
           ../src/uuid/sysdep.c \
           ../src/uuid/uuid.c \
           ../src/utils/shvuuid.cpp \
           ../src/md5/md5.c \
           ../src/utils/shvmd5sum.cpp \
           ../src/gui/utils/shvrect.cpp \
           ../src/utils/shvmathtokenhashmap.cpp \
           ../src/threadutils/shvthreadpoolbase.cpp \
           ../src/framework/shvconsole.cpp \
           ../src/utils/shvbase64.cpp

LIBS += -lWs2_32

win32 {
	system("echo n | copy /-Y ..\\..\\include\\shvconfig.h.template ..\\..\\include\\shvconfig.h")
}
