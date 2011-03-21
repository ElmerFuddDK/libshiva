#ifndef __SHIVA_THREADUTILS_READWRITELOCK_H
#define __SHIVA_THREADUTILS_READWRITELOCK_H

#include "shvmutexbase.h"
#include "shvthreadbase.h"
#include "../utils/shvlist.h"



//-=========================================================================================================
///  SHVReadWriteLock class
/**
 */

class SHVAPI SHVReadWriteLock
{
public:


	// Constructor
	SHVReadWriteLock();


	void LockShared();
	void LockExclusive();
	SHVBool TryLockExclusive();
	void Unlock();

	bool IsExclusiveLocked();
	bool HasExclusiveLock();

private:

	///\cond INTERNAL
	struct Wait
	{
		SHVMutexBase& WaitObject;
		SHVThreadBase::ThreadID NewOwner;

		inline Wait(SHVMutexBase& waitObject, SHVThreadBase::ThreadID newOwner) : WaitObject(waitObject), NewOwner(newOwner) {}
	};
	SHVList<Wait> WaitQueue;

	SHVMutexBase Lock;
	SHVThreadBase::ThreadID ExclusiveOwner;
	int RefCount;
	///\endcond
};

#endif
