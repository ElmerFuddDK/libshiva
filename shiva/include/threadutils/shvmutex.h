#ifndef __SHIVA_THREADUTILS_MUTEX_H
#define __SHIVA_THREADUTILS_MUTEX_H

#include "shvmutexbase.h"
#include "shvthreadbase.h"

//-=========================================================================================================
///  SHVMutex class - thread aware, reference counting mutex
/**
 * SHVMutex is the safe locking class for mutexes, since it is thread aware and reference counted.
 * This means it is safe to lock it multiple times with the same thread, and unlock it the same amount of
 * times before it finally gets unlocked.
 */

class SHVAPI SHVMutex : SHVMutexBase
{
public:

	// defines
	enum { Infinite = -1 };


	SHVMutex();

	bool Lock(int timeout = Infinite);
	void Unlock();

	int UnlockAll();
	bool LockMultiple(int refCount, int timeout = Infinite);


	bool IsLocked();
	SHVThreadBase::ThreadID GetOwner();
	bool HasLock();
	int GetLockCount();


private:
	int RefCount;
	SHVThreadBase::ThreadID Owner;
};

#endif
