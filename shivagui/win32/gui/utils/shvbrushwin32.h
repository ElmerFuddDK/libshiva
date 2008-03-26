#ifndef __SHIVA_GUIUTILSWIN32_BRUSH_H
#define __SHIVA_GUIUTILSWIN32_BRUSH_H


#include "../../../../include/gui/utils/shvbrush.h"
#include "shvcolorwin32.h"


//-=========================================================================================================
/// SHVBrushWin32 - brush class
/**
 * Implements a brush for win32.
 */

class SHVBrushWin32 : public SHVBrush
{
public:


	// Constructors
	SHVBrushWin32(SHVColorWin32* color, int style);
	virtual ~SHVBrushWin32();

	virtual SHVColor* GetColor();

	inline HBRUSH GetBrush();

private:
	///\cond INTERNAL
	SHVColorWin32Ref Color;
	HBRUSH hBrush;
	///\endcond
};
typedef SHVRefObjectContainer<SHVBrushWin32> SHVBrushWin32Ref;



// ============================================ implementation ============================================ //

/*************************************
 * GetPen
 *************************************/
HBRUSH SHVBrushWin32::GetBrush()
{
	return hBrush;
}

#endif
