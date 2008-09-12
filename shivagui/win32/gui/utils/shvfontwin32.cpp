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

#include "shvfontwin32.h"
#include "../shvwin32.h"


//=========================================================================================================
// SHVFontWin32
//=========================================================================================================

/*************************************
 * Constructors
 *************************************/
SHVFontWin32::SHVFontWin32(HFONT font, bool owner) : Font(font), Owner(owner)
{
	SHVASSERT(Font);
	CellHeight = -1;
	ApproximateWidth = -1;
}
SHVFontWin32::SHVFontWin32(const SHVStringC fontName, int height, int styles)
{
LOGFONT lf;

	Owner = true;

	// Create font
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = -height;
	lf.lfWeight = ( styles & SHVFont::StyleBold ? FW_BOLD : FW_NORMAL );
	lf.lfItalic = ( styles & SHVFont::StyleItalic ? TRUE : FALSE );
	lf.lfUnderline = ( styles & SHVFont::StyleUnderline ? TRUE : FALSE );
	::memcpy( lf.lfFaceName, fontName.GetSafeBuffer(), (fontName.GetLength()+1)*sizeof(SHVTChar) );
	SHVVERIFY(Font = ::CreateFontIndirect(&lf));
	CellHeight = -1;
	ApproximateWidth = -1;
}

/*************************************
 * Destructor
 *************************************/
SHVFontWin32::~SHVFontWin32()
{
	if (Font && Owner)
		::DeleteObject(Font);
}

/*************************************
 * CreateCopy
 *************************************/
SHVFont* SHVFontWin32::CreateCopy(int sizePercentage, int styles)
{
SHVFontWin32* retVal = NULL;
LOGFONT lf;

	if  (::GDIGetObject(Font, sizeof(LOGFONT), &lf) != 0)
	{
		if (sizePercentage != 100)
		{
			lf.lfHeight = (lf.lfHeight*sizePercentage)/100;
		}

		if (styles != SHVFont::StyleUnchanged)
		{
			lf.lfWeight = ( styles & SHVFont::StyleBold ? FW_BOLD : FW_NORMAL );
			lf.lfItalic = ( styles & SHVFont::StyleItalic ? TRUE : FALSE );
			lf.lfUnderline = ( styles & SHVFont::StyleUnderline ? TRUE : FALSE );
		}

		retVal = new SHVFontWin32(::CreateFontIndirect(&lf));
	}

	return retVal;
}

/*************************************
 * GetHeight
 *************************************/
int SHVFontWin32::GetHeight()
{
int retVal = -1;
LOGFONT lf;

	if  (::GDIGetObject(Font, sizeof(LOGFONT), &lf) != 0)
	{
	HDC hDC = ::GetDC(NULL);

		SHVASSERT(lf.lfHeight < 0);

		if (lf.lfHeight < 0)
			lf.lfHeight = -lf.lfHeight;

		retVal = (WORD)MulDiv(lf.lfHeight, 72, GetDeviceCaps(hDC, LOGPIXELSY));

		::ReleaseDC(NULL, hDC);
	}
	return retVal;
}

/*************************************
 * GetCellHeight
 *************************************/
int SHVFontWin32::GetCellHeight()
{
	if (CellHeight == -1)
		CellHeight = SHVFontWin32::CalculateCellHeight(Font);
	return CellHeight;
}

/*************************************
 * GetApproximateWidth
 *************************************/
int SHVFontWin32::GetApproximateWidth()
{
	if (ApproximateWidth == -1)
		ApproximateWidth = CalculateTextWidth(_T("W"));
	return ApproximateWidth;
}

/*************************************
 * GetWidth
 *************************************/
int SHVFontWin32::CalculateTextWidth(const SHVStringC text)
{
HDC dc = ::GetDC(NULL);
int dcBackup = ::SaveDC(dc);
SIZE sz;
int retVal;

	SHVASSERT(!Win32::CheckForNewlines(text));

	::SelectObject(dc,Font);
	
	SHVVERIFY(::GetTextExtentPoint(dc,text.GetSafeBuffer(),(int)text.GetLength(),&sz));
	retVal = sz.cx;

	::RestoreDC(dc,dcBackup);
	::ReleaseDC(NULL,dc);

	return retVal;
}

/*************************************
 * GetName
 *************************************/
SHVStringBuffer SHVFontWin32::GetName()
{
SHVString retVal;
LOGFONT lf;
	if  (::GDIGetObject(Font, sizeof(LOGFONT), &lf) != 0)
	{
		retVal = lf.lfFaceName;
	}
	return retVal.ReleaseBuffer();
}

/*************************************
 * CreateSystemFont
 *************************************/
SHVFontWin32* SHVFontWin32::CreateSystemFont()
{
#if defined(__SHIVA_WINCE) && (_WIN32_WCE < 500)
LOGFONT lf;

	///\todo Implement a way to get the real message font from the system on windows CE

	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = 13; // 8DPI size calculated to pixels from the windows PPI factor (96)
	lf.lfWeight = FW_NORMAL;
	_tcscpy(lf.lfFaceName, _T("MS Shell Dlg"));
	return new SHVFontWin32(::CreateFontIndirect(&lf));
#elif defined(__SHIVA_WINCE)
	///\todo Test if this method works for older wince's
	return new SHVFontWin32((HFONT)::GetStockObject(SYSTEM_FONT),false);
#else
NONCLIENTMETRICS ncm;

	memset(&ncm, 0, sizeof(NONCLIENTMETRICS));
	ncm.cbSize = sizeof(NONCLIENTMETRICS);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);
	return new SHVFontWin32(::CreateFontIndirect(&ncm.lfMessageFont));
#endif
}

/*************************************
 * MapStyles
 *************************************/
int SHVFontWin32::MapStyles(LOGFONT& lf)
{
int retVal = SHVFont::StyleNormal;

	if (lf.lfWeight != FW_NORMAL)
		retVal |= SHVFont::StyleBold;
	if (lf.lfItalic)
		retVal |= SHVFont::StyleItalic;
	if (lf.lfUnderline)
		retVal |= SHVFont::StyleUnderline;

	return retVal;
}

/*************************************
 * CalculateCellHeight
 *************************************/
int SHVFontWin32::CalculateCellHeight(HFONT font)
{
HDC dc = ::GetDC(NULL);
int dcBackup = ::SaveDC(dc);
SIZE sz;
int retVal;

	::SelectObject(dc,font);
	
	SHVVERIFY(::GetTextExtentPoint(dc,_T(" "),1,&sz));
	retVal = sz.cy;

	::RestoreDC(dc,dcBackup);
	::ReleaseDC(NULL,dc);

	return retVal;
}
