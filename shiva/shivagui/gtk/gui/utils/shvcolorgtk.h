#ifndef __SHIVA_GUIUTILSGTK_COLOR_H
#define __SHIVA_GUIUTILSGTK_COLOR_H


#include "../../../../include/gui/utils/shvcolor.h"

#include <gtk/gtk.h>


//-=========================================================================================================
/// SHVColorGtk - color class
/**
 * Implements a color for gtk.
 */

class SHVColorGtk : public SHVColor
{
public:


	// Constructors
	inline SHVColorGtk(ColorVal r, ColorVal g, ColorVal b);
	inline SHVColorGtk(const GdkColor& color);

	virtual ColorVal GetRed() const;
	virtual ColorVal GetGreen() const;
	virtual ColorVal GetBlue() const;

	inline GdkColor* GetColor() const;
	inline static GdkColor* GetNative(SHVColor* color);

private:
	///\cond INTERNAL
	GdkColor Color;
	///\endcond
};
typedef SHVRefObjectContainer<SHVColorGtk> SHVColorGtkRef;



// ============================================ implementation ============================================ //

/*************************************
 * constructor
 *************************************/
SHVColorGtk::SHVColorGtk(ColorVal r, ColorVal g, ColorVal b)
{
	Color.red = r*0x100+r;
	Color.green = g*0x100+g;
	Color.blue = b*0x100+b;
	Color.pixel = b+(g*0x0100)+(r*0x010000);
}
SHVColorGtk::SHVColorGtk(const GdkColor& color) : Color(color)
{
}

/*************************************
 * GetColor
 *************************************/
GdkColor* SHVColorGtk::GetColor() const
{
	return &((SHVColorGtk*)this)->Color;
}

GdkColor* SHVColorGtk::GetNative(SHVColor* color)
{
	return &((SHVColorGtk*)color)->Color;
}

#endif
