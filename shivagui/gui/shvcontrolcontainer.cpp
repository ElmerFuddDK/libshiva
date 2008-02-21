#include "stdafx.h"
#include "../../include/platformspc.h"

#include "../include/shvcontrolcontainer.h"
#include "shvguimanagerimpl.h"
#include "../../include/framework/shveventstructs.h"

//=========================================================================================================
// SHVControlContainer - base control container class
//=========================================================================================================

/*************************************
 * Destructor
 *************************************/
SHVControlContainer::~SHVControlContainer()
{
	Clear();
	GetImplementor()->Destroy(this);
}

/*************************************
 * GetType
 *************************************/
int SHVControlContainer::GetType()
{
	return SHVControl::TypeContainer;
}

/*************************************
 * Create
 *************************************/
SHVBool SHVControlContainer::Create()
{
SHVBool retVal;
SHVGUIManagerImpl* manager = (SHVGUIManagerImpl*)GetManager();

	SHVASSERT(GetImplementor()->GetSubType(this) == SHVControlContainer::SubTypeMainWindow || 
		GetImplementor()->GetSubType(this) == SHVControlContainer::SubTypeDialog || 
		GetImplementor()->GetSubType(this) == SHVControlContainer::SubTypeModalDialog );

	retVal = GetImplementor()->Create(this, NULL, SHVControl::FlagVisible);

	if (retVal)
	{
		// if we are in modal mode, set modal mode
		if (GetImplementor()->GetSubType(this) == SHVControlContainer::SubTypeDialog)
		{
			if (manager->TopLevelModalDialogs.GetCount())
				SetModalMode(true);

			manager->TopLevelDialogs.AddTail(this);
		}
		else if (GetImplementor()->GetSubType(this) == SHVControlContainer::SubTypeModalDialog)
		{
			if (!manager->TopLevelModalDialogs.GetCount())
			{
			SHVListIterator<SHVControlContainer*> itr(manager->TopLevelDialogs);

				manager->MainWindow->SetModalMode(true);

				while (itr.MoveNext())
					itr.Get()->SetModalMode(true);
			}

			manager->TopLevelModalDialogs.AddTail(this);
		}
	}

	return retVal;
}

/*************************************
 * Close
 *************************************/
SHVBool SHVControlContainer::Close()
{
SHVBool retVal;
SHVGUIManagerImpl* manager = (SHVGUIManagerImpl*)GetManager();

	SHVASSERT(GetImplementor()->GetSubType(this) == SHVControlContainer::SubTypeMainWindow || 
		GetImplementor()->GetSubType(this) == SHVControlContainer::SubTypeDialog || 
		GetImplementor()->GetSubType(this) == SHVControlContainer::SubTypeModalDialog );

	retVal = ( IsCreated() ? GetImplementor()->Destroy(this) : SHVBool::True );

	if (!IsCreated())
	{
		// if we are in modal mode, set modal mode
		if (GetImplementor()->GetSubType(this) == SHVControlContainer::SubTypeDialog)
		{
		SHVListPos pos = manager->TopLevelDialogs.Find(this);

			ModalMode = ModalNone;

			if (pos)
				manager->TopLevelDialogs.RemoveAt(pos);
		}
		else if (GetImplementor()->GetSubType(this) == SHVControlContainer::SubTypeModalDialog)
		{
		SHVListPos pos = manager->TopLevelModalDialogs.Find(this);

			ModalMode = ModalNone;

			if (pos)
				manager->TopLevelModalDialogs.RemoveAt(pos);

			if (!manager->TopLevelModalDialogs.GetCount())
			{
			SHVListIterator<SHVControlContainer*> itr(manager->TopLevelDialogs);

				manager->MainWindow->SetModalMode(false);

				while (itr.MoveNext())
					itr.Get()->SetModalMode(false);
			}
		}
	}

	return retVal;
}

/*************************************
 * GetData
 *************************************/
SHVBool SHVControlContainer::GetData(SHVControlData* data)
{
SHVControlDataRowRef dataRow = data->GetRow();

	if (!dataRow.IsNull())
	{
		return dataRow->SetValue(GetTitle());
	}

	return SHVBool::False;
}

/*************************************
 * SetData
 *************************************/
SHVBool SHVControlContainer::SetData(SHVControlData* data)
{
SHVControlDataRowRef dataRow = data->GetRow();

	if (!dataRow.IsNull())
	{
		SetTitle(dataRow->GetValue());
		return SHVBool::True;
	}

	return SHVBool::False;
}

/*************************************
 * SetLayoutEngine
 *************************************/
void SHVControlContainer::SetLayoutEngine(SHVControlLayout* engine)
{
	LayoutEngine = engine;

	GetImplementor()->SetResizable(engine ? true : false);

	ResizeControls();
}

/*************************************
 * GetLayoutEngine
 *************************************/
SHVControlLayout* SHVControlContainer::GetLayoutEngine()
{
	return LayoutEngine;
}

/*************************************
 * Clear
 *************************************/
void SHVControlContainer::Clear()
{
	for (size_t controls = CalculateControlCount(); controls;)
	{
		// Setting parent to NULL will destroy the internal control and remove it from us
		GetControl(--controls)->SetParent(NULL);
	}
}

/*************************************
 * ContainsControl
 *************************************/
SHVBool SHVControlContainer::ContainsControl(SHVControl* cntrl)
{
SHVBool retVal(SHVBool::False);

	for (size_t i=CalculateControlCount();i && !retVal;)
	{
		if (GetControl(--i) == cntrl)
			retVal = SHVBool::True;
	}

	return retVal;
}

/*************************************
 * PreDestroy
 *************************************/
SHVBool SHVControlContainer::PreDestroy()
{
	if (!PreDestroySubscriber.IsNull())
	{
		SHVEventDataBool allowPredestroy(SHVBool::True);
		PreDestroySubscriber->EmitNow(GetModuleList(),new SHVEventData<SHVEventDataBool>(allowPredestroy, NULL, EventPreDestroy));
		return allowPredestroy.GetValue();
	}
	else
		return SHVBool(SHVBool::True);
}

/*************************************
 * SubscribePreDestroy
 *************************************/
void SHVControlContainer::SubscribePreDestroy(SHVEventSubscriberBase* subs)
{
	PreDestroySubscriber = subs;
}

/*************************************
 * AddControl
 *************************************/
SHVBool SHVControlContainer::AddControl(SHVControl* cntrl)
{
	if (ContainsControl(cntrl))
		return SHVBool::True;

	Controls.Add(cntrl->CreateRef(),ControlGrowSize);
	return SHVBool::True;
}

/*************************************
 * RemoveControl
 *************************************/
SHVBool SHVControlContainer::RemoveControl(SHVControl* cntrl)
{
size_t count = Controls.CalculateCount();
size_t i;
SHVBool retVal(SHVBool::False);

	if (cntrl == NULL)
		return retVal;

	for (i=count;i>0 && !retVal;)
	{
		if (Controls[--i] == cntrl)
			retVal = SHVBool::True;
	}

	// if we found it, remove it from the vector
	if (retVal)
	{
		Controls.Replace(i,NULL);
		for (i++;i<count;i++)
		{
			Controls.Replace(i-1, Controls.Replace(i,NULL));
		}

		Controls.Truncate(ControlGrowSize);

		cntrl->DestroyRef();
	}

	return retVal;
}

/*************************************
 * SetModalMode
 *************************************/
///\cond INTERNAL
void SHVControlContainer::SetModalMode(bool enable)
{
	if (enable && ModalMode == ModalNone)
	{
	ModalModes newModalMode = ( GetFlag(SHVControl::FlagDisabled) ? ModalGlobalSelfDisabled : ModalGlobalDisabled );

		// only set disabled if enabled
		if (newModalMode == ModalGlobalDisabled)
		{
			SetFlag(SHVControl::FlagDisabled);
		}

		ModalMode = newModalMode;
	}
	else if (!enable && ModalMode != ModalNone)
	{
	ModalModes oldModalMode = ModalMode;

		ModalMode = ModalNone;

		// only remove disabled if enabled
		if (oldModalMode == ModalGlobalDisabled)
		{
			SetFlag(SHVControl::FlagDisabled,false);
		}
	}
}
///\endcond


//=========================================================================================================
// SHVControlImplementerContainer
//=========================================================================================================

/*************************************
 * UpdateSetFlag
 *************************************/
void SHVControlImplementerContainer::UpdateSetFlag(SHVControl* owner, int& flag, bool enable)
{
	// update the internal modal disable stuff
	if ( ((SHVControlContainer*)owner)->ModalMode )
	{
		if ( (flag&SHVControl::FlagDisabled) )
		{
			((SHVControlContainer*)owner)->ModalMode = ( enable ? SHVControlContainer::ModalGlobalSelfDisabled
																: SHVControlContainer::ModalGlobalDisabled );

			flag -= SHVControl::FlagDisabled;
		}
	}
}

/*************************************
 * GetDisabledFlag
 *************************************/
bool SHVControlImplementerContainer::GetDisabledFlag(SHVControl* owner, int flag)
{
	// returns false if the container is globally disabled and locally enabled and the check is for disabled
	if  ( (flag&SHVControl::FlagDisabled) &&
			   ((SHVControlContainer*)owner)->ModalMode == SHVControlContainer::ModalGlobalDisabled )
	{
		return false;
	}

	return true;
}
