#include "stdafx.h"
#include "../../include/platformspc.h"

#include "../include/shvcontrollabel.h"

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
SHVBool SHVControlLabel::SetData(SHVControlData* data)
{
SHVControlDataRowRef dataRow = data->GetRow();

	if (!dataRow.IsNull())
	{
		SetText(dataRow->GetValue());
	}

	return SHVBool::False;
}


