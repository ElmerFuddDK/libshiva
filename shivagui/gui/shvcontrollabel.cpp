#include "stdafx.h"
#include "../../include/platformspc.h"

#include "../../include/gui/shvcontrollabel.h"

/*************************************
 * GetType
 *************************************/
int SHVControlLabel::GetType()
{
	return SHVControl::TypeLabel;
}


/*************************************
 * GetData
 *************************************/
SHVBool SHVControlLabel::GetData(SHVControlData* data)
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
SHVBool SHVControlLabel::SetData(SHVControlData* data)
{
	if (data)
	{
		SetText(data->GetValue());
		return SHVBool::True;
	}

	return SHVBool::False;
}


