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

#include "shvcontrolimplementercheckboxwin32.h"
#include "shvwin32.h"

#ifndef MAKEPOINTS
# define MAKEPOINTS(l)       (*((POINTS FAR *)&(l)))
#endif



/*************************************
 * Constructor
 *************************************/
SHVControlImplementerCheckboxWin32::SHVControlImplementerCheckboxWin32(int subType) : SHVControlImplementerWin32<SHVControlImplementerCheckbox>()
{
	SubType = subType;
}

/*************************************
 * Create
 *************************************/
SHVBool SHVControlImplementerCheckboxWin32::Create(SHVControl* owner, SHVControlImplementer* parent, int flags)
{
	if (!IsCreated() && parent && parent->IsCreated())
	{
	int btnStyle = (SubType == SHVControlCheckbox::SubTypeTristate ? BS_AUTO3STATE : BS_AUTOCHECKBOX);
		SetHandle(CreateWindowW(L"BUTTON", L"", WS_CHILD|WS_TABSTOP|Win32::MapFlags(flags)|btnStyle,
			0, 0, 0, 0, Win32::GetHandle(parent), NULL, Win32::GetInstance(owner), NULL));

		if (IsCreated())
		{
			OrigProc =  (WNDPROC)GetWindowLongPtr(GetHandle(),GWLP_WNDPROC);
			SetWindowLongPtr(GetHandle(),GWLP_USERDATA,(LONG_PTR)owner);
			SetWindowLongPtr(GetHandle(),GWLP_WNDPROC,(LONG_PTR)&SHVControlImplementerCheckboxWin32::WndProc);
		}
		
		owner->SetFont(NULL,true);

		return IsCreated();
	}
	
	return SHVBool::False;
}

/*************************************
 * GetSubType
 *************************************/
int SHVControlImplementerCheckboxWin32::GetSubType(SHVControl* owner)
{
	return SHVControl::SubTypeDefault;
}

/*************************************
 * GetText
 *************************************/
SHVStringBuffer SHVControlImplementerCheckboxWin32::GetText()
{
	SHVASSERT(IsCreated());
	return GetWindowTextBase();
}

/*************************************
 * SetText
 *************************************/
void SHVControlImplementerCheckboxWin32::SetText(SHVControlCheckbox* owner, const SHVStringC& text, bool autoSize)
{
	SHVASSERT(IsCreated());

	SetWindowTextBase(text);

	if (autoSize)
	{
	SHVRect rect(GetRect(owner));
	SHVFontRef font = GetFont(owner);

		rect.SetWidth(MulDiv(font->CalculateTextWidth(text),160,100));

		SetRect(owner,rect);
	}
}

/*************************************
 * GetChecked
 *************************************/
SHVInt SHVControlImplementerCheckboxWin32::GetChecked()
{
LRESULT checkResult;

	SHVASSERT(IsCreated());

	checkResult = CallWindowProc(OrigProc, GetHandle(), BM_GETCHECK, 0, 0);

	switch (checkResult)
	{
	case BST_CHECKED:
		return SHVControlCheckbox::StateChecked;
	case BST_UNCHECKED:
		return SHVControlCheckbox::StateUnchecked;
	default:
		SHVASSERT(false);
	case BST_INDETERMINATE:
		return SHVInt();
	}
}

/*************************************
 * SetChecked
 *************************************/
void SHVControlImplementerCheckboxWin32::SetChecked(SHVControlCheckbox* owner, SHVInt state)
{
	SHVASSERT(IsCreated());

	switch (state.IfNull(-1))
	{
	case -1:
		CallWindowProc(OrigProc, GetHandle(), BM_SETCHECK, BST_INDETERMINATE, 0);
		break;
	case SHVControlCheckbox::StateUnchecked:
		CallWindowProc(OrigProc, GetHandle(), BM_SETCHECK, BST_UNCHECKED, 0);
		break;
	default:
		CallWindowProc(OrigProc, GetHandle(), BM_SETCHECK, BST_CHECKED, 0);
		break;
	}
}


///\cond INTERNAL
/*************************************
 * WndProc
 *************************************/
LRESULT CALLBACK SHVControlImplementerCheckboxWin32::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
SHVControlCheckbox* owner = (SHVControlCheckbox*)GetWindowLongPtr(hWnd,GWLP_USERDATA);
SHVControlImplementerCheckboxWin32* self = (owner ? (SHVControlImplementerCheckboxWin32*)owner->GetImplementor() : NULL);
SHVControlCheckboxRef refToSelf;

	switch (message) 
	{
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_LBUTTONUP:
		{
		LRESULT retVal, oldCheckState, newCheckState;

			oldCheckState = CallWindowProc(self->OrigProc,hWnd, BM_GETCHECK, 0, 0);
			retVal = CallWindowProc(self->OrigProc,hWnd, message, wParam, lParam);
			newCheckState = CallWindowProc(self->OrigProc,hWnd, BM_GETCHECK, 0, 0);

			if (newCheckState != oldCheckState)
			{
				refToSelf = owner;
				owner->PerformChanged();
			}

			return retVal;
		}
	default:
		return CallWindowProc(self->OrigProc,hWnd, message, wParam, lParam);
	}
	return 0;
}
///\endcond
