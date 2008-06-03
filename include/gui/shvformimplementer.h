#ifndef __SHIVA_GUI_FORMIMPLEMENTER_H
#define __SHIVA_GUI_FORMIMPLEMENTER_H


#include "../../include/utils/shvptrcontainer.h"

class SHVGUIManager;


//-=========================================================================================================
/// SHVFormImplementer - Interface for default implementation of a form
/**
 */

class SHVFormImplementer
{
public:


	virtual void Show() = 0;
	virtual void Hide() = 0;
	virtual void Enable() = 0;
	virtual void Disable() = 0;

	virtual SHVBool GetData(SHVDataBinder* data) = 0;
	virtual SHVBool SetData(SHVDataBinder* data) = 0;

	virtual SHVString8C GetEntityName() = 0;

	virtual SHVGUIManager* GetManager() = 0;
	virtual SHVControlContainer* GetContainer() = 0;


	virtual ~SHVFormImplementer() {}
};
typedef SHVPtrContainer<SHVFormImplementer> SHVFormImplementerPtr;

#endif
