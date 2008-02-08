#include "stdafx.h"
#include "../../include/platformspc.h"

#include "../include/shvcontrolcontainer.h"
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
	SHVASSERT(GetImplementor()->GetSubType(this) == SHVControlContainer::SubTypeMainWindow || 
		GetImplementor()->GetSubType(this) == SHVControlContainer::SubTypeDialog);

	return GetImplementor()->Create(this, NULL, SHVControl::FlagVisible);
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
 * ContainsControl
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
 * ContainsControl
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
