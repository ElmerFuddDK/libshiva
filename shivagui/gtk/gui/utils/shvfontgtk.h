#ifndef __SHIVA_GUIUTILSGTK_FONT_H
#define __SHIVA_GUIUTILSGTK_FONT_H


#include "../../../../include/gui/utils/shvfont.h"

#include <gtk/gtk.h>


//-=========================================================================================================
/// SHVFontGtk - Font class
/**
 * Implements a font for GTK
 */

class SHVFontGtk : public SHVFont
{
public:


	// Constructors
	SHVFontGtk(PangoFontDescription* font, PangoContext* context, bool owner);
	virtual ~SHVFontGtk();


	virtual SHVFont* CreateCopy(int sizePercentage = 100, int styles = StyleUnchanged);

	virtual int GetHeight();
	virtual int GetCellHeight();
	virtual int CalculateTextWidth(const SHVStringC text);
	virtual int GetApproximateWidth();

	virtual SHVStringBuffer GetName();

	inline PangoFontDescription* GetFont();
	static SHVFontGtk* CopyFrom(PangoFontDescription* font, PangoContext* context);
	static SHVFontGtk* CreateFromName(GtkWidget* mainWnd, const SHVStringC name, int height, int styles);


private:
	///\cond INTERNAL
	PangoFontDescription* Font;
	GdkFont* DrawFont;
	PangoContext* Context;
	int Height;
	int CellHeight;
	int ApproximateWidth;
	
	bool InitDrawFont();
	///\endcond
};
typedef SHVRefObjectContainer<SHVFontGtk> SHVFontGtkRef;



// ============================================ implementation ============================================ //

/*************************************
 * GetFont
 *************************************/
PangoFontDescription* SHVFontGtk::GetFont()
{
	return Font;
}

#endif
