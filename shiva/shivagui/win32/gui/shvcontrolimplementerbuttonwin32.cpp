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

#include "shvcontrolimplementerbuttonwin32.h"
#include "shvwin32.h"

#ifndef MAKEPOINTS
# define MAKEPOINTS(l)       (*((POINTS FAR *)&(l)))
#endif



/*************************************
 * Constructor
 *************************************/
SHVControlImplementerButtonWin32::SHVControlImplementerButtonWin32() : SHVControlImplementerWin32<SHVControlImplementerButton>()
{
    ActivationKey = 0;
}

/*************************************
 * Create
 *************************************/
SHVBool SHVControlImplementerButtonWin32::Create(SHVControl* owner, SHVControlImplementer* parent, int flags)
{
	if (!IsCreated() && parent && parent->IsCreated())
	{
		SetHandle(CreateWindow(_T("BUTTON"), _T(""), WS_CHILD|WS_TABSTOP|Win32::MapFlags(flags)|BS_PUSHLIKE,
			0, 0, 0, 0, Win32::GetHandle(parent), NULL, Win32::GetInstance(owner), NULL));

		if (IsCreated())
		{
			OrigProc =  (WNDPROC)GetWindowLongPtr(GetHandle(),GWLP_WNDPROC);
			SetWindowLongPtr(GetHandle(),GWLP_USERDATA,(LONG_PTR)owner);
			SetWindowLongPtr(GetHandle(),GWLP_WNDPROC,(LONG_PTR)&SHVControlImplementerButtonWin32::WndProc);
		}
		
		owner->SetFont(NULL,true);

		return IsCreated();
	}
	
	return SHVBool::False;
}

/*************************************
 * GetSubType
 *************************************/
int SHVControlImplementerButtonWin32::GetSubType(SHVControl* owner)
{
	return SHVControl::SubTypeDefault;
}

/*************************************
 * GetText
 *************************************/
SHVStringBuffer SHVControlImplementerButtonWin32::GetText()
{
SHVString retVal;

	SHVASSERT(IsCreated());

	retVal.SetBufferSize( GetWindowTextLength(GetHandle())+1 );
	GetWindowText(GetHandle(),(TCHAR*)retVal.GetBuffer(), (int)retVal.GetBufferLen());

	return retVal.ReleaseBuffer();
}

/*************************************
 * SetText
 *************************************/
void SHVControlImplementerButtonWin32::SetText(SHVControlButton* owner, const SHVStringC& text, bool autoSize)
{
	SHVASSERT(IsCreated());

	SetWindowText(GetHandle(),(const TCHAR*)text.GetSafeBuffer());

	if (autoSize)
	{
	SHVRect rect(GetRect(owner));
	SHVFontRef font = GetFont(owner);

		rect.SetWidth(MulDiv(font->CalculateTextWidth(text),160,100));

		SetRect(owner,rect);
	}
}

///\cond INTERNAL
/*************************************
 * WndProc
 *************************************/
LRESULT CALLBACK SHVControlImplementerButtonWin32::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
SHVControlButton* owner = (SHVControlButton*)GetWindowLongPtr(hWnd,GWLP_USERDATA);
SHVControlImplementerButtonWin32* self = (owner ? (SHVControlImplementerButtonWin32*)owner->GetImplementor() : NULL);
SHVControlButtonRef refToSelf;

	switch (message) 
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_RETURN:
		case VK_SPACE:
			self->ActivationKey = wParam;
			break;
		}
		return CallWindowProc(self->OrigProc,hWnd, message, wParam, lParam);
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_RETURN:
		case VK_SPACE:
			if (self->ActivationKey == wParam)
			{
				self->ActivationKey = 0;
				refToSelf = owner; // ensure the validity of the object through this function
				owner->PerformClicked();
			}
			break;
		}
		return CallWindowProc(self->OrigProc,hWnd, message, wParam, lParam);
	case WM_LBUTTONUP:
		{
		POINTS p = MAKEPOINTS(lParam);
		RECT rect;

			::GetWindowRect(hWnd,&rect);

			if (p.x >= 0 && p.x < (rect.right-rect.left) &&
				p.y >= 0 && p.y < (rect.bottom-rect.top))
			{
				refToSelf = owner; // ensure the validity of the object through this function
				owner->PerformClicked();
			}
		}
	default:
		return CallWindowProc(self->OrigProc,hWnd, message, wParam, lParam);
	}
	return 0;
}
///\endcond
