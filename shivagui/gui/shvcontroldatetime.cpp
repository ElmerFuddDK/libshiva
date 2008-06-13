#include "stdafx.h"
#include "../../include/platformspc.h"

#include "../../include/gui/shvcontroldatetime.h"

//=========================================================================================================
// SHVControlDateTime
//=========================================================================================================

/*************************************
 * Destructor
 *************************************/
SHVControlDateTime::~SHVControlDateTime()
{
}

/*************************************
 * GetType
 *************************************/
int SHVControlDateTime::GetType()
{
	return SHVControl::TypeDateTime;
}

/*************************************
 * GetData
 *************************************/
SHVBool SHVControlDateTime::GetData(SHVDataBinder* data)
{
	if (data)
	{
		return data->SetValue(GetTime().ToDateString());
	}

	return SHVBool::False;
}

/*************************************
 * SetData
 *************************************/
SHVBool SHVControlDateTime::SetData(SHVDataBinder* data)
{
	if (data)
	{
	SHVTime t;
		t.SetFromDateString(data->GetValue());
		SetTime(t);
		return SHVBool::True;
	}

	return SHVBool::False;
}
