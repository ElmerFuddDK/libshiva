#ifndef __SHIVA_GUIUTILSGTK_BITMAP_H
#define __SHIVA_GUIUTILSGTK_BITMAP_H


#include "../../../../include/gui/utils/shvbitmap.h"
#include "shvcolorgtk.h"

#include <gtk/gtk.h>

// forward declare
class SHVDrawGtk;
class SHVGUIManagerGtk;


//-=========================================================================================================
/// SHVBitmapGtk
/**
 */

class SHVBitmapGtk : public SHVBitmap
{
public:


	// Constructor
	SHVBitmapGtk(SHVGUIManagerGtk* guiManager, char ** xpmResource);
	virtual ~SHVBitmapGtk();


	// from SHVBitmap
	virtual SHVBool IsCreated();

	virtual SHVInt GetWidth();
	virtual SHVInt GetHeight();

	bool RealiseTransparentBitmap(SHVColor* color);

private:
friend class SHVDrawGtk;
	///\cond INTERNAL
	char** XpmResource;
	GtkWidget* MainWnd;
	GdkDrawable* Bitmap;
	GdkBitmap* TransparencyMask;
	SHVColorGtkRef TransparentColor;
	SHVInt Width, Height;
	///\endcnd
};
typedef SHVRefObjectContainer<SHVBitmapGtk> SHVBitmapGtkRef;

#endif
