#include "stdafx.h"
#include "../../../../include/platformspc.h"

#include "shvbitmapwin32.h"


//=========================================================================================================
// SHVBitmapWin32
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVBitmapWin32::SHVBitmapWin32(HINSTANCE hInstance, int resID) : SHVBitmap()
{
	//hBitmap = (HBITMAP)::LoadImage(hInstance, MAKEINTRESOURCE(resID), IMAGE_BITMAP, 0,0, LR_CREATEDIBSECTION);
	hBitmap = ::LoadBitmap(hInstance,MAKEINTRESOURCE(resID));

	if (IsCreated())
	{
	BITMAP bmInfo;
		::GetObject(hBitmap, sizeof(BITMAP), &bmInfo);
		Width = bmInfo.bmWidth;
		Height = bmInfo.bmHeight;
	}
}

/*************************************
 * Destructor
 *************************************/
SHVBitmapWin32::~SHVBitmapWin32()
{
	if (hBitmap)
		::DeleteObject(hBitmap);
}

/*************************************
 * IsCreated
 *************************************/
SHVBool SHVBitmapWin32::IsCreated()
{
	return (hBitmap ? SHVBool::True : SHVBool::False);
}

/*************************************
 * GetWidth
 *************************************/
SHVInt SHVBitmapWin32::GetWidth()
{
	return Width;
}

/*************************************
 * GetHeight
 *************************************/
SHVInt SHVBitmapWin32::GetHeight()
{
	return Height;
}
