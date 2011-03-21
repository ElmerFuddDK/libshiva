#ifndef __SHIVA_GUIUTILSWIN32_COLOR_H
#define __SHIVA_GUIUTILSWIN32_COLOR_H


#include "../../../../include/gui/utils/shvcolor.h"


//-=========================================================================================================
/// SHVColorWin32 - color class
/**
 * Implements a color for win32.
 */

class SHVColorWin32 : public SHVColor
{
public:


	// Constructors
	inline SHVColorWin32(ColorVal r, ColorVal g, ColorVal b);
	inline SHVColorWin32(COLORREF color);

	virtual ColorVal GetRed() const;
	virtual ColorVal GetGreen() const;
	virtual ColorVal GetBlue() const;

	inline COLORREF GetColor() const;

	static SHVColorWin32* FromSysColor(int color); ///< eg. COLOR_3DFACE

private:
	///\cond INTERNAL
	COLORREF Color;
	///\endcond
};
typedef SHVRefObjectContainer<SHVColorWin32> SHVColorWin32Ref;



// ============================================ implementation ============================================ //

/*************************************
 * constructor
 *************************************/
SHVColorWin32::SHVColorWin32(ColorVal r, ColorVal g, ColorVal b)
{
	Color = RGB(r,g,b);
}
SHVColorWin32::SHVColorWin32(COLORREF color)
{
	Color = color;
}

/*************************************
 * GetColor
 *************************************/
COLORREF SHVColorWin32::GetColor() const
{
	return Color;
}

#endif
