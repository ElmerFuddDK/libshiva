#ifndef __SHIVA_FRAMEWORK_EVENTTARGETDYNAMIC_H
#define __SHIVA_FRAMEWORK_EVENTTARGETDYNAMIC_H

#include "shvevent.h"


//-=========================================================================================================
/// SHVEventTargetDynamic class - abstract for implementing dynamic event targets
/**
 * Inherit from this class when implementing an event target that is a dynamic
 * object. Dynamic objects should never use ordinary subscribers unless they
 * share lifetime with the objects they are used within. For instance it is
 * Ok for a dynamic target to subscribe to an event in another dynamic target,
 * but only if the other target is quaranteed destroyed before the first one.
 */

class SHVAPI SHVEventTargetDynamic : public SHVRefObject, public SHVEventTarget
{
protected:
friend class SHVEventSubscriberDynamic;

	// From SHVEventTarget
	virtual void OnEvent(SHVEvent* event) = 0;

	inline SHVEventTargetDynamic() {}
};
typedef SHVRefObjectContainer<SHVEventTargetDynamic> SHVEventTargetDynamicRef;

#endif
