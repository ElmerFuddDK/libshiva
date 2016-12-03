#ifndef __SHIVA_THREADUTILS_SEMAPHORELOCKER_H
#define __SHIVA_THREADUTILS_SEMAPHORELOCKER_H

#include "shvsemaphore.h"

//-=========================================================================================================
///  SHVSemaphoreLocker class - convenience class to handle function locks
/**
 * Convenience class to handle function locks. See example.
\code
bool examplefunc(SHVSemaphore* sem)
{
SHVSemaphoreLocker lock(sem); // will wait for 1 resource in the semaphore
	doSomething();
	if (checkSomeThing())
		return false; // Will signal the waited resource in the semaphore on automatic destructor
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
	inline SHVSemaphoreLocker(const SHVSemaphoreLocker &l);
	inline const SHVSemaphoreLocker& operator =(const SHVSemaphoreLocker& l);
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
SHVSemaphoreLocker::SHVSemaphoreLocker(const SHVSemaphoreLocker& l) : Semaphore(((SHVSemaphoreLocker*)&l)->Semaphore), Resources(l.Resources)
{
	((SHVSemaphoreLocker*)&l)->Semaphore = NULL;
}

/*************************************
 * operator =
 *************************************/
const SHVSemaphoreLocker& SHVSemaphoreLocker::operator =(const SHVSemaphoreLocker& l)
{
	if (Semaphore)
		Semaphore->Signal(Resources);
	
	Semaphore = ((SHVSemaphoreLocker*)&l)->Semaphore;
	Resources = l.Resources;
	((SHVSemaphoreLocker*)&l)->Semaphore = NULL;
	
	return *this;
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
