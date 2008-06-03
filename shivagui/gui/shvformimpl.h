#ifndef __SHIVA_GUIIMPL_FORM_H
#define __SHIVA_GUIIMPL_FORM_H

#include "../../include/gui/shvformbase.h"
#include "../../include/gui/shvformimplementer.h"
#include "../../include/gui/shvguimanager.h"


//-=========================================================================================================
/// SHVFormImpl - implementation of a form implementer
/**
 */

class SHVFormImpl : public SHVFormImplementer
{
public:

	// Constructor
	SHVFormImpl(SHVFormBase* owner, SHVGUIManager* manager, SHVControlContainer* controlContainer, SHVString8C entityName);


	virtual void Show();
	virtual void Hide();
	virtual void Enable();
	virtual void Disable();

	virtual SHVBool GetData(SHVDataBinder* data);
	virtual SHVBool SetData(SHVDataBinder* data);

	virtual SHVString8C GetEntityName();

	virtual SHVGUIManager* GetManager();
	virtual SHVControlContainer* GetContainer();

private:
	void OnPreDestroyEvent(SHVEvent* event);

	SHVFormBase* Owner;
	SHVGUIManager* GUIManager;
	SHVString8C EntityName;
	SHVControlContainerRef ControlContainer;
};

#endif
