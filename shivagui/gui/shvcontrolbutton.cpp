#include "stdafx.h"
#include "../../include/platformspc.h"

#include "../include/shvcontrolbutton.h"

//=========================================================================================================
// SHVControlButton - base button control class
//=========================================================================================================

/*************************************
 * Destructor
 *************************************/
SHVControlButton::~SHVControlButton()
{
}

/*************************************
 * GetType
 *************************************/
int SHVControlButton::GetType()
{
	return SHVControl::TypeButton;
}

/*************************************
 * GetData
 *************************************/
SHVBool SHVControlButton::GetData(SHVControlData* data)
{
SHVControlDataRowRef dataRow = data->GetRow();

	if (!dataRow.IsNull())
	{
		return dataRow->SetValue(GetText());
	}

	return SHVBool::False;
}

/*************************************
 * SetData
 *************************************/
SHVBool SHVControlButton::SetData(SHVControlData* data)
{
SHVControlDataRowRef dataRow = data->GetRow();

	if (!dataRow.IsNull())
	{
		SetText(dataRow->GetValue());
		return SHVBool::True;
	}

	return SHVBool::False;
}

/*************************************
 * PerformClicked
 *************************************/
void SHVControlButton::PerformClicked()
{
	if (!Clicked.IsNull())
	{
		Clicked->EmitNow(GetModuleList(),new SHVEvent(NULL,EventClicked,NULL,this));
	}
}

/*************************************
 * SubscribeClicked
 *************************************/
void SHVControlButton::SubscribeClicked(SHVEventSubscriberBase* subscriber)
{
	Clicked = subscriber;
}
