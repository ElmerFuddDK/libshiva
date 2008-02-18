#include "stdafx.h"
#include "../../../../include/platformspc.h"

#include "shvdrawwin32.h"
#include "../shvwin32.h"

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
	color->ValidateRefCount();
}
void SHVDrawWin32::DrawLine(SHVPoint from, SHVPoint to, SHVPen* pen)
{
int dcBackup = ::SaveDC(hDC);
POINT point;

	::SelectObject(hDC, SHVDrawWin32::GetPen(pen));

	::MoveToEx(hDC, from.x, from.y, &point);
	::LineTo(hDC, to.x, to.y);

	::RestoreDC(hDC,dcBackup);

	pen->ValidateRefCount();
}

/*************************************
 * DrawPixel
 *************************************/
void SHVDrawWin32::DrawPixel(SHVPoint where, SHVColor* color)
{
	::SetPixel(hDC, where.x, where.y, SHVDrawWin32::GetColor(color));
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

	::SelectObject(hDC, SHVDrawWin32::GetPen(pen));
	
	::MoveToEx(hDC, rect.GetLeft(), rect.GetTop(), &point);
	::LineTo(hDC, rect.GetRight(), rect.GetTop());
	::LineTo(hDC, rect.GetRight(), rect.GetBottom());
	::LineTo(hDC, rect.GetLeft(), rect.GetBottom());
	::LineTo(hDC, rect.GetLeft(), rect.GetTop());

	::RestoreDC(hDC,dcBackup);
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
	color->ValidateRefCount();
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
	color->ValidateRefCount();
}

/*************************************
 * DrawBitmap
 *************************************/
void SHVDrawWin32::DrawBitmap(SHVBitmap* bitmap, SHVPoint position, int width, int height, SHVColor* transparentColor)
{
	bitmap->ValidateRefCount();
}
void SHVDrawWin32::DrawBitmap(SHVBitmap* bitmap, SHVPoint position, SHVColor* transparentColor)
{
	bitmap->ValidateRefCount();
}

/*************************************
 * DrawBitmapCentered
 *************************************/
void SHVDrawWin32::DrawBitmapCentered(SHVBitmap* bitmap, SHVRect rDest, SHVColor* transparentColor)
{
	bitmap->ValidateRefCount();
}

/*************************************
 * DrawText
 *************************************/
void SHVDrawWin32::DrawText(const SHVStringC txt, SHVRect rect, int options)
{
	DrawText(NULL,txt,rect,options);
}
void SHVDrawWin32::DrawText(SHVFont* font, const SHVStringC txt, SHVRect rect, int options)
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
