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


protected:
	inline SHVFont() : SHVRefObject() {}
};
typedef SHVRefObjectContainer<SHVFont> SHVFontRef;

#endif
