#ifndef __SHIVA_FRAMEWORK_EVENTTHREADPOOL_H
#define __SHIVA_FRAMEWORK_EVENTTHREADPOOL_H

#include "shvmodulelist.h"
#include "shveventqueuelist.h"

#include "../threadutils/shvthread.h"
#include "../threadutils/shvthreadpool.h"

///\cond INTERNAL
class SHVEventThreadPool;
class SHVEventQueueListThreadPool : public SHVEventQueueList
{
private:
friend class SHVEventThreadPool;

	SHVEventThreadPool* Owner;

	inline SHVEventQueueListThreadPool(SHVEventThreadPool* owner) : Owner(owner) {}
	virtual void PerformEvent(SHVModuleList& modules, EventEntryPtr& entry);
	inline SHVBool EventActiveInQueue(SHVModuleList& modules) { return SHVEventQueueList::EventActiveInQueue(modules); }
	inline void EventDeactivatedInQueue(SHVModuleList& modules) { SHVEventQueueList::EventDeactivatedInQueue(modules); }
};
///\endcond


//-=========================================================================================================
/// SHVEventThreadPool class - Implementation of an event thread pool
/**
 * Implements an event thread pool. It can be derived for special usage.\n
 * It is generally encouraged to always use an event thread when implementing worker threads
 * in SHIVA, since events are integrated into the entire framework. That way you don't have to
 * worry about shutting down the thread.\n
 * The event thread pool does not have a specific thread assigned to it, hence the GetThreadID
 * method will return SHVThreadBase::InvalidThreadID.\n
 * Threads will be started as events are enqueued, up to the maxCount assigned at event thread
 * startup. Each time a new thread is started the ThreadStartingSubscriber is emitted.
 */

class SHVAPI SHVEventThreadPool : public SHVEventQueue
{
public:


	enum Events {
		EventStateStarting,
		EventStateStopping,
		EventPoolThreadStarting
	};


	// constructor
	SHVEventThreadPool(SHVEventSubscriberBase* stateSubscriber = NULL);
	~SHVEventThreadPool();


	// ThreadStarting event subscriber
	void SetThreadStartingSubscriber(SHVEventSubscriberBase* subs);


	// thread handling
	void Start(SHVModuleList& modules, int maxCount = 25, short threadPriority = SHVThreadBase::PrioNormal);
	void Stop();
	void WaitForTermination();


	// EventQueue functions
	virtual void EnqueueEvent(SHVEvent* event, SHVEventSubscriberBase* subscriber);
	virtual void SignalDispatcher();
	virtual SHVThreadBase::ThreadID GetThreadID();

	virtual SHVBool LockEvent();
	virtual void UnlockEvent();


protected:

	inline SHVModuleList* GetModuleList();

	void SetSignalTimeout(int interval=SHVMutexBase::Infinite);

	// stubbed functions for inheritance
	virtual void ThreadStarting();
	virtual void ThreadStopping();
	virtual void PreEventDispatch();
	virtual void PostEventDispatch();

private:
	///\cond INTERNAL
friend class SHVEventQueueListThreadPool;

	SHVModuleList* Modules;

	SHVMutexBase ThreadSignal;
	SHVThread<SHVEventThreadPool> EventThread;
	SHVThreadPool<SHVEventThreadPool,SHVEventQueueListThreadPool::EventEntry*> ThreadPool;
	bool KillSignal;
	int TimeoutInterval;

	SHVEventQueueListThreadPool EventList;
	SHVEventSubscriberBaseRef StateSubscriber;
	SHVEventSubscriberBaseRef ThreadStartingSubscriber;

	void ThreadFunc();
	void PoolThreadStarting();
	void PoolEmitEvent(SHVEventQueueListThreadPool::EventEntry* entry);

	///\endcond
};


// ============================================= implementation ============================================= //

/*************************************
 * GetModuleList
 *************************************/
SHVModuleList* SHVEventThreadPool::GetModuleList()
{
	return Modules;
}

#endif
