#ifndef __SHIVA_GUIUTILSGTK_DRAW_H
#define __SHIVA_GUIUTILSGTK_DRAW_H


#include "../../../../include/gui/utils/shvdraw.h"
#include "shvcolorgtk.h"
#include "shvfontgtk.h"
#include "../../../../include/gui/utils/shvbrush.h"
//#include "shvbrushgtk.h"
//#include "shvpengtk.h"


//-=========================================================================================================
/// SHVDrawGtk - drawing class
/**
 * Implements draw for gtk.
 */

class SHVDrawGtk : public SHVDraw
{
public:


	// Constructors
	SHVDrawGtk(SHVGUIManager* manager, GdkGC* gc, GdkWindow* window, SHVControl* control);
	virtual ~SHVDrawGtk();


	// Obtain the internal rectangle of a window
	virtual SHVRect GetClientRect(SHVControl* control);
	inline void CloseDC();


	// Get internal GC
	inline GdkGC* GetGC();


	// Drawing routines:
	// -----------------

	// DrawLine
	virtual void DrawLine(SHVPoint from, SHVPoint to, SHVColor* color);
	virtual void DrawLine(SHVPoint from, SHVPoint to, SHVPen* pen);

	virtual void DrawXORLine(SHVPoint from, SHVPoint to);

	// DrawPixel
	virtual void DrawPixel(SHVPoint where, SHVColor* color);

	// DrawRect
	virtual void DrawRect(SHVRect rect, SHVColor* color);
	virtual void DrawRectFilled(SHVRect rect, SHVColor* color);
	virtual void DrawEdgeAndShrink(SHVRect& rect, EdgeTypes type = EdgeTypeDefault, int flags = EdgeFlagDefault);

	virtual void DrawXORRect(SHVRect rect);

	// DrawPoly
	virtual void DrawPolyline(SHVColor* color, int points, ...);
	virtual void DrawPolygon(SHVColor* color, int points, ...);

	// Draw bitmaps
	virtual void DrawBitmap(SHVBitmap* bitmap, SHVPoint position, int width, int height, SHVColor* transparentColor = NULL);
	virtual void DrawBitmap(SHVBitmap* bitmap, SHVPoint position, SHVColor* transparentColor = NULL);
	virtual void DrawBitmapCentered(SHVBitmap* bitmap, SHVRect rDest, SHVColor* transparentColor = NULL);
	virtual void DrawBitmapCentered(SHVBitmap* bitmap, SHVRect rDest, int width, int height, SHVColor* transparentColor = NULL);

	// Draw text
	virtual void DrawText(const SHVStringC txt, SHVRect rect, SHVColor* color = NULL, int options = TextDefault);
	virtual void DrawText(SHVFont* font, const SHVStringC txt, SHVRect rect, SHVColor* color = NULL, int options = TextDefault);


	// Static mapping functions
	inline GdkRectangle MapRect(const SHVRect& rect);
	inline SHVRect MapRect(const GdkRectangle& rect);
	inline static GdkColor* GetColor(SHVColor* color);
	inline static GdkFill GetBrush(SHVBrush* brush);

protected:
	///\cond INTERNAL
	//void TransparentBlit(HDC bmDC, SHVPoint position, int width, int height, COLORREF colorTransparent, bool stretchmode, int targetWidth, int targetHeight);
	void SetClipRect(SHVRect* rect = NULL);

	GdkGC* GC;
	SHVControl* Control;
	GtkWidget* Widget;
	GdkWindow* Window;
	SHVRect WindowArea;
	///\endcond
};
typedef SHVRefObjectContainer<SHVDrawGtk> SHVDrawGtkRef;



// ============================================ implementation ============================================ //

/*************************************
 * CloseDC
 *************************************/
void SHVDrawGtk::CloseDC()
{
	if (GC)
	{
		g_object_unref(GC);
		GC = NULL;
	}
}

/*************************************
 * GetGC
 *************************************/
GdkGC* SHVDrawGtk::GetGC()
{
	return GC;
}

/*************************************
 * MapRect
 *************************************/
GdkRectangle SHVDrawGtk::MapRect(const SHVRect& rect)
{
GdkRectangle rct;
	rct.x = rect.GetX();
	rct.y = rect.GetY();
	rct.width = rect.GetWidth();
	rct.height = rect.GetHeight();

	return rct;
}

/*************************************
 * MapRect
 *************************************/
SHVRect SHVDrawGtk::MapRect(const GdkRectangle& rect)
{
SHVRect rct;
	rct.SetX(rect.x);
	rct.SetY(rect.y);
	rct.SetWidth(rect.width);
	rct.SetHeight(rect.height);

	return rct;
}

/*************************************
 * GetColor
 *************************************/
GdkColor* SHVDrawGtk::GetColor(SHVColor* color)
{
	return ((SHVColorGtk*)color)->GetColor();
}

/*************************************
 * GetBrush
 *************************************/
GdkFill SHVDrawGtk::GetBrush(SHVBrush* brush)
{
//	return ((SHVBrushGtk*)brush)->GetBrush();
	return GDK_SOLID;
}

#endif
