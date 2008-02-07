#include "stdafx.h"
#include "../../include/platformspc.h"

#include "../include/shvcontroledit.h"

//=========================================================================================================
// SHVControlEdit - base edit control class
//=========================================================================================================

/*************************************
 * Destructor
 *************************************/
SHVControlEdit::~SHVControlEdit()
{
}

/*************************************
 * GetType
 *************************************/
int SHVControlEdit::GetType()
{
	return SHVControl::TypeEdit;
}

/*************************************
 * GetData
 *************************************/
SHVBool SHVControlEdit::GetData(SHVControlData* data)
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
SHVBool SHVControlEdit::SetData(SHVControlData* data)
{
SHVControlDataRowRef dataRow = data->GetRow();

	if (!dataRow.IsNull())
	{
		SetText(dataRow->GetValue());
		return SHVBool::True;
	}

	return SHVBool::False;
}
