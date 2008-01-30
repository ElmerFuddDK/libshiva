#ifndef __SHIVA_FRAMEWORK_MODULE_H
#define __SHIVA_FRAMEWORK_MODULE_H

#include "shvmodulelist.h"
#include "shveventemitter.h"
#include "shveventsubscriber.h"


//-=========================================================================================================
/// SHVModule class - base implementation of a module
/**
 * You should normally inherit from this class for creating modules. It contains event dispatcher stuff
 * as well as a default implementation of all virtuals.
 */

class SHVAPI SHVModule : public SHVModuleBase
{
public:


	// Register/Deregister stuff
	virtual SHVBool Register();
	virtual void PostRegister();
	virtual void PreUnregister();
	virtual void Unregister();


	// properties
	virtual SHVBool IsRegistered();


	// Event stuff
	virtual void EventSubscribe(SHVEvent* event, SHVEventSubscriberBase* subscriber);
	void EventSubscribe(SHVInt id, SHVEventSubscriberBase* subscriber);
	void EventSubscribe(const SHVChar* name, SHVEventSubscriberBase* subscriber);
	virtual void OnEvent(SHVEvent* event);


	inline SHVModuleList& GetModuleList();


protected:
	SHVModule(SHVModuleList& modules, const SHVChar* name, const SHVChar* group = NULL);

	bool EmitEvent(SHVEvent* event);

	SHVModuleList& Modules;
	SHVEventEmitter Events;

private:
	///\cond INTERNAL
	bool Registered;
	///\endcond
};



// ============================================= implementation ============================================= //

/*************************************
 * GetModuleList
 *************************************/
SHVModuleList& SHVModule::GetModuleList()
{
	return Modules;
}

#endif
