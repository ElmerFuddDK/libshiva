#ifndef __SHIVA_FRAMEWORK_MODULEFACTORY_H
#define __SHIVA_FRAMEWORK_MODULEFACTORY_H

#include "shvmodule.h"


//-=========================================================================================================
/// SHVModuleFactory class - interface for instantiating modules from a module library
/**
 * Contains the basic interface for instantiating modules from a module library.
 */

class SHVAPI SHVModuleFactory : public SHVEventTarget
{
public:


	// destructor
	virtual ~SHVModuleFactory() {}

	// Version check
	virtual SHVBool CheckVersion(int verMajor,int verMinor,int verRelease) = 0;


	// Module instantiation
	virtual SHVBool ResolveModules(const SHVStringC symbol) = 0;


	// from SHVEventTarget
protected:
	virtual void OnEvent(SHVEvent* event) = 0;
};
typedef SHVPtrContainer<SHVModuleFactory> SHVModuleFactoryPtr;



// ============================================== documentation ============================================= //

/** \fn void SHVModuleBase::EventSubscribe(SHVEvent* event, SHVEventSubscriberBase* subscriber)
 \brief Is called in order to subscribe on an event this module emits
 \param event Event object identifying the event to be emitted
 \param subscriber Pointer to subscriber to be used when emitting
 */

#endif
