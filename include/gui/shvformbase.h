#ifndef __SHIVA_GUI_FORMBASE_H
#define __SHIVA_GUI_FORMBASE_H

#include "../../include/framework/shveventtargetdynamic.h"
#include "shvcontrollayout.h"


//-=========================================================================================================
/// SHVFormBase - Interface for a base form
/**
 * This class is the basis of all forms in SHIVA.
 */

class SHVFormBase : public SHVEventTargetDynamic
{
public:

	virtual void InitializeForm() = 0;

	virtual void Show() = 0;
	virtual void Hide() = 0;
	virtual void Enable() = 0;
	virtual void Disable() = 0;

	virtual SHVBool GetData(SHVControlData* data) = 0;
	virtual SHVBool SetData(SHVControlData* data) = 0;

	virtual SHVString8C GetEntityName() = 0;

	virtual SHVBool PreClose() = 0;

};
typedef SHVRefObjectContainer<SHVFormBase> SHVFormBaseRef;


#endif
