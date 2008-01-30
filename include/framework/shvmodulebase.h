#ifndef __SHIVA_FRAMEWORK_MODULEBASE_H
#define __SHIVA_FRAMEWORK_MODULEBASE_H

#include "shveventsubscriberbase.h"
#include "../utils/shvptrcontainer.h"


//-=========================================================================================================
/// SHVModuleBase class - interface for modules
/**
 * Contains the basic interface for modules. It implements the name and type functionality.
 */

class SHVAPI SHVModuleBase : public SHVEventTarget
{
public:


	// destructor
	virtual ~SHVModuleBase() {}


	// Register/Deregister stuff
	virtual SHVBool Register() = 0;
	virtual void PostRegister() = 0;
	virtual void PreUnregister() = 0;
	virtual void Unregister() = 0;


	// properties
	inline const SHVString8C& GetName();
	inline const SHVString8C& GetGroup();
	virtual SHVBool IsRegistered() = 0;


	virtual void EventSubscribe(SHVEvent* event, SHVEventSubscriberBase* subscriber) = 0;
	inline void EventSubscribe(SHVInt id, SHVEventSubscriberBase* subscriber);
	inline void EventSubscribe(const SHVChar* name, SHVEventSubscriberBase* subscriber);


	// from SHVEventTarget
protected:
	virtual void OnEvent(SHVEvent* event) = 0;


protected:
	inline SHVModuleBase(const SHVChar* name, const SHVChar* group = NULL);

	///\cond INTERNAL
	SHVString8C ModuleName, ModuleGroup;
	///\endcond
};
typedef SHVPtrContainer<SHVModuleBase> SHVModuleBasePtr;



// ============================================= implementation ============================================= //

/*************************************
 * Constructor
 *************************************/
/// Constructor of SHVModuleBase
/**
 \param name Contains the unique name of the module
 \param group Contains an optional group name of the module
 */
SHVModuleBase::SHVModuleBase(const SHVChar* name, const SHVChar* group) : ModuleName(name), ModuleGroup(group)
{}


/*************************************
 * GetName
 *************************************/
/// Returns name identifying the module
const SHVString8C& SHVModuleBase::GetName()
{
	return ModuleName;
}

/*************************************
 * GetGroup
 *************************************/
/// Returns the group this module is a part of
const SHVString8C& SHVModuleBase::GetGroup()
{
	return ModuleGroup;
}

/*************************************
 * EventSubscribe
 *************************************/
/// Event subscription overload
/**
 \see SHVModuleBase::EventSubscribe(SHVEvent* event, SHVEventSubscriberBase* subscriber)
 */
void SHVModuleBase::EventSubscribe(SHVInt id, SHVEventSubscriberBase* subscriber)
{
	EventSubscribe(new SHVEvent(NULL,id), subscriber);
}
/// Event subscription overload
/**
 \see SHVModuleBase::EventSubscribe(SHVEvent* event, SHVEventSubscriberBase* subscriber)
 */
void SHVModuleBase::EventSubscribe(const SHVChar* name, SHVEventSubscriberBase* subscriber)
{
	EventSubscribe(new SHVEventString(NULL,name), subscriber);
}



// ============================================== documentation ============================================= //

/** \fn void SHVModuleBase::EventSubscribe(SHVEvent* event, SHVEventSubscriberBase* subscriber)
 \brief Is called in order to subscribe on an event this module emits
 \param event Event object identifying the event to be emitted
 \param subscriber Pointer to subscriber to be used when emitting
 */

#endif
