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


//-=========================================================================================================
///  SHVMutexTempUnlocker class - convenience class to temporarily releasing all locks
/**
 * Convenience class to handle unlocks. See example.
\code
	void examplefunc()
	{
	SHVMutexLocker lock(myLock);
		doSomething();
		if (checkSomeThing())
		{
		SHVMutexTempUnlocker (myLock);
			// We are now unlocked within this scope
			doSomeUnlocked();
		}
		doSomethingElse(); // We are locked again
	}
\endcode
 */

class SHVMutexTempUnlocker
{
public:
	inline SHVMutexTempUnlocker(const SHVMutex* mutex): Mutex(*(SHVMutex*)mutex) { Count = Mutex.UnlockAll(); }
	inline SHVMutexTempUnlocker(const SHVMutex& mutex): Mutex(*((SHVMutex*)&mutex))  { Count = Mutex.UnlockAll(); }
	inline ~SHVMutexTempUnlocker() { Mutex.LockMultiple(Count); }
private:
	SHVMutex& Mutex;
	int Count;
};


// ============================================== documentation ============================================= //

/// \class SHVMutexLocker shvmutexlocker.h "shiva/include/threadutils/shvmutexlocker.h"
/// \class SHVMutexTempUnlocker shvmutexlocker.h "shiva/include/threadutils/shvmutexlocker.h"

#endif
