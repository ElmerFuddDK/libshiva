#include "stdafx.h"
#include "../../../../include/platformspc.h"

#include "shvcolorwin32.h"


//=========================================================================================================
// SHVColorWin32
//=========================================================================================================

/*************************************
 * GetRed
 *************************************/
SHVColor::ColorVal SHVColorWin32::GetRed() const
{
	return (SHVColor::ColorVal)GetRValue(Color);
}

/*************************************
 * GetGreen
 *************************************/
SHVColor::ColorVal SHVColorWin32::GetGreen() const
{
	return (SHVColor::ColorVal)GetGValue(Color);
}

/*************************************
 * GetBlue
 *************************************/
SHVColor::ColorVal SHVColorWin32::GetBlue() const
{
	return (SHVColor::ColorVal)GetBValue(Color);
}

/*************************************
 * FromSysColor
 *************************************/
SHVColorWin32* SHVColorWin32::FromSysColor(int color)
{
	return new SHVColorWin32(::GetSysColor(color));
}
