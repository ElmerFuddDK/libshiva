#ifndef __SHIVA_GUIUTILSWIN32_PEN_H
#define __SHIVA_GUIUTILSWIN32_PEN_H


#include "../../../../include/gui/utils/shvpen.h"
#include "shvcolorwin32.h"


//-=========================================================================================================
/// SHVPenWin32 - pen class
/**
 * Implements a pen for win32.
 */

class SHVPenWin32 : public SHVPen
{
public:


	// Constructors
	SHVPenWin32(SHVColorWin32* color, int style, int width);
	virtual ~SHVPenWin32();

	virtual SHVColor* GetColor();

	inline HPEN GetPen();

private:
	///\cond INTERNAL
	SHVColorWin32Ref Color;
	HPEN hPen;
	///\endcond
};
typedef SHVRefObjectContainer<SHVPenWin32> SHVPenWin32Ref;



// ============================================ implementation ============================================ //

/*************************************
 * GetPen
 *************************************/
HPEN SHVPenWin32::GetPen()
{
	return hPen;
}

#endif
