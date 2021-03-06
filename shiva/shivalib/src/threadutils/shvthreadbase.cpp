/*
 *   Copyright (C) 2008 by Lars Eriksen
 *
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 of the
 *   License, or (at your option) any later version with the following
 *   exeptions:
 *
 *   1) Static linking to the library does not constitute derivative work
 *      and does not require the author to provide source code for the
 *      application.
 *      Compiling applications with the source code directly linked in is
 *      Considered static linking as well.
 *
 *   2) You do not have to provide a copy of the license with programs
 *      that are linked against this code.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "stdafx.h"
#include "../../../include/platformspc.h"

#include "../../../include/threadutils/shvthreadbase.h"

#ifdef __SHIVA_WIN32
# ifndef STACK_SIZE_PARAM_IS_A_RESERVATION
#  define STACK_SIZE_PARAM_IS_A_RESERVATION 0x00010000
# endif
#elif defined(__SHIVA_EPOC)
# include <e32hal.h>
# include <e32base.h>
#else
# ifdef __SHIVA_POSIX_OSX
#  include <mach/clock.h>
#  include <mach/mach.h>
# endif
# include <signal.h>
# include <pthread.h>
# include <sched.h>
# include <sys/time.h>
# if !defined(CLOCK_MONOTONIC_RAW)
#  define CLOCK_MONOTONIC_RAW CLOCK_MONOTONIC
# endif
#endif


// ========================================================================================================
//  SHVThreadBase class
// ========================================================================================================
/// \class SHVThreadBase shvthreadbase.h "shiva/include/threadutils/shvthreadbase.h"

/*************************************
 * Constructor
 *************************************/
SHVThreadBase::SHVThreadBase()
{
#ifdef __SHIVA_WIN32
	ThreadHandle = INVALID_HANDLE_VALUE;
#elif defined(__SHIVA_EPOC)
	Running = false;
#else
	ThreadHandle = 0;
#endif
}

/*************************************
 * Destructor
 *************************************/
SHVThreadBase::~SHVThreadBase()
{
	if (IsRunning())
		Kill();
}

/*************************************
 * Start
 *************************************/
/// Starts the thread
/**
 \param func Pointer to the function to be called
 \param data Data pointer to be used as paramater in func
 \param priority One of the Prio* enum priorities (optional)
 \param name The name of the thread, for debugging (optional)
 \param stackSize Optional stack size
 \return Success
 *
 * If the thread is not already running, it will start the thread as func(data).
 * It will be started with the thread priority given by the priority parameter.
 * It will return true if the thread gets started, false if the start failed, or
 * it is already running.
 * NOTE: name is not supported on all platforms. It is meant as an aid for debugging.
 */
bool SHVThreadBase::Start(ThreadFunc func, void* data, short priority, const SHVStringC& name, SHVInt stackSize)
{
bool retVal = false;
#ifdef __SHIVA_WIN32
int prio;
DWORD threadID;
DWORD creationFlags = CREATE_SUSPENDED;
void** tmpData = (void**)::malloc(sizeof(void*)*2);

	tmpData[0] = (void*)func;
	tmpData[1] = data;

	switch(priority)
	{
	case PrioIdle:		prio = THREAD_PRIORITY_IDLE;		 break;
	case PrioLowest:	prio = THREAD_PRIORITY_LOWEST;		 break;
	case PrioLow:		prio = THREAD_PRIORITY_BELOW_NORMAL; break;
	case PrioHigh:		prio = THREAD_PRIORITY_ABOVE_NORMAL; break;
	case PrioHighest:	prio = THREAD_PRIORITY_HIGHEST;		 break;
	case PrioRealtime:	prio = THREAD_PRIORITY_TIME_CRITICAL;break;
	default:
	case PrioNormal:	prio = THREAD_PRIORITY_NORMAL;		 break;
	}

#ifdef __SHIVA_WINCE
	SHVASSERT(stackSize.IsNull()); // Stack size cannot be set on WinCE ... Yay M$!
#else
	if (!stackSize.IsNull())
		creationFlags |= STACK_SIZE_PARAM_IS_A_RESERVATION;
#endif

	ThreadHandle = CreateThread(NULL,stackSize.IfNull(0),&StartupFuncWin32,tmpData,creationFlags,&threadID);

	retVal = (ThreadHandle ? true : false);

	if (retVal)
	{
		ID = threadID;
		::SetThreadPriority(ThreadHandle, prio);
		ResumeThread(ThreadHandle);
	}
	else
	{
		ThreadHandle = INVALID_HANDLE_VALUE;
		::free(tmpData);
	}
#elif defined(__SHIVA_EPOC)
SHVStringC prefix(name);
SHVString tmpName;
void** tmpData = (void**)::malloc(sizeof(void*)*2);

	tmpData[0] = (void*)func;
	tmpData[1] = data;

	if (prefix != _S(""))
		prefix = _S("SHIVA");

	// create a unique name - this is required in epoc, dunno why
	tmpName.Format(_S("%s%d-%d"), prefix.GetSafeBuffer(), ((long)this), GetTickCount());

	if (ThreadHandle.Create(tmpName.ToPtr(),&StartupFuncEpoc,stackSize.IfNull(KDefaultStackSize),NULL,tmpData) == KErrNone)
	{
	TThreadId tmpId = ThreadHandle.Id();
	TThreadPriority prio;

		ID = *(TUint*)&tmpId;
		retVal = Running = true;

		switch(priority)
		{
		case PrioIdle:		prio = EPriorityAbsoluteVeryLow; break;
		case PrioLowest:	prio = EPriorityMuchLess;		 break;
		case PrioLow:		prio = EPriorityLess;			 break;
		case PrioHigh:		prio = EPriorityMore;			 break;
		case PrioHighest:	prio = EPriorityMuchMore;		 break;
		case PrioRealtime:	prio = EPriorityRealTime;		 break;
		default:
		case PrioNormal:	prio = EPriorityNormal;			 break;
		}
		ThreadHandle.SetPriority(prio);

		ThreadHandle.Resume();
	}
	else
		::free(tmpData);
#else
pthread_attr_t attr;
sched_param param;
int policy;
int min,max;
bool setPrio = false;
pthread_mutex_t* mutex;
void** tmpData = (void**)::malloc(sizeof(void*)*3);

	SHVUNUSED_PARAM(name);

	mutex = (pthread_mutex_t*)::malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutex,NULL);
	pthread_mutex_lock(mutex);

	tmpData[0] = (void*)func;
	tmpData[1] = data;
	tmpData[2] = (void*)mutex;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);

	if (priority > PrioNormal && HasSystemPrivs())
	{
		setPrio = true;
		policy = SCHED_RR;
		min = sched_get_priority_min(policy);
		max = sched_get_priority_max(policy);

		if (priority >= PrioRealtime) priority = max;
		else if (priority <= PrioHigh) priority = min;
		else priority = ((max-min)/2 + min);

		pthread_attr_setschedpolicy(&attr,policy);
		param.sched_priority = priority;
		SHVVERIFY(pthread_attr_setschedparam(&attr, &param) == 0);
	}
#ifdef SCHED_BATCH
	else if (priority == PrioLowest || priority == PrioLow)
	{
		setPrio = true;
		policy = SCHED_BATCH;

		min = sched_get_priority_min(policy);
		max = sched_get_priority_max(policy);

		if (priority >= PrioLow) priority = max;
		else priority = min;
		
		pthread_attr_setschedpolicy(&attr,policy);
		param.sched_priority = priority;
		SHVVERIFY(pthread_attr_setschedparam(&attr, &param) == 0);
	}
#endif
#ifdef SCHED_IDLE
	else if (priority == PrioIdle)
	{
		setPrio = true;
		policy = SCHED_IDLE;
		pthread_attr_setschedpolicy(&attr,policy);
		param.sched_priority = sched_get_priority_min(policy);
		SHVVERIFY(pthread_attr_setschedparam(&attr, &param) == 0);
	}
#endif
	
	SHVVERIFY(pthread_attr_setstacksize(&attr,(size_t)stackSize.IfNull(0x100000)) == 0); // 1 MB standard - 12 MB is simply too much

	ThreadHandle = (pthread_create( (pthread_t*)&ID, &attr, StartupFuncPosix, (void*)tmpData) ? 0/*error*/ : 1/*ok*/ );
#ifdef __SHIVA_POSIX_LINUX
	if (setPrio)
		SHVVERIFY(pthread_setschedparam(ID,policy,&param) == 0);
#endif

	retVal = (ThreadHandle ? true : false);

	pthread_mutex_unlock(mutex);

	if (!retVal)
	{
		pthread_mutex_destroy(mutex);
		::free(mutex);
		::free(tmpData);
	}

	pthread_attr_destroy(&attr);
#endif

	return retVal;
}

/*************************************
 * Kill
 *************************************/
/// Kills the thread
/**
 \return Success
 *
 * Don't use this function unless you really really have to - it
 * may cause damage to your program, since it simply stops the
 * execution of the thread, without any cleanup.
 */
bool SHVThreadBase::Kill()
{
bool retVal = false;

	if (IsRunning())
	{
#ifdef __SHIVA_WIN32
		retVal = (TerminateThread(ThreadHandle,0) ? true : false);
		if (retVal)
		{
			CloseHandle(ThreadHandle);
			ThreadHandle = INVALID_HANDLE_VALUE;
		}
#elif defined(__SHIVA_EPOC)
		ThreadHandle.Kill(0);
		Running = false;
		retVal = true;
#else
		pthread_kill((pthread_t)ID,SIGTERM);
		ThreadHandle = 0;
		retVal = true;
#endif
	}

	return retVal;
}

/*************************************
 * IsRunning
 *************************************/
/// Checks if the thread is started
/**
 \return true if the thread is running
 *
 * It is notable that the function will return false if the ResetHandle()
 * function has been called after startup, even if the thread is still
 * running.
 */
bool SHVThreadBase::IsRunning()
{
#ifdef __SHIVA_WIN32
	return (ThreadHandle != INVALID_HANDLE_VALUE);
#elif defined(__SHIVA_EPOC)
	if (!Running) return false;
	else if (ThreadHandle.ExitType() != EExitPending) Running = false;
	return Running;
#else
	return (ThreadHandle != 0);
#endif
}

/*************************************
 * GetThreadID
 *************************************/
/// Thread ID property
/**
 \return Thread ID
 */
SHVThreadBase::ThreadID SHVThreadBase::GetThreadID() const
{
	return ID;
}

/*************************************
 * ResetHandle
 *************************************/
/// Resets the handle to the thread
/**
 * Use this function from the thread before terminating, in order to
 * clear it, thus allowing it to be started again.
 * It is essential that you remember to call this function before the
 * thread terminates, otherwise it may crash on destroy.
 * Whilst it is possible to call it from other threads, it is generally
 * a bad idea, since the handle state is not thread safe.
 * In case you find this a bit too complicated, check out SHVThread instead.
 */
void SHVThreadBase::ResetHandle()
{
	if (IsRunning())
	{
#ifdef __SHIVA_WIN32
		CloseHandle(ThreadHandle);
		ThreadHandle = INVALID_HANDLE_VALUE;
#elif defined(__SHIVA_EPOC)
		ThreadHandle.Close();
		Running = false;
#else
		ThreadHandle = 0;
#endif
	}
}

/*************************************
 * Sleep
 *************************************/
/// Suspends exectution from the calling thread in given duration
/**
 \param millisecs Suspension duration in milliseconds
 */
void SHVThreadBase::Sleep(int millisecs)
{
#ifdef __SHIVA_WIN32
	::Sleep(millisecs);
#elif defined(__SHIVA_EPOC)
RTimer timer;
TRequestStatus status;

	timer.CreateLocal();
	timer.After(status, millisecs*1000);
	User::WaitForRequest(status);
	timer.Cancel();
	timer.Close();
#else
	::usleep(millisecs*1000);
#endif
}

/*************************************
 * GetCurrentThreadID
 *************************************/
/// Get thread id of the calling thread
/**
 \return Thread id
 */
SHVThreadBase::ThreadID SHVThreadBase::GetCurrentThreadID()
{
#ifdef __SHIVA_WIN32
	return ::GetCurrentThreadId();
#elif defined(__SHIVA_EPOC)
ThreadID retVal;
RThread curThread;
TThreadId tmpId = curThread.Id();

	retVal = *(TUint*)&tmpId;

	return retVal;
#else
	return (SHVThreadBase::ThreadID)pthread_self();
#endif
}

/*************************************
 * GetTickCount
 *************************************/
/// returns os specific tick count
/**
 \return tick counts in non-specific duration
 *
 * This function is used to get tick counts. The duration of
 * a tick is not specified. If you want more proper tick count
 * use the GetTicksInMilliSecs function.
 \note Ticks relative to the clock and not CPU in windows.
 */
long SHVThreadBase::GetTickCount()
{
#ifdef __SHIVA_WIN32
	return ::GetTickCount();
#elif defined(__SHIVA_EPOC)
	return User::TickCount();
#elif defined(__SHIVA_POSIX_OSX)
clock_serv_t cclock;
mach_timespec_t n;

	host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cclock);
	if (clock_get_time(cclock, &n))
	{
		fprintf(stderr,"GETTICKCOUNT ERROR\n");
		abort();
	}
	mach_port_deallocate(mach_task_self(), cclock);
	
	return long(n.tv_sec*1000000L) + long(n.tv_nsec/1000L);
#elif _POSIX_TIMERS > 0
struct timespec n;
	if (clock_gettime(CLOCK_MONOTONIC_RAW, &n))
	{
		fprintf(stderr,"GETTICKCOUNT ERROR\n");
		abort();
	}
	return long(n.tv_sec*1000000L) + long(n.tv_nsec/1000L);
#else
timeval val;
	gettimeofday(&val,NULL);
	return val.tv_sec+val.tv_usec;
#endif
}

/*************************************
 * GetTicksInMilliSecs
 *************************************/
/// returns CPU tick count in milli seconds
/**
 \return tick count in milliseconds
 *
 * Note that this function returns time relative to CPU
 * time. If the system is suspended then the tick counter
 * gets suspended as well. For ticks relative to the system
 * clock, use SHVTime::GetRelativeTimeInMilliSecs().
 \bug Ticks relative to the clock and not CPU in windows XP/2k3.
 */
long SHVThreadBase::GetTicksInMilliSecs()
{
#ifdef __SHIVA_WINCE
	return ::GetTickCount();
#elif defined(__SHIVA_WIN32)
typedef BOOL (FAR WINAPI *QueryUnbiasedInterruptTimeFunc)(PULONGLONG biasTick);
static int functionResolved;
static QueryUnbiasedInterruptTimeFunc QueryUnbiasedInterruptTimePtr;

	if (!functionResolved)
	{
	HINSTANCE hDll = LoadLibraryA("kernel32.dll");
		QueryUnbiasedInterruptTimePtr = (QueryUnbiasedInterruptTimeFunc)GetProcAddress(hDll, "QueryUnbiasedInterruptTime");
		functionResolved = 1;
	}

	if (QueryUnbiasedInterruptTimePtr)
	{
	ULONGLONG ticks;
		SHVVERIFY((*QueryUnbiasedInterruptTimePtr)(&ticks));
		return (long)(ticks/10000ll);
	}
	else
	{
		return ::GetTickCount();
	}
#elif defined(__SHIVA_EPOC)
TTimeIntervalMicroSeconds32 t = 1;
	UserHal::TickPeriod(t);
	return User::TickCount()*t.Int();
#elif defined(__SHIVA_POSIX_OSX)
clock_serv_t cclock;
mach_timespec_t n;

	host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cclock);
	if (clock_get_time(cclock, &n))
	{
		fprintf(stderr,"GETTICKCOUNT ERROR\n");
		abort();
	}
	mach_port_deallocate(mach_task_self(), cclock);
	
	return long(n.tv_sec*1000L) + long(n.tv_nsec/1000000L);
#elif _POSIX_TIMERS > 0
struct timespec n;
	if (clock_gettime(CLOCK_MONOTONIC_RAW, &n))
	{
		fprintf(stderr,"GETTICKCOUNT ERROR\n");
		abort();
	}
	return long(n.tv_sec*1000L) + long(n.tv_nsec/1000000L);
#else
# warning GetTicksInMilliSecs is not relative to system time
timeval val;
	gettimeofday(&val,NULL);
	return (val.tv_sec*1000)+(val.tv_usec/1000);
#endif
}

/*************************************
 * HasSystemPrivs
 *************************************/
/// Check for system level privileges
/**
 \return true if system privileges are present
 *
 * This function will always return true on windows and symbian
 * platforms.
 */
bool SHVThreadBase::HasSystemPrivs()
{
#ifdef __SHIVA_WIN32
	return true;
#elif defined(__SHIVA_EPOC)
	return true;
#else
	return getuid() == 0;
#endif
}

/*************************************
 * Startup funcs
 *************************************/
///\cond INTERNAL
#ifdef __SHIVA_WIN32
DWORD WINAPI SHVThreadBase::StartupFuncWin32(LPVOID d)
{
ThreadFunc func = (ThreadFunc)((void**)d)[0];
void* data = ((void**)d)[1];
	::free(d);

	func(data);

	return 0;
}
#elif defined(__SHIVA_EPOC)
TInt SHVThreadBase::StartupFuncEpoc(TAny* d)
{
ThreadFunc func = (ThreadFunc)((void**)d)[0];
void* data = ((void**)d)[1];
CTrapCleanup* CleanupStack = CTrapCleanup::New();
TInt err = 0;

	TRAP(err,func(data));
	delete CleanupStack;
	::free(d);

	if (err != KErrNone)
		User::Leave(err);

	return 0;
}
#else
void* SHVThreadBase::StartupFuncPosix(void* d)
{
ThreadFunc func = (ThreadFunc)((void**)d)[0];
void* data = ((void**)d)[1];
pthread_mutex_t* mutex = (pthread_mutex_t*)((void**)d)[2];

	pthread_mutex_lock(mutex);
	pthread_mutex_unlock(mutex);
	pthread_mutex_destroy(mutex);
	::free(mutex);
	::free(d);

	func(data);

	pthread_exit(NULL);
	return NULL;
}
#endif
///\endcond
