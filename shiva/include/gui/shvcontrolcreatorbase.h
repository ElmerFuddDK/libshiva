#ifndef __SHIVA_GUI_CONTROLCREATORBASE_H
#define __SHIVA_GUI_CONTROLCREATORBASE_H

#include "../../include/utils/shvrefobject.h"
#include "shvcontrol.h"


// forward declare
class SHVGUIManager;

//-=========================================================================================================
/// SHVControlCreatorBase - Interface for creating a control
/**
 * The safest way to use this interface is to use the SHVControlCreator template.
 */
 
class SHVControlCreatorBase : public SHVRefObject
{
public:

	// creation method
	virtual SHVControl* New(SHVGUIManager* manager) = 0;

protected:
	inline SHVControlCreatorBase() : SHVRefObject() {}
};
typedef SHVRefObjectContainer<SHVControlCreatorBase> SHVControlCreatorBaseRef;

#endif
