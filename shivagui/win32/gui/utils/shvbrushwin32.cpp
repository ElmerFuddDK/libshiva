#include "stdafx.h"
#include "../../../../include/platformspc.h"

#include "shvbrushwin32.h"


//=========================================================================================================
// SHVBrushWin32
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVBrushWin32::SHVBrushWin32(SHVColorWin32* color, int style)
{
	switch (style)
	{
	case StyleDefault:
	default:
		hBrush = ::CreateSolidBrush(color->GetColor());
	}

	Color = color;
}

/*************************************
 * Destructor
 *************************************/
SHVBrushWin32::~SHVBrushWin32()
{
	if (hBrush)
		::DeleteObject(hBrush);
}

/*************************************
 * Constructor
 *************************************/
SHVColor* SHVBrushWin32::GetColor()
{
	return Color;
}
