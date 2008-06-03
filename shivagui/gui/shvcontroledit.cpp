#include "stdafx.h"
#include "../../include/platformspc.h"

#include "../../include/gui/shvcontroledit.h"

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
SHVBool SHVControlEdit::GetData(SHVDataBinder* data)
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
SHVBool SHVControlEdit::SetData(SHVDataBinder* data)
{
	if (data)
	{
		SetText(data->GetValue());
		return SHVBool::True;
	}

	return SHVBool::False;
}
