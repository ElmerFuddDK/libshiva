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
#include "../../../include/platformspc.h"

#include "shvcontrolimplementerwin32base.h"
#include "shvwin32.h"
#include "utils/shvfontwin32.h"
#include "utils/shvdrawwin32.h"

//=========================================================================================================
// SHVControlImplementerWin32Base - implementation of base implementer methods
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVControlImplementerWin32Base::SHVControlImplementerWin32Base()
{
	Window = Win32::InvalidHandle();
}

/*************************************
 * Destructor
 *************************************/
SHVControlImplementerWin32Base::~SHVControlImplementerWin32Base()
{
	SHVASSERT(!IsCreated());
}

/*************************************
 * IsCreated
 *************************************/
SHVBool SHVControlImplementerWin32Base::IsCreated()
{
	return (Window != Win32::InvalidHandle());
}

/*************************************
 * Reparent
 *************************************/
SHVBool SHVControlImplementerWin32Base::Reparent(SHVControl* owner, SHVControlImplementer* parent, int flags)
{
HWND prevParent;

	SHVASSERT(IsCreated() && parent->IsCreated());

	prevParent = ::SetParent(Window,Win32::GetHandle(parent));
	SetFlag(owner, flags);

	return prevParent != NULL;
}

/*************************************
 * Destroy
 *************************************/
SHVBool SHVControlImplementerWin32Base::Destroy(SHVControl* owner)
{
SHVBool retVal(IsCreated());

	if (retVal)
	{
	HWND oldwindow = Window;

		Window = Win32::InvalidHandle();
		::DestroyWindow(oldwindow);

		Font = NULL;
	}

	return retVal;
}

/*************************************
 * GetRect
 *************************************/
SHVRect SHVControlImplementerWin32Base::GetRect(SHVControl* owner)
{
RECT nativeRect;

	SHVASSERT(IsCreated() && owner && owner->GetImplementor()->GetNative() == this);

	::GetWindowRect(Window,&nativeRect);

	if (owner->GetParent())
	{
	POINT topleft, bottomright;

		topleft.x = nativeRect.left;
		topleft.y = nativeRect.top;
		bottomright.x = nativeRect.right;
		bottomright.y = nativeRect.bottom;

		ScreenToClient(Win32::GetHandle(owner->GetParent()),&topleft);
		ScreenToClient(Win32::GetHandle(owner->GetParent()),&bottomright);

		return SHVRect(topleft.x,topleft.y,bottomright.x,bottomright.y);
	}

	return SHVDrawWin32::MapRect(nativeRect);
}

/*************************************
 * SetRect
 *************************************/
void SHVControlImplementerWin32Base::SetRect(SHVControl* owner, const SHVRect& rect)
{
	SHVASSERT(IsCreated());
	::MoveWindow(Window,rect.GetLeft(),rect.GetTop(),rect.GetWidth(),rect.GetHeight(),TRUE);
}

/*************************************
 * SetFlag
 *************************************/
SHVBool SHVControlImplementerWin32Base::SetFlag(SHVControl* owner, int flag, bool enable)
{
SHVBool retVal(IsCreated());

	if (retVal && (flag & SHVControl::FlagVisible))
	{
		::ShowWindow(Window, enable ? SW_SHOW : SW_HIDE);
	}

	if (retVal && (flag & SHVControl::FlagDisabled))
	{
		::EnableWindow(Window, !enable);
	}

	return retVal;
}

/*************************************
 * GetFlag
 *************************************/
bool SHVControlImplementerWin32Base::GetFlag(SHVControl* owner, int flag)
{
bool retVal(IsCreated());
DWORD styles = (retVal ? ::GetWindowLong(Window,GWL_STYLE) : 0);

	if (retVal && (flag & SHVControl::FlagVisible))
	{
		retVal = ((styles & WS_VISIBLE) ? true : false);
	}

	if (retVal && (flag & SHVControl::FlagDisabled))
	{
		retVal = ((styles & WS_DISABLED) ? true : false);
	}

	return retVal;
}

/*************************************
 * GetFont
 *************************************/
SHVFont* SHVControlImplementerWin32Base::GetFont(SHVControl* owner)
{
	return Font;
}

/*************************************
 * SetFont
 *************************************/
SHVBool SHVControlImplementerWin32Base::SetFont(SHVControl* owner, SHVFont* font, SHVInt newHeight)
{
SHVBool retVal(IsCreated());

	if (retVal && font)
	{
		Font = (SHVFontWin32*)font;

		::SendMessage(GetHandle(),WM_SETFONT,(WPARAM)Font->GetFont(),0);

		if (!newHeight.IsNull())
		{
		RECT rect;

			::GetWindowRect(Window,&rect);
			::MoveWindow(Window,rect.left,rect.top,rect.right-rect.left,newHeight,TRUE);
		}
	}

	return retVal;
}

/*************************************
 * GetText
 *************************************/
SHVStringBuffer SHVControlImplementerWin32Base::GetText()
{
SHVString retVal;

	SHVASSERT(IsCreated());

	retVal = GetWindowTextBase();

	if (Win32::CheckForNewlines(retVal))
		Win32::ConvertNewlinesBack(retVal);

	return retVal.ReleaseBuffer();
}

/*************************************
 * SetText
 *************************************/
void SHVControlImplementerWin32Base::SetText(const SHVStringC& text)
{
	SHVASSERT(IsCreated());

	if (Win32::CheckForNewlines(text))
		SetWindowTextBase(Win32::ConvertNewlinesC(text));
	else
		SetWindowTextBase(text);
}

/*************************************
 * CalculateMinSize
 *************************************/
SHVPoint SHVControlImplementerWin32Base::CalculateMinSize(SHVControl* owner, int widthInChars, int heightInChars)
{

	SHVASSERT(IsCreated());

	if (widthInChars || heightInChars)
	{
	SHVFontRef font = GetFont(owner);

		if (font.IsNull())
			font = owner->GetManager()->GetFont(SHVGUIManager::CfgFontNormal);

		return SHVPoint( widthInChars * font->GetApproximateWidth(), heightInChars * font->GetCellHeight() );
	}

	return SHVPoint();
}

void SHVControlImplementerWin32Base::SetResizable(bool resizable)
{
#ifndef __SHIVA_POCKETPC
DWORD style = ::GetWindowLong(GetHandle(), GWL_STYLE);
DWORD oldStyle = style;

	// only modify the style if resizable is different from what it was before
	if (resizable)
		style |= WS_THICKFRAME;
	else
		style = (style & ~WS_VISIBLE);

	if (style != oldStyle)
		::SetWindowLong(GetHandle(), GWL_STYLE, style);
#endif
}

/*************************************
 * GetHandle
 *************************************/
HWND SHVControlImplementerWin32Base::GetHandle()
{
	return Window;
}

/*************************************
 * SetHandle
 *************************************/
void SHVControlImplementerWin32Base::SetHandle(HWND handle)
{
	if (Window == handle)
		return;

	SHVASSERT(!IsCreated());
	Window = handle;
}

/*************************************
 * ModifyStyle
 *************************************/
void SHVControlImplementerWin32Base::ModifyStyle(int set, int remove)
{
DWORD style = ::GetWindowLong(GetHandle(), GWL_STYLE);
DWORD oldStyle = style;

	style |= set;
	style = (style & ~remove);

	if (style != oldStyle)
		::SetWindowLong(GetHandle(), GWL_STYLE, style);
}

/*************************************
 * ModifyStyleEx
 *************************************/
void SHVControlImplementerWin32Base::ModifyStyleEx(int set, int remove)
{
DWORD style = ::GetWindowLong(GetHandle(), GWL_EXSTYLE);
DWORD oldStyle = style;

	style |= set;
	style = (style & ~remove);

	if (style != oldStyle)
		::SetWindowLong(GetHandle(), GWL_EXSTYLE, style);
}

/*************************************
 * GetWindowTextBase
 *************************************/
SHVStringBuffer SHVControlImplementerWin32Base::GetWindowTextBase()
{

#if defined(UNICODE) && __SHVSTRINGDEFAULT == 16
SHVString retVal;
	retVal.SetBufferSize( GetWindowTextLength(GetHandle())+1 );
	GetWindowText(GetHandle(),retVal.GetBufferWin32(), (int)retVal.GetBufferLen());
	return retVal.ReleaseBuffer();
#elif !defined(UNICODE) && __SHVSTRINGDEFAULT == 8
SHVString retVal;
	retVal.SetBufferSize( GetWindowTextLength(GetHandle())+1 );
	GetWindowText(GetHandle(),retVal.GetBuffer(), (int)retVal.GetBufferLen());
	return retVal.ReleaseBuffer();
#else
SHVString16 retVal;
	retVal.SetBufferSize( GetWindowTextLength(GetHandle())+1 );
	GetWindowTextW(GetHandle(),retVal.GetBufferWin32(), (int)retVal.GetBufferLen());
# if __SHVSTRINGDEFAULT == 16
	return retVal.ReleaseBuffer();
# else
	return retVal.ToStrT();
# endif
#endif
}

/*************************************
 * SetWindowTextBase
 *************************************/
void SHVControlImplementerWin32Base::SetWindowTextBase(const SHVStringC text)
{
#if defined(UNICODE) && __SHVSTRINGDEFAULT == 16
	::SetWindowText(GetHandle(),text.GetSafeBufferWin32());
#elif !defined(UNICODE) && __SHVSTRINGDEFAULT == 8
	::SetWindowText(GetHandle(),text.GetSafeBuffer());
#else
	::SetWindowTextW(GetHandle(),text.AsStr16C().GetSafeBufferWin32());
#endif
}
