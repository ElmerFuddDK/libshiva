#ifndef __SHIVA_FRAMEWORK_TIMERIMPL_H
#define __SHIVA_FRAMEWORK_TIMERIMPL_H

#include "../../../include/framework/shvtimer.h"
#include "../../../include/framework/shveventthread.h"

#include "../../../include/utils/shvlist.h"
#include "../../../include/utils/shvrefobject.h"


class SHVTimerImpl;
///\cond INTERNAL
//=========================================================================================================
// SHVTimerThread class - The internal timer thread
//=========================================================================================================
class SHVAPI SHVTimerThread : public SHVEventThread
{
public:


	// constructor
	SHVTimerThread(SHVTimerImpl* timer);

	void ResetTimerThread();

protected:

	virtual void PreEventDispatch();

private:

	long LastTick;
	SHVTimerImpl* Timer;
};
///\endcond



//-=========================================================================================================
/// SHVTimerInstanceImpl class - interface for a timer
/**
 * Implements the timer instance interface
 */

class SHVAPI SHVTimerInstanceImpl : public SHVTimerInstance
{
friend class SHVTimerImpl;
friend class SHVTimerThread;
public:


	// constructor
	SHVTimerInstanceImpl(SHVTimerImpl* timer, SHVEventSubscriberBase* subscriber);


	// controlling the timer
	virtual void Set(SHVTimerInstance::Modes mode, int interval);
	virtual void SetAbsolute(const SHVTime& time);


	// properties
	virtual SHVTimerInstance::Modes GetMode();
	virtual int GetInterval();

private:
	///\cond INTERNAL
	SHVTimerImpl* Timer;

	SHVEventSubscriberBaseRef Subscriber;
	SHVTime AbsoluteTime;

	SHVTimerInstance::Modes Mode;
	int Interval;
	long TickHit;
	bool WrapAround; // true if the tick is lower than now


	bool PerformPending(long now, bool wrappedAround, long& waitInterval);
	void Perform();
	long CalculateWaitInterval(long now, SHVTime& timeNow);

	///\endcond
};
typedef SHVRefObjectContainer<SHVTimerInstanceImpl> SHVTimerInstanceImplRef;



//-=========================================================================================================
/// SHVTimerImpl module - Implementation of the timer module
/**
 * Implements the timer module.
 */

class SHVAPI SHVTimerImpl : public SHVTimer
{
friend class SHVTimerInstanceImpl;
friend class SHVTimerThread;
public:


	// constructor
	SHVTimerImpl(SHVModuleList& modules);


	// module functions
	virtual SHVBool Register();
	virtual void PostRegister();
	virtual void Unregister();


	// timer creation
	virtual SHVTimerInstance* CreateTimer(SHVEventSubscriberBase* subscriber);
	virtual void RemoveTimer(SHVTimerInstance* timer);


private:
	///\cond INTERNAL

	inline SHVModuleList& GetModuleList();

	SHVList<SHVTimerInstanceImplRef,SHVTimerInstanceImpl*> TimerList;

	SHVTimerThread TimerThread;

	void RecalculateTimer(SHVTimerInstanceImpl* timer);
	///\endcond
};



// ============================================ implementation ============================================ //

/*************************************
 * GetModuleList
 *************************************/
///\cond INTERNAL
SHVModuleList& SHVTimerImpl::GetModuleList()
{
	return Modules;
}
///\endcond

#endif
