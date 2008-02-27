#ifndef __SHIVA_GUI_SHVFORM_H
#define __SHIVA_GUI_SHVFORM_H


#include "../../include/framework/shveventtargetdynamic.h"
#include "shvcontrol.h"
#include "shvcontrolcontainer.h"

//-=========================================================================================================
/// SHVForm - base form for coordinating 1..n ControlContainers/Controls
/**
 * This class is the basis of all forms in SHIVA.
 */

class SHVForm: public SHVEventTargetDynamic
{
public:
	SHVForm(SHVGUIManager* manager, SHVControlContainer* controlContainer, SHVString8C entityName);

	virtual void InitializeForm(SHVControlLayout* layout) = 0;
	virtual void InitializeForm();

	virtual void Show();
	virtual void Hide();
	virtual void Enable();
	virtual void Disable();

	virtual SHVBool GetData(SHVControlData* data);
	virtual SHVBool SetData(SHVControlData* data);

	virtual SHVString8C GetEntityName();

	virtual SHVBool PreClose();

protected:
	virtual void OnEvent(SHVEvent* event);
	virtual void OnPreDestroyEvent(SHVEvent* event);
	virtual void OnResizeForm(SHVControlContainer* container, SHVControlLayout* layout);

	SHVControlContainerRef ControlContainer;
	SHVString8C EntityName;
	SHVGUIManager* GUIManager;
};

typedef SHVRefObjectContainer<SHVForm> SHVFormRef;


#endif