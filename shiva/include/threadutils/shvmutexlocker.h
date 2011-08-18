#ifndef __SHIVA_THREADUTILS_MUTEXLOCKER_H
#define __SHIVA_THREADUTILS_MUTEXLOCKER_H

#include "shvmutex.h"

//-=========================================================================================================
///  SHVMutexLocker class - convenience class to handle function locks
/**
 * Convenience class to handle function locks. See example.
\code
	void examplefunc()
	{
	SHVMutexLocker lock(myLock);
		doSomething();
		if (checkSomeThing())
			return; // While result in "lock" being destroyed and mutex unlocked.
		doSomethingElse();
	}
\endcode
 */

class SHVMutexLocker
{
public:
	inline SHVMutexLocker(const SHVMutex* mutex): Mutex(*(SHVMutex*)mutex) { Mutex.Lock();   }
	inline SHVMutexLocker(const SHVMutex& mutex): Mutex(*((SHVMutex*)&mutex))  { Mutex.Lock();   }
	inline ~SHVMutexLocker()               { Mutex.Unlock(); }
private:
	SHVMutex& Mutex;
};



// ============================================== documentation ============================================= //

/// \class SHVMutexLocker shvmutexlocker.h "shiva/include/threadutils/shvmutexlocker.h"

#endif
