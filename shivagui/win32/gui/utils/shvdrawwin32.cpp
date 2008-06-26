/*
 *   Copyright (C) 2008 by Lars Eriksen
 *
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 of the
 *   License, or (at your option) any later version with the following
 *   exeptions:
 *
 *   1) Static linking to the library does not constitute derivative work
 *      and does not require the author to provide source code for the
 *      application.
 *      Compiling applications with the source code directly linked in is
 *      Considered static linking as well.
 *
 *   2) You do not have to provide a copy of the license with programs
 *      that are linked against this code.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "stdafx.h"
#include "../../../../include/platformspc.h"

#include "shvdrawwin32.h"
#include "shvbitmapwin32.h"
#include "../shvwin32.h"


#ifdef __SHIVA_WINCE
struct SHV_DrawWin32_LineDraw
{
	POINT p[2];
	inline SHV_DrawWin32_LineDraw() { p[0].x = p[0].y = p[1].x = p[1].y = 0; }
};
# define SHVLINEDRAW SHV_DrawWin32_LineDraw __drawlines
# define SHVMOVETOEX(dc,_x,_y,point) __drawlines.p[0].x = _x; __drawlines.p[0].y = _y
# define SHVLINETO(dc,_x,_y) __drawlines.p[1].x = _x; __drawlines.p[1].y = _y; ::Polyline(dc, __drawlines.p, 2); __drawlines.p[0] = __drawlines.p[1]
#else
# define SHVLINEDRAW
# define SHVMOVETOEX(dc,x,y,point) ::MoveToEx(dc,x,y,point)
# define SHVLINETO(dc,x,y) ::LineTo(dc,x,y)
#endif


//=========================================================================================================
// SHVDrawWin32 - drawing class
//=========================================================================================================

/*************************************
 * GetClientRect
 *************************************/
SHVRect SHVDrawWin32::GetClientRect(SHVControl* control)
{
RECT nativeRect;

	::GetClientRect(Win32::GetHandle(control),&nativeRect);

	return SHVRect(nativeRect.left,nativeRect.top,nativeRect.right,nativeRect.bottom);
	
}

/*************************************
 * DrawLine
 *************************************/
void SHVDrawWin32::DrawLine(SHVPoint from, SHVPoint to, SHVColor* color)
{
SHVPenRef pen = GUIManager->CreatePen(color);
	DrawLine(from,to,pen);
	if (color)
		color->ValidateRefCount();
}
void SHVDrawWin32::DrawLine(SHVPoint from, SHVPoint to, SHVPen* pen)
{
int dcBackup = ::SaveDC(hDC);
POINT point;
SHVLINEDRAW;

	SHVUNUSED_PARAM(point);

	::SelectObject(hDC, SHVDrawWin32::GetPen(pen));

	SHVMOVETOEX(hDC, from.x, from.y, &point);
	SHVLINETO(hDC, to.x, to.y);

	::RestoreDC(hDC,dcBackup);

	if (pen)
		pen->ValidateRefCount();
}

/*************************************
 * DrawXORLine
 *************************************/
void SHVDrawWin32::DrawXORLine(SHVPoint from, SHVPoint to)
{
int dcBackup = ::SaveDC(hDC);
POINT point;
int drawMode = ::SetROP2(hDC,R2_NOT);
SHVLINEDRAW;

	SHVUNUSED_PARAM(point);

	SHVMOVETOEX(hDC, from.x, from.y, &point);
	SHVLINETO(hDC, to.x, to.y);

	::SetROP2(hDC,drawMode);
	::RestoreDC(hDC,dcBackup);
}

/*************************************
 * DrawPixel
 *************************************/
void SHVDrawWin32::DrawPixel(SHVPoint where, SHVColor* color)
{
	::SetPixel(hDC, where.x, where.y, SHVDrawWin32::GetColor(color));
	if (color)
		color->ValidateRefCount();
}

/*************************************
 * DrawRect
 *************************************/
void SHVDrawWin32::DrawRect(SHVRect rect, SHVColor* color)
{
SHVPenRef pen = GUIManager->CreatePen(color);
int dcBackup = ::SaveDC(hDC);
POINT point;
SHVLINEDRAW;

	SHVUNUSED_PARAM(point);

	::SelectObject(hDC, SHVDrawWin32::GetPen(pen));
	
	SHVMOVETOEX(hDC, rect.GetLeft(), rect.GetTop(), &point);
	SHVLINETO(hDC, rect.GetRight(), rect.GetTop());
	SHVLINETO(hDC, rect.GetRight(), rect.GetBottom());
	SHVLINETO(hDC, rect.GetLeft(), rect.GetBottom());
	SHVLINETO(hDC, rect.GetLeft(), rect.GetTop());

	::RestoreDC(hDC,dcBackup);
	if (color)
		color->ValidateRefCount();
}

/*************************************
 * DrawRectFilled
 *************************************/
void SHVDrawWin32::DrawRectFilled(SHVRect rect, SHVColor* color)
{
RECT rectNative(SHVDrawWin32::MapRect(rect));

	::SetBkColor(hDC, SHVDrawWin32::GetColor(color));
	::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rectNative, NULL, 0, NULL);
	if (color)
		color->ValidateRefCount();
}

/*************************************
 * DrawXORRect
 *************************************/
void SHVDrawWin32::DrawXORRect(SHVRect rect)
{
int dcBackup = ::SaveDC(hDC);
POINT point;
int drawMode = ::SetROP2(hDC,R2_NOT);
SHVLINEDRAW;

	SHVUNUSED_PARAM(point);
	
	SHVMOVETOEX(hDC, rect.GetLeft(), rect.GetTop(), &point);
	SHVLINETO(hDC, rect.GetRight(), rect.GetTop());
	SHVLINETO(hDC, rect.GetRight(), rect.GetBottom());
	SHVLINETO(hDC, rect.GetLeft(), rect.GetBottom());
	SHVLINETO(hDC, rect.GetLeft(), rect.GetTop());

	::SetROP2(hDC,drawMode);
	::RestoreDC(hDC,dcBackup);
}

/*************************************
 * DrawPolyline
 *************************************/
void SHVDrawWin32::DrawPolyline(SHVColor* color, int points, ...)
{
POINT* pts = new POINT[points];
SHVPenRef pen = GUIManager->CreatePen(color);
int dcBackup = ::SaveDC(hDC);

	// assume SHVPoint is of same size as POINT
	SHVASSERT(sizeof(POINT) == sizeof(SHVPoint));

	va_list argList;
	va_start( argList, points );
	for (int i=0; i<points; i++)
		pts[i] = va_arg(argList, POINT);
	va_end(argList);

	::SelectObject(hDC, SHVDrawWin32::GetPen(pen));

	::Polyline(hDC,pts,points);

	::RestoreDC(hDC,dcBackup);

	delete [] pts;
	if (color)
		color->ValidateRefCount();
}

/*************************************
 * DrawPolygon
 *************************************/
void SHVDrawWin32::DrawPolygon(SHVColor* color, int points, ...)
{
POINT* pts = new POINT[points];
SHVBrushRef brush = GUIManager->CreateBrush(color);
SHVPenRef pen = GUIManager->CreatePen(color);
int dcBackup = ::SaveDC(hDC);

	// assume SHVPoint is of same size as POINT
	SHVASSERT(sizeof(POINT) == sizeof(SHVPoint));

	va_list argList;
	va_start( argList, points );
	for (int i=0; i<points; i++)
		pts[i] = va_arg(argList, POINT);
	va_end(argList);

	::SelectObject(hDC,SHVDrawWin32::GetPen(pen));
	::SelectObject(hDC,SHVDrawWin32::GetBrush(brush));

	::Polygon(hDC,pts,points);

	::RestoreDC(hDC,dcBackup);

	delete [] pts;
	if (color)
		color->ValidateRefCount();
}

/*************************************
 * DrawBitmap
 *************************************/
void SHVDrawWin32::DrawBitmap(SHVBitmap* bitmap, SHVPoint position, int width, int height, SHVColor* transparentColor)
{
HDC bmDC;
HGDIOBJ oldBM;

	if(bitmap && bitmap->IsCreated())
	{
	HBITMAP hBitmap = ((SHVBitmapWin32*)bitmap)->hBitmap;


		if(width == 0 && height == 0)
		{
			width = bitmap->GetWidth();
			height = bitmap->GetHeight();
		}

		bmDC = ::CreateCompatibleDC(hDC);
		oldBM = ::SelectObject(bmDC,hBitmap);

		if(width != bitmap->GetWidth() || height != bitmap->GetHeight())
			::StretchBlt(hDC,position.x, position.y, width, height, bmDC, 0,0, bitmap->GetWidth(), bitmap->GetHeight(), SRCCOPY);
		else
			::BitBlt(hDC,position.x, position.y, width, height, bmDC, 0, 0, SRCCOPY);

		::SelectObject(bmDC,oldBM);
		::DeleteDC(bmDC);
	}

	bitmap->ValidateRefCount();
	if (transparentColor)
		transparentColor->ValidateRefCount();
}
void SHVDrawWin32::DrawBitmap(SHVBitmap* bitmap, SHVPoint position, SHVColor* transparentColor)
{
	DrawBitmap(bitmap,position,0,0,transparentColor);
}

/*************************************
 * DrawBitmapCentered
 *************************************/
void SHVDrawWin32::DrawBitmapCentered(SHVBitmap* bitmap, SHVRect rDest, SHVColor* transparentColor)
{
	bitmap->ValidateRefCount();
	if (transparentColor)
		transparentColor->ValidateRefCount();
}

/*************************************
 * DrawText
 *************************************/
void SHVDrawWin32::DrawText(const SHVStringC txt, SHVRect rect, SHVColor* color, int options)
{
	DrawText(NULL,txt,rect,color,options);
}
void SHVDrawWin32::DrawText(SHVFont* font, const SHVStringC txt, SHVRect rect, SHVColor* color, int options)
{
int dcBackup = ::SaveDC(hDC);
int oldBkMode = ::SetBkMode(hDC,TRANSPARENT);
int nWidth = rect.GetWidth();
SIZE sz;
int mapoptions = DT_NOPREFIX;
RECT rectNative(SHVDrawWin32::MapRect(rect));
bool endEllipsis = ( (options&TextEndEllipsis) ? true : false );

	if (font)
		::SelectObject(hDC,SHVDrawWin32::GetFont(font));
	else if (hWnd)
		::SelectObject(hDC,(HFONT)::SendMessage(hWnd,WM_GETFONT,0,0));

	if (color)
		::SetTextColor(hDC, SHVDrawWin32::GetColor(color));

	if ( (options&TextHCenter) == TextHCenter )
		mapoptions |= DT_CENTER;
	else if ( (options&TextRight) )
		mapoptions |= DT_RIGHT;
	else
		mapoptions |= DT_LEFT;

	if ( (options&TextVCenter) == TextVCenter )
		mapoptions |= DT_VCENTER;
	else if ( (options&TextBottom) )
		mapoptions |= DT_BOTTOM;
	else
		mapoptions |= DT_TOP;

	if ( !(options&TextMultiLine) )
	{
		mapoptions |= DT_SINGLELINE;
	}
	else
	{
		endEllipsis = false;
	}

	::GetTextExtentPoint(hDC,txt.GetSafeBuffer(),(int)txt.GetLength(),&sz);
	if (sz.cx > nWidth && endEllipsis)
	{
	SHVString strTemp;
	size_t i;

		// Text doesn't fit in rect. We have to truncate it and add ellipsis to the end.

		// substract the 3 dots
		::GetTextExtentPoint(hDC, _T("..."), 3, &sz);
		nWidth -= sz.cx;

		for(i=txt.GetLength(); i>0; i--)
		{
			::GetTextExtentPoint(hDC, txt.GetSafeBuffer(), (int)i, &sz);
			if (sz.cx <= nWidth)
				break;
		}

		strTemp = txt.Left(i) + SHVStringC(_T("..."));

		::DrawText(hDC, strTemp.GetSafeBuffer(), (int)strTemp.GetLength(), &rectNative, mapoptions);
	}
	else
	{
		::DrawText(hDC, txt.GetSafeBuffer(), (int)txt.GetLength(), &rectNative, mapoptions);
	}

//	for windows - maybe later -- ::DrawText( hDC, txt.GetSafeBuffer(), -1, &rectNative, mapoptions | (endEllipsis ? DT_END_ELLIPSIS : 0) );

	::SetBkMode(hDC,oldBkMode);
	::RestoreDC(hDC,dcBackup);

	if (font)
		font->ValidateRefCount();
	if (color)
		color->ValidateRefCount();
}


//=========================================================================================================
// SHVDrawPaintWin32 - drawing class for WM_PAINT
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVDrawPaintWin32::SHVDrawPaintWin32(SHVGUIManager* manager, HWND wnd) : SHVDrawWin32(manager,NULL)
{
	hWnd = wnd;
	hDC = ::BeginPaint(hWnd, &ps);

	SHVASSERT(hDC && hWnd);
}

/*************************************
 * Destructor
 *************************************/
SHVDrawPaintWin32::~SHVDrawPaintWin32()
{
	if (hDC)
		::EndPaint(hWnd, &ps);
}
