#ifndef __SHIVA_GUIUTILS_FONT_H
#define __SHIVA_GUIUTILS_FONT_H


#include "../../../include/utils/shvrefobject.h"
#include "../../../include/utils/shvstring.h"


//-=========================================================================================================
/// SHVFont - Font class
/**
 * Implements a font.
 */

class SHVFont : public SHVRefObject
{
public:


	// Properties
	enum Styles {
		StyleUnchanged = -1,
		StyleNormal = 0,
		StyleBold = 1,
		StyleItalic = 2,
		StyleUnderline = 4
	};


	virtual SHVFont* CreateCopy(int sizePercentage = 100, int styles = StyleUnchanged) = 0;

	virtual int GetHeight() = 0;
	virtual int GetCellHeight() = 0;
	virtual int CalculateTextWidth(const SHVStringC text) = 0;
	virtual int GetApproximateWidth() = 0;

	virtual SHVStringBuffer GetName() = 0;


	// Conversion between font units and pixels
	inline int LFUToWidth(int width);
	inline int LFUToHeight(int height);
	inline int WidthToLFU(int width);
	inline int HeightToLFU(int height);


protected:
	inline SHVFont() : SHVRefObject() {}
};
typedef SHVRefObjectContainer<SHVFont> SHVFontRef;



// ============================================= implementation ============================================= //


/*************************************
 * LFUToWidth
 *************************************/
int SHVFont::LFUToWidth(int width)
{
	return ((width*10+5)*GetApproximateWidth())/100;
}

int SHVFont::LFUToHeight(int height)
{
	return ((height*10+5)*GetCellHeight())/100;
}

int SHVFont::WidthToLFU(int width)
{
	return (((100*width)+5)/GetApproximateWidth())/10;
}

int SHVFont::HeightToLFU(int height)
{
	return (((100*height)+5)/GetCellHeight())/10;
}

#endif
