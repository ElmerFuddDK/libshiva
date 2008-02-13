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
	SHVMutexLock lock(myLock);
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
	SHVMutexLocker(SHVMutex* mutex): Mutex(*mutex) { Mutex.Lock();   }
	SHVMutexLocker(SHVMutex& mutex): Mutex(mutex)  { Mutex.Lock();   }
	~SHVMutexLocker()               { Mutex.Unlock(); }
private:
	SHVMutex& Mutex;
};

#endif