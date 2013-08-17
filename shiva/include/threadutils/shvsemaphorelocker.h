#ifndef __SHIVA_THREADUTILS_SEMAPHORELOCKER_H
#define __SHIVA_THREADUTILS_SEMAPHORELOCKER_H

#include "shvsemaphore.h"

//-=========================================================================================================
///  SHVSemaphoreLocker class - convenience class to handle function locks
/**
 * Convenience class to handle function locks. See example.
\code
bool examplefunc()
{
SHVSemaphoreLocker lock(mySemaphore);
	doSomething();
	if (checkSomeThing())
		return false; // Will signal the waited semaphore in the locker on automatic destructor
	return doSomethingElse();
}
\endcode
 \note If NULL is given on the constructor is will simply do nothing, supporting a runtime optional resource lock
 */

class SHVSemaphoreLocker
{
public:
	inline SHVSemaphoreLocker(const SHVSemaphore* semaphore, unsigned resources = 1);
	inline SHVSemaphoreLocker(const SHVSemaphore& semaphore, unsigned resources = 1);
	inline ~SHVSemaphoreLocker();
private:
	SHVSemaphore* Semaphore;
	int Resources;
};



// ============================================== documentation ============================================= //

/// \class SHVSemaphoreLocker shvsemaphorelocker.h "shiva/include/threadutils/shvsemaphorelocker.h"


/*************************************
 * Constructor
 *************************************/
SHVSemaphoreLocker::SHVSemaphoreLocker(const SHVSemaphore* semaphore, unsigned resources) : Semaphore((SHVSemaphore*)semaphore), Resources(resources)
{
	if (Semaphore)
		Semaphore->Wait(Resources);
}
SHVSemaphoreLocker::SHVSemaphoreLocker(const SHVSemaphore& semaphore, unsigned resources) : Semaphore((SHVSemaphore*)&semaphore), Resources(resources)
{
	if (Semaphore)
		Semaphore->Wait(Resources);
}

/*************************************
 * Destructor
 *************************************/
SHVSemaphoreLocker::~SHVSemaphoreLocker()
{
	if (Semaphore)
		Semaphore->Signal(Resources);
}

#endif
