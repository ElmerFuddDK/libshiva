#include "stdafx.h"
#include "../../../../include/platformspc.h"

#include "shvpenwin32.h"


//=========================================================================================================
// SHVColorWin32
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVPenWin32::SHVPenWin32(SHVColorWin32* color, int style, int width)
{
int penStyle;

	switch (style)
	{
	case StyleDefault:
	default:
		penStyle = PS_COSMETIC;
	}

	hPen = ::CreatePen(penStyle,width,color->GetColor());

	Color = color;
}

/*************************************
 * Destructor
 *************************************/
SHVPenWin32::~SHVPenWin32()
{
	if (hPen)
		::DeleteObject(hPen);
}

/*************************************
 * Constructor
 *************************************/
SHVColor* SHVPenWin32::GetColor()
{
	return Color;
}
