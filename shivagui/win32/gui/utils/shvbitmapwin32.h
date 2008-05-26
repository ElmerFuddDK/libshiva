#ifndef __SHIVA_GUIUTILSWIN32_BITMAP_H
#define __SHIVA_GUIUTILSWIN32_BITMAP_H


#include "../../../../include/gui/utils/shvbitmap.h"

// forward declare
class SHVDrawWin32;


//-=========================================================================================================
/// SHVBitmapWin32
/**
 */

class SHVBitmapWin32 : public SHVBitmap
{
public:


	// Constructor
	SHVBitmapWin32(HINSTANCE hInstance, int resID);
	virtual ~SHVBitmapWin32();


	// from SHVBitmap
	virtual SHVBool IsCreated();

	virtual SHVInt GetWidth();
	virtual SHVInt GetHeight();


private:
friend class SHVDrawWin32;
	///\cond INTERNAL
	HBITMAP hBitmap;
	SHVInt Width, Height;
	///\endcnd
};
typedef SHVRefObjectContainer<SHVBitmapWin32> SHVBitmapWin32Ref;

#endif
