#ifndef __SHIVA_THREADUTILS_THREADBASE_H
#define __SHIVA_THREADUTILS_THREADBASE_H

#include "../utils/shvstring.h"

#define SHVTHREADCHECK(x) SHVASSERT(x == SHVThreadBase::GetCurrentThreadID())


//-=========================================================================================================
///  SHVThreadBase class
/**
 * SHVThreadBase is a threading class for starting static functions in a seperate thread. It also contains
 * a few static functions for gaining basic thread info/control.\n
 * It is intended to be a base class for other thread classes. If you want a simple threading class for
 * worker threads, use SHVThread.\n
 * If you do use this class, remember to call ResetHandle() when the thread function terminates, otherwise
 * it will crash on some systems. This is taken care of in SHVThread, but intentionally left up to the user
 * here, in order to support restarting the thread from itself.
 */

class SHVAPI SHVThreadBase
{
public:

	// defines
	typedef void (*ThreadFunc)(void*);
	typedef size_t ThreadID;

	enum {
		PrioIdle    = -3,
		PrioLowest  = -2,
		PrioLow     = -1,
		PrioNormal  = 0,
		PrioHigh    = 1,
		PrioHighest = 2,
		PrioRealtime = 3
	};


	SHVThreadBase();
	~SHVThreadBase();

	bool Start(ThreadFunc func, void* data, short priority = PrioNormal, const SHVStringC& name = NULL);
	bool Kill();

	bool IsRunning();
	ThreadID GetThreadID() const;

	void ResetHandle();

	static void Sleep(int millisecs);
	static ThreadID GetCurrentThreadID();
	static long GetTickCount();
	static long GetTicksInMilliSecs();
	static bool HasSystemPrivs();

private:
	///\cond INTERNAL
#ifdef __SHIVA_WIN32
	HANDLE ThreadHandle;
	static DWORD WINAPI StartupFuncWin32(LPVOID d);
#elif defined(__SHIVA_EPOC)
	RThread ThreadHandle;
	bool Running;
	static TInt StartupFuncEpoc(TAny* d);
#else
	int ThreadHandle;
	static void* StartupFuncPosix(void* d);
#endif
	ThreadID ID;
	///\endcond
};

#endif
