#ifndef __SHIVA_GUIUTILS_BITMAP_H
#define __SHIVA_GUIUTILS_BITMAP_H

#include "../../../include/utils/shvrefobject.h"


//-=========================================================================================================
/// SHVBitmap - interface for bitmap
/**
 */

class SHVBitmap : public SHVRefObject
{
public:


	virtual SHVBool IsCreated() = 0;

	virtual SHVInt GetWidth() = 0;
	virtual SHVInt GetHeight() = 0;


protected:
	inline SHVBitmap() {}
};
typedef SHVRefObjectContainer<SHVBitmap> SHVBitmapRef;


#endif
