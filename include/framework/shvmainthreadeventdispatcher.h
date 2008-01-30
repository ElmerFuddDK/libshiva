#ifndef __SHIVA_FRAMEWORK_MAINTHREADEVENTDISPATCHER_H
#define __SHIVA_FRAMEWORK_MAINTHREADEVENTDISPATCHER_H

#include "../frameworkimpl/shvmainthreadeventqueue.h"

//-=========================================================================================================
/// SHVMainThreadEventDispatcher class - Interface for the main thread event dispatcher
/**
 * Implement this class to control the event loop and dispatching for the main thread.
 */

class SHVAPI SHVMainThreadEventDispatcher : public SHVEventTarget
{
public:

	virtual ~SHVMainThreadEventDispatcher() {}

	inline void SetEventQueue(SHVMainThreadEventQueue* eventqueue);

	virtual void SignalDispatcher() = 0;

	virtual SHVBool InitializeEventLoop() = 0;
	virtual void RunEventLoop() = 0;
	virtual void StopEventLoop(SHVBool errors) = 0;


protected:
	inline SHVMainThreadEventDispatcher();

	virtual void OnEvent(SHVEvent* event) = 0;
	inline void DispatchEvents();
	inline SHVBool Running();

	SHVMainThreadEventQueue* Queue;
};



// ============================================= implementation ============================================= //

/** \fn void SHVMainThreadEventDispatcher::SignalDispatcher()
 \brief Signals the main thread event queue for dispatching
 *
 * In this function you will need to implement a signal to the main
 * thread to execute DispatchEvents.
 */

/** \fn void SHVMainThreadEventDispatcher::InitializeEventLoop()
 \brief Is called before the application is started
 *
 * Use this function to initialize data used by the event loop
 * just before the application is started.
 */

/** \fn void SHVMainThreadEventDispatcher::RunEventLoop()
 \brief Runs the event loop
 *
 * In this function you must implement the main event loop. Use
 * the Running() function to query when to exit the event loop.
 */

/** \fn void SHVMainThreadEventDispatcher::StopEventLoop()
 \brief Is called in order to signal the event loop to stop
 *
 */

/*************************************
 * Constructor
 *************************************/
SHVMainThreadEventDispatcher::SHVMainThreadEventDispatcher()
{
	Queue = NULL;
}

/*************************************
 * SetEventQueue
 *************************************/
/// Sets the event queue
void SHVMainThreadEventDispatcher::SetEventQueue(SHVMainThreadEventQueue* eventqueue)
{
	Queue = eventqueue;
}

/*************************************
 * DispatchEvents
 *************************************/
/// Dispatches the events in the queue
void SHVMainThreadEventDispatcher::DispatchEvents()
{
	if (Queue->Running)
		Queue->EventList.DispatchEvents(Queue->GetModuleList());
}

/*************************************
 * Running
 *************************************/
/// Returns running state for the event loop
SHVBool SHVMainThreadEventDispatcher::Running()
{
	return Queue->Running;
}

#endif
