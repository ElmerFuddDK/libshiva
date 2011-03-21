#ifndef __SHIVA_GUIUTILSWIN32_FONT_H
#define __SHIVA_GUIUTILSWIN32_FONT_H


#include "../../../../include/gui/utils/shvfont.h"


//-=========================================================================================================
/// SHVFont - Font class
/**
 * Implements a font.
 */

class SHVFontWin32 : public SHVFont
{
public:


	// ConstructorS
	SHVFontWin32(HFONT font, bool owner = true);
	SHVFontWin32(const SHVStringC fontName, int height, int styles = StyleNormal);
	virtual ~SHVFontWin32();


	virtual SHVFont* CreateCopy(int sizePercentage = 100, int styles = StyleUnchanged);

	virtual int GetHeight();
	virtual int GetCellHeight();
	virtual int CalculateTextWidth(const SHVStringC text);
	virtual int GetApproximateWidth();

	virtual SHVStringBuffer GetName();

	inline HFONT GetFont();


	static SHVFontWin32* SHVFontWin32::CreateSystemFont();
	static int MapStyles(LOGFONT& lf);
	static int CalculateCellHeight(HFONT font);

private:
	///\cond INTERNAL
	HFONT Font;
	int CellHeight;
	int ApproximateWidth;
	bool Owner;
	///\endcond
};
typedef SHVRefObjectContainer<SHVFontWin32> SHVFontWin32Ref;



// ============================================ implementation ============================================ //

/*************************************
 * GetFont
 *************************************/
HFONT SHVFontWin32::GetFont()
{
	return Font;
}

#endif
