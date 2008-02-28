#include "stdafx.h"
#include "../../include/platformspc.h"


#include "shvformimpl.h"
#include "../../include/framework/shveventsubscriber.h"
#include "../../include/framework/shvevent.h"
#include "../../include/framework/shveventstructs.h"

//=========================================================================================================
// SHVFormImpl - base form interface
//=========================================================================================================
SHVFormImpl::SHVFormImpl(SHVFormBase* owner, SHVGUIManager* manager, SHVControlContainer* controlContainer, SHVString8C entityName): 
	Owner(owner),
	GUIManager(manager), 
	ControlContainer(controlContainer),
	EntityName(entityName)
{
	ControlContainer->SubscribePreDestroy(new SHVEventSubscriberFunc<SHVFormImpl>(this,&SHVFormImpl::OnPreDestroyEvent));
}

/*************************************
 * Show
 *************************************/
void SHVFormImpl::Show()
{
	ControlContainer->SetFlag(SHVControl::FlagVisible, true);
}

/*************************************
 * Hide
 *************************************/
void SHVFormImpl::Hide()
{
	ControlContainer->SetFlag(SHVControl::FlagVisible, false);
}

/*************************************
 * Enable
 *************************************/
void SHVFormImpl::Enable()
{
	ControlContainer->SetFlag(SHVControl::FlagDisabled, false);
}

/*************************************
 * Disable
 *************************************/
void SHVFormImpl::Disable()
{
	ControlContainer->SetFlag(SHVControl::FlagDisabled, true);
}

/*************************************
 * GetData
 *************************************/
SHVBool SHVFormImpl::GetData(SHVControlData* data)
{
	return ControlContainer->GetData(data);
}

/*************************************
 * SetData
 *************************************/
SHVBool SHVFormImpl::SetData(SHVControlData* data)
{
	return ControlContainer->SetData(data);
}

/*************************************
 * GetEntityName
 *************************************/
SHVString8C SHVFormImpl::GetEntityName()
{
	return EntityName;
}

/*************************************
 * GetManager
 *************************************/
SHVGUIManager* SHVFormImpl::GetManager()
{
	return GUIManager;
}

/*************************************
 * GetContainer
 *************************************/
SHVControlContainer* SHVFormImpl::GetContainer()
{
	return ControlContainer;
}

/*************************************
 * OnPreDestroyEvent
 *************************************/
void SHVFormImpl::OnPreDestroyEvent(SHVEvent* event)
{
	SHVEventDataBool::SetValue(event, Owner->PreClose());
}
