#ifndef __SHIVA_GUIUTILSGTK_PEN_H
#define __SHIVA_GUIUTILSGTK_PEN_H


#include "../../../../include/gui/utils/shvpen.h"
#include "shvcolorgtk.h"


//-=========================================================================================================
/// SHVPenGtk - pen class
/**
 * Implements a pen for gtk.
 */

class SHVPenGtk : public SHVPen
{
public:


	// Constructors
	SHVPenGtk(SHVColorGtk* color, int pen, int width);
	virtual ~SHVPenGtk();

	virtual SHVColor* GetColor();

	inline GdkLineStyle GetPen();
	inline int GetWidth();
	
	static void Apply(SHVPen* pen, GdkGC* GC, GdkGCValues& values);
	static void Reset(SHVPen* pen, GdkGC* GC, GdkGCValues& values);

private:
	///\cond INTERNAL
	SHVColorGtkRef Color;
	GdkLineStyle Pen;
	int Width;
	///\endcond
};
typedef SHVRefObjectContainer<SHVPenGtk> SHVPenGtkRef;



// ============================================ implementation ============================================ //

/*************************************
 * GetPen
 *************************************/
GdkLineStyle SHVPenGtk::GetPen()
{
	return Pen;
}

/*************************************
 * GetPen
 *************************************/
int SHVPenGtk::GetWidth()
{
	return Width;
}

#endif
