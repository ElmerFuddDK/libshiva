#include "stdafx.h"
#include "../../include/platformspc.h"

#include "../../include/gui/shvcontrolbutton.h"

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
	if (data)
	{
		return data->SetValue(GetText());
	}

	return SHVBool::False;
}

/*************************************
 * SetData
 *************************************/
SHVBool SHVControlButton::SetData(SHVControlData* data)
{
	if (data)
	{
		SetText(data->GetValue());
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
		Clicked->EmitNow(GetModuleList(),new SHVEvent(NULL,EventClicked,SHVInt(),this));
	}
}

/*************************************
 * SubscribeClicked
 *************************************/
void SHVControlButton::SubscribeClicked(SHVEventSubscriberBase* subscriber)
{
	Clicked = subscriber;
}
