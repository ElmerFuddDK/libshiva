#ifndef __SHIVA_GUIUTILSWIN32_DRAW_H
#define __SHIVA_GUIUTILSWIN32_DRAW_H


#include "../../../../include/gui/utils/shvdraw.h"
#include "shvcolorwin32.h"
#include "shvfontwin32.h"
#include "shvbrushwin32.h"
#include "shvpenwin32.h"


//-=========================================================================================================
/// SHVDrawWin32 - drawing class
/**
 * Implements draw for win32.
 */

class SHVDrawWin32 : public SHVDraw
{
public:


	// Constructors
	inline SHVDrawWin32(SHVGUIManager* manager, HDC dc);


	// Obtain the internal rectangle of a window
	virtual SHVRect GetClientRect(SHVControl* control);
	inline void CloseDC();


	// Get internal DC handle
	inline HDC GetHDC();


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

	virtual void DrawXORRect(SHVRect rect);

	// DrawPoly
	virtual void DrawPolyline(SHVColor* color, int points, ...);
	virtual void DrawPolygon(SHVColor* color, int points, ...);

	// Draw bitmaps
	virtual void DrawBitmap(SHVBitmap* bitmap, SHVPoint position, int width, int height, SHVColor* transparentColor = NULL);
	virtual void DrawBitmap(SHVBitmap* bitmap, SHVPoint position, SHVColor* transparentColor = NULL);
	virtual void DrawBitmapCentered(SHVBitmap* bitmap, SHVRect rDest, SHVColor* transparentColor = NULL);

	// Draw text
	virtual void DrawText(const SHVStringC txt, SHVRect rect, SHVColor* color = NULL, int options = TextDefault);
	virtual void DrawText(SHVFont* font, const SHVStringC txt, SHVRect rect, SHVColor* color = NULL, int options = TextDefault);


	// Static mapping functions
	inline static RECT MapRect(const SHVRect& rect);
	inline static SHVRect MapRect(const RECT& rect);
	inline static COLORREF GetColor(SHVColor* color);
	inline static HPEN GetPen(SHVPen* pen);
	inline static HBRUSH GetBrush(SHVBrush* brush);
	inline static HFONT GetFont(SHVFont* font);

protected:
	///\cond INTERNAL
	HDC hDC;
	HWND hWnd;
	///\endcond
};
typedef SHVRefObjectContainer<SHVDrawWin32> SHVDrawWin32Ref;



//-=========================================================================================================
/// SHVDrawPaintWin32 - drawing class for WM_PAINT
/**
 * Implements draw for win32.
 */
class SHVDrawPaintWin32 : public SHVDrawWin32
{
public:


	SHVDrawPaintWin32(SHVGUIManager* manager, HWND wnd);
	virtual ~SHVDrawPaintWin32();

protected:
	///\cond INTERNAL
	PAINTSTRUCT ps;
	///\endcond
};



// ============================================ implementation ============================================ //

/*************************************
 * constructor
 *************************************/
SHVDrawWin32::SHVDrawWin32(SHVGUIManager* manager, HDC dc) : SHVDraw(manager)
{
	hDC = dc;
	hWnd = NULL;
}

/*************************************
 * CloseDC
 *************************************/
void SHVDrawWin32::CloseDC()
{
	if (hDC)
	{
		::ReleaseDC(hWnd,hDC);
		hDC = NULL;
	}
}

/*************************************
 * GetHDC
 *************************************/
HDC SHVDrawWin32::GetHDC()
{
	return hDC;
}

/*************************************
 * MapRect
 *************************************/
RECT SHVDrawWin32::MapRect(const SHVRect& rect)
{
RECT rct;
	rct.left = rect.GetLeft();
	rct.top = rect.GetTop();
	rct.right = rect.GetRight();
	rct.bottom = rect.GetBottom();

	return rct;
}

/*************************************
 * MapRect
 *************************************/
SHVRect SHVDrawWin32::MapRect(const RECT& rect)
{
SHVRect rct;
	rct.SetLeft(rect.left);
	rct.SetTop(rect.top);
	rct.SetRight(rect.right);
	rct.SetBottom(rect.bottom);

	return rct;
}

/*************************************
 * GetColor
 *************************************/
COLORREF SHVDrawWin32::GetColor(SHVColor* color)
{
	return ((SHVColorWin32*)color)->GetColor();
}
/*************************************
 * GetPen
 *************************************/
HPEN SHVDrawWin32::GetPen(SHVPen* pen)
{
	return ((SHVPenWin32*)pen)->GetPen();
}

/*************************************
 * GetBrush
 *************************************/
HBRUSH SHVDrawWin32::GetBrush(SHVBrush* brush)
{
	return ((SHVBrushWin32*)brush)->GetBrush();
}

/*************************************
 * GetFont
 *************************************/
HFONT SHVDrawWin32::GetFont(SHVFont* font)
{
	return ((SHVFontWin32*)font)->GetFont();
}

#endif
