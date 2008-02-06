#include "stdafx.h"
#include "../../include/platformspc.h"

#include "../include/shvcontrolcontainer.h"


//=========================================================================================================
// SHVControlContainer - base control container class
//=========================================================================================================

/*************************************
 * Destructor
 *************************************/
SHVControlContainer::~SHVControlContainer()
{
	for (size_t i=CalculateControlCount();i;)
	{
		((SHVControl*)Controls[--i])->DestroyRef();
	}

	Controls.Clear();
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

	return GetImplementor()->Create(this, NULL);
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
