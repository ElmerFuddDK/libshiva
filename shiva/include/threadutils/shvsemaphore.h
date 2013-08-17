#ifndef __SHIVA_THREADUTILS_SEMAPHORE_H
#define __SHIVA_THREADUTILS_SEMAPHORE_H

#include "shvmutex.h"
#include "../utils/shvlist.h"

//-=========================================================================================================
///  SHVSemaphore class
/**
 * This is the SHIVA semaphore class. On its constructor it takes the amount of resources
 * available. Set it to 0 if you want it to wait from the first Wait command.\n
 * Calling Signal will increment the resource count and Wait will decrement it. If the
 * resource count is insufficient on Wait then it will wait until they are available from
 * a subsequent Signal call.
 */

class SHVAPI SHVSemaphore
{
public:


	SHVSemaphore(unsigned resourceCount);
	~SHVSemaphore();

	void Lock();
	void Unlock();


	void Wait(unsigned resources = 1);
	void Signal(unsigned resources = 1);
	int GetResourceCount();


private:
	
	struct WaitLock {
		SHVMutexBase Lock;
		unsigned ResourceCount;
		inline WaitLock(unsigned resourceCount) : ResourceCount(resourceCount) { Lock.Lock(); }
	};
	
	SHVList<WaitLock*> WaitQueue;
	
	unsigned ResourceCount;
	SHVMutex SignalLock;
};

#endif
