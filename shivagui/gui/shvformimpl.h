#ifndef __SHIVA_GUIIMPL_FORM_H
#define __SHIVA_GUIIMPL_FORM_H

#include "../include/shvformbase.h"
#include "../include/shvformimplementer.h"
#include "../include/shvguimanager.h"


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

	virtual SHVBool GetData(SHVControlData* data);
	virtual SHVBool SetData(SHVControlData* data);

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