#include "stdafx.h"
#include "../../include/platformspc.h"


#include "../include/shvform.h"
#include "../../include/framework/shveventsubscriber.h"
#include "../../include/framework/shvevent.h"
#include "../../include/framework/shveventstructs.h"

//=========================================================================================================
// SHVForm - base form interface
//=========================================================================================================
SHVForm::SHVForm(SHVGUIManager* manager, SHVControlContainer* controlContainer, SHVString8C entityName): 
	GUIManager(manager), 
	ControlContainer(controlContainer),
	EntityName(entityName)
{
	ControlContainer->SubscribePreDestroy(new SHVEventSubscriberFunc<SHVForm>(this,&SHVForm::OnPreDestroyEvent));
}

/*************************************
 * InitializeForm
 *************************************/
void SHVForm::InitializeForm()
{
	InitializeForm(new SHVControlLayoutCallback<SHVForm>(this, &SHVForm::OnResizeForm));
}

/*************************************
 * Show
 *************************************/
void SHVForm::Show()
{
	ControlContainer->SetFlag(SHVControl::FlagVisible, true);
}

/*************************************
 * Hide
 *************************************/
void SHVForm::Hide()
{
	ControlContainer->SetFlag(SHVControl::FlagVisible, false);
}

/*************************************
 * Enable
 *************************************/
void SHVForm::Enable()
{
	ControlContainer->SetFlag(SHVControl::FlagDisabled, false);
}

/*************************************
 * Disable
 *************************************/
void SHVForm::Disable()
{
	ControlContainer->SetFlag(SHVControl::FlagDisabled, true);
}

/*************************************
 * GetData
 *************************************/
SHVBool SHVForm::GetData(SHVControlData* data)
{
	return ControlContainer->GetData(data);
}

/*************************************
 * SetData
 *************************************/
SHVBool SHVForm::SetData(SHVControlData* data)
{
	return ControlContainer->SetData(data);
}

/*************************************
 * GetEntityName
 *************************************/
SHVString8C SHVForm::GetEntityName()
{
	return EntityName;
}

/*************************************
 * PreClose
 *************************************/
/// Called before the control container is closed
/** 
 * Return false to prevent the window/dialog from closing
 */
SHVBool SHVForm::PreClose()
{
	return SHVBool(true);
}


/*************************************
 * OnEvent
 *************************************/
void SHVForm::OnEvent(SHVEvent* event)
{
}

/*************************************
 * OnPreDestroyEvent
 *************************************/
void SHVForm::OnPreDestroyEvent(SHVEvent* event)
{
	SHVEventDataBool::SetValue(event, PreClose());
}

/*************************************
 * OnResizeForm
 *************************************/
/// Override this to place the controls within the ControlContainer
/**
 * Default behaviour when no layout engine is used
 */
void SHVForm::OnResizeForm(SHVControlContainer* container, SHVControlLayout* layout)
{
}
