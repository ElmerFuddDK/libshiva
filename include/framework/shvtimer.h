#ifndef __SHIVA_FRAMEWORK_TIMER_H
#define __SHIVA_FRAMEWORK_TIMER_H

#include "shvmodule.h"
#include "../utils/shvrefobject.h"


//-=========================================================================================================
/// SHVTimerInstance class - interface for a timer
/**
 * This interface is used when a timer is needed. It provides functions for controlling a timer.
 */

class SHVAPI SHVTimerInstance : public SHVRefObject
{
public:


	// defines
	enum Modes {
		ModeStopped = 0,
		ModeOnce,
		ModeRecurring
	};


	// controlling the timer
	virtual void Set(Modes mode, int interval) = 0;


	// properties
	virtual Modes GetMode() = 0;
	virtual int GetInterval() = 0;

};
typedef SHVRefObjectContainer<SHVTimerInstance> SHVTimerInstanceRef;



//-=========================================================================================================
/// SHVTimer module - The framework module for handling timer events
/**
 * Interface for the timer module. This module is the only module that is part of the framework,
 * because it is used by the framework itself.
 */

class SHVAPI SHVTimer : public SHVModule
{
public:


	// defines
	enum Events {
		EventTimeout
	};


	// timer creation
	virtual SHVTimerInstance* CreateTimer(SHVEventSubscriberBase* subscriber) = 0;
	virtual void RemoveTimer(SHVTimerInstance* timer) = 0;


protected:
	inline SHVTimer(SHVModuleList& modules);
};



// ============================================= implementation ============================================= //

/*************************************
 * Constructor
 *************************************/
SHVTimer::SHVTimer(SHVModuleList& modules) : SHVModule(modules,"Timer")
{}

#endif
