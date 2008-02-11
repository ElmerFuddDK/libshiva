#include "stdafx.h"
#include "../../../../include/platformspc.h"

#include "shvfontwin32.h"

//=========================================================================================================
// SHVFontWin32
//=========================================================================================================

/*************************************
 * Constructors
 *************************************/
SHVFontWin32::SHVFontWin32(HFONT font, bool owner) : Font(font), Owner(owner)
{
	SHVASSERT(Font);
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

	if  (::GetObject(Font, sizeof(LOGFONT), &lf) != 0)
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

	if  (::GetObject(Font, sizeof(LOGFONT), &lf) != 0)
	{
	HDC hDC = ::GetDC(NULL);

		if (lf.lfHeight < 0)
			lf.lfHeight = -lf.lfHeight;

		retVal = (WORD)MulDiv(lf.lfHeight, 72, GetDeviceCaps(hDC, LOGPIXELSY));

		::ReleaseDC(NULL, hDC);
	}
	return retVal;
}

/*************************************
 * GetWidth
 *************************************/
int SHVFontWin32::GetWidth()
{
	return -1;
}

/*************************************
 * GetName
 *************************************/
SHVStringBuffer SHVFontWin32::GetName()
{
SHVString retVal;
LOGFONT lf;
	if  (::GetObject(Font, sizeof(LOGFONT), &lf) != 0)
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
/*#ifdef __SHIVA_WINCE
LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = 13; // 8DPI size calculated to pixels from the windows PPI factor (96)
	lf.lfWeight = FW_NORMAL;
	_tcscpy(lf.lfFaceName, _T("MS Shell Dlg"));
	return new SHVFontWin32(::CreateFontIndirect(&lf));
#else*/
NONCLIENTMETRICS ncm;

	memset(&ncm, 0, sizeof(NONCLIENTMETRICS));
	ncm.cbSize = sizeof(NONCLIENTMETRICS);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);
	return new SHVFontWin32(::CreateFontIndirect(&ncm.lfMessageFont));
//#endif
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
