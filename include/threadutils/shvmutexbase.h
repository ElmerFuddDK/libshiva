#ifndef __SHIVA_THREADUTILS_MUTEXBASE_H
#define __SHIVA_THREADUTILS_MUTEXBASE_H

#ifdef __SHIVA_WIN32
# include "../utils/shvlist.h"
#elif defined(__SHIVA_EPOC)
# include <e32std.h>
# include "../utils/shvlist.h"
#else
# include "../utils/shvlist.h"
# include <pthread.h>
#endif


//-=========================================================================================================
///  SHVMutexBase class - Non-thread aware locking class
/**
 * SHVMutexBase is the base class for all locking classes in SHIVA.
 * It is non-thread aware - meaning you can lock it in one thread, and unlock it in another. Thus all
 * functions are atomic.
 * Furthermore one should note that it it NOT reference counted. For this one should use the SHVMutex class.
 */

class SHVAPI SHVMutexBase
{
public:

	// defines
	enum { Infinite = -1 };
	enum { InvalidThreadID = -1 };  ///<\todo investigate invalid thread ID on various platforms


	SHVMutexBase();
	~SHVMutexBase();

	bool Lock(int timeout = Infinite);
	void Unlock();

	bool IsLocked();


private:
	///\cond INTERNAL
#ifdef __SHIVA_WIN32
	HANDLE Locker;
	SHVList<HANDLE,HANDLE> Queue;
	int Mutex;
	HANDLE NewOwner;
#elif defined(__SHIVA_EPOC)
	typedef struct SHVQueuedThread {
		RTimer* Timer; // NULL means infinite
		RMutex* Mutex; // NULL means finite
		TThreadId Owner;
	};

	SHVList<SHVQueuedThread,SHVQueuedThread&> Queue;
	RMutex Locker;
	int Mutex;
	void* NewOwner;
#else
	int Mutex;
	SHVList<pthread_cond_t*,pthread_cond_t*> Queue;
	pthread_mutex_t Locker;
	pthread_cond_t* NewOwner;
#endif
	///\endcond
};

#endif
