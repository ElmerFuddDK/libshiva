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
	virtual void SetupDefaults(SHVModuleList& modules) = 0;

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
/// \class SHVMainThreadEventDispatcher shvmainthreadeventdispatcher.h "shiva/include/framework/shvmainthreadeventdispatcher.h"

/** \fn void SHVMainThreadEventDispatcher::SetupDefaults(SHVModuleList& modules)
 \brief Called when the event queue wants to set up default values
 *
 * You will need to set up the default variables in the default config
 * here. Currently there is 2 mandatory values that needs to be resolved:\n
 * SHVModuleList::DefaultCfgAppPath and SHVModuleList::DefaultCfgAppName.\n
 * Application path is normally the location of the application and
 * application name is normally the name of the executable.
 */

/** \fn void SHVMainThreadEventDispatcher::SignalDispatcher()
 \brief Signals the main thread event queue for dispatching
 *
 * In this function you will need to implement a signal to the main
 * thread to execute DispatchEvents.
 */

/** \fn SHVBool SHVMainThreadEventDispatcher::InitializeEventLoop()
 \brief Is called before the application is started
 \return false if initalization failed
 *
 * When implementing a main thread event queue you can put code here to
 * initialize data used by the event loop and startup process.\n
 * On GUI based platforms this could be the main GUI window.\n
 * If an error should occur you must return false or an error value.
 */

/** \fn void SHVMainThreadEventDispatcher::RunEventLoop()
 \brief Runs the event loop
 *
 * In this function you must implement the main event loop. Use
 * the Running() function to query when to exit the event loop.
 */

/** \fn void SHVMainThreadEventDispatcher::StopEventLoop(SHVBool errors)
 \brief Is called in order to signal the event loop to stop
 \param errors Indicates whether we are stopping because of errors
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
