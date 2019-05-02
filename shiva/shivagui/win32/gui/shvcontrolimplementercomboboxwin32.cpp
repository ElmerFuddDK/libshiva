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

#include "shvcontrolimplementercomboboxwin32.h"
#include "shvwin32.h"

#ifndef MAKEPOINTS
# define MAKEPOINTS(l)       (*((POINTS FAR *)&(l)))
#endif



//=========================================================================================================
// SHVControlImplementerComboBoxWin32
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVControlImplementerComboBoxWin32::SHVControlImplementerComboBoxWin32(int subType)
	: SHVControlImplementerWin32<SHVControlImplementerComboBox>(), SubType(subType)
{
}

/*************************************
 * Create
 *************************************/
SHVBool SHVControlImplementerComboBoxWin32::Create(SHVControl* owner, SHVControlImplementer* parent, int flags)
{
	if (!IsCreated() && parent && parent->IsCreated())
	{
	int styles = Win32::MapFlags(flags)|WS_CHILD|WS_TABSTOP|WS_BORDER|WS_VSCROLL|CBS_HASSTRINGS;

		switch (SubType)
		{
		case SHVControlComboBox::SubTypeEdit:
			styles |= CBS_DROPDOWN;
			break;
		default:
		case SHVControlComboBox::SubTypeList:
			styles |= CBS_DROPDOWNLIST;
			break;
		}

		SetHandle(CreateWindowW(L"COMBOBOX", L"", styles,
			0, 0, 0, 0, Win32::GetHandle(parent), NULL, Win32::GetInstance(owner), NULL));

		if (IsCreated())
		{
			OrigProc =  (WNDPROC)GetWindowLongPtr(GetHandle(),GWLP_WNDPROC);
			SetWindowLongPtr(GetHandle(),GWLP_USERDATA,(LONG_PTR)owner);
			SetWindowLongPtr(GetHandle(),GWLP_WNDPROC,(LONG_PTR)&SHVControlImplementerComboBoxWin32::WndProc);
		}
		
		owner->SetFont(NULL,true);

		return IsCreated();
	}
	
	return SHVBool::False;
}

/*************************************
 * GetSubType
 *************************************/
int SHVControlImplementerComboBoxWin32::GetSubType(SHVControl* owner)
{
	return SubType;
}

/*************************************
 * GetText
 *************************************/
SHVStringBuffer SHVControlImplementerComboBoxWin32::GetText(SHVControlComboBox* owner)
{
	SHVASSERT(IsCreated());
	return GetWindowTextBase();
}

/*************************************
 * SetText
 *************************************/
void SHVControlImplementerComboBoxWin32::SetText(SHVControlComboBox* owner, const SHVStringC& text)
{
	SHVASSERT(IsCreated());
	SetWindowTextBase(text);
}

/*************************************
 * GetSelected
 *************************************/
SHVInt SHVControlImplementerComboBoxWin32::GetSelected(SHVControlComboBox* owner)
{
SHVInt retVal;
int val;

	SHVASSERT(IsCreated());

	val = (int)::SendMessage(GetHandle(),CB_GETCURSEL,0,0);

	if (val != CB_ERR)
		retVal = val;

	return retVal;
}

/*************************************
 * SetSelected
 *************************************/
void SHVControlImplementerComboBoxWin32::SetSelected(SHVControlComboBox* owner, SHVInt index)
{
	SHVASSERT(IsCreated());

	::SendMessage(GetHandle(),CB_SETCURSEL,index.IfNull(-1),0);
}

/*************************************
 * GetItemCount
 *************************************/
size_t SHVControlImplementerComboBoxWin32::GetItemCount(SHVControlComboBox* owner)
{
int val;
	SHVASSERT(IsCreated());

	val = (int)::SendMessage(GetHandle(),CB_GETCOUNT,0,0);

	if (val < 0)
		val = 0;

	return (size_t)val;
}

/*************************************
 * ClearItems
 *************************************/
void SHVControlImplementerComboBoxWin32::ClearItems(SHVControlComboBox* owner)
{
	SHVASSERT(IsCreated());

	::SendMessage(GetHandle(),CB_RESETCONTENT,0,0);
}

/*************************************
 * GetItemText
 *************************************/
SHVStringBuffer SHVControlImplementerComboBoxWin32::GetItemText(SHVControlComboBox* owner, size_t index)
{
SHVString retVal;

	if (index < GetItemCount(owner))
	{
#if defined(UNICODE) && __SHVSTRINGDEFAULT == 16
		retVal.SetBufferSize(::SendMessage(GetHandle(),CB_GETLBTEXTLEN,(WPARAM)index,0)+1);
		::SendMessage(GetHandle(),CB_GETLBTEXT,(WPARAM)index,(LPARAM)retVal.GetBuffer());
#elif !defined(UNICODE) && __SHVSTRINGDEFAULT == 8
		retVal.SetBufferSize(::SendMessage(GetHandle(),CB_GETLBTEXTLEN,(WPARAM)index,0)+1);
		::SendMessage(GetHandle(),CB_GETLBTEXT,(WPARAM)index,(LPARAM)retVal.GetBuffer());
#elif defined(UNICODE)
	SHVString16 retValW;
		retValW.SetBufferSize(::SendMessage(GetHandle(),CB_GETLBTEXTLEN,(WPARAM)index,0)+1);
		::SendMessage(GetHandle(),CB_GETLBTEXT,(WPARAM)index,(LPARAM)retValW.GetBuffer());
		retVal = retValW.ToStrT();
#else
	SHVString8 retValA;
		retValA.SetBufferSize(::SendMessage(GetHandle(),CB_GETLBTEXTLEN,(WPARAM)index,0)+1);
		::SendMessage(GetHandle(),CB_GETLBTEXT,(WPARAM)index,(LPARAM)retValA.GetBuffer());
		retVal = retValA.ToStrT();
#endif
	}

	return retVal.ReleaseBuffer();
}

/*************************************
 * GetItemData
 *************************************/
SHVRefObject* SHVControlImplementerComboBoxWin32::GetItemData(SHVControlComboBox* owner, size_t index)
{
SHVRefObject* retVal = NULL;

	if (index < GetItemCount(owner))
	{
		///\todo Check up on the way we store refcounting objects in a combo box is valid in win64
		retVal = (SHVRefObject*)::SendMessage(GetHandle(),CB_GETITEMDATA,(WPARAM)index,0);
	}

	return retVal;
}

/*************************************
 * AddItem
 *************************************/
void SHVControlImplementerComboBoxWin32::AddItem(SHVControlComboBox* owner, const SHVStringC str, SHVRefObject* data)
{
int idx;
#if defined(UNICODE) && __SHVSTRINGDEFAULT == 16
const SHVString16C strT(str);
#elif !defined(UNICODE) && __SHVSTRINGDEFAULT == 8
const SHVString8C strT(str);
#elif defined(UNICODE)
SHVString16 strT(str.ToStr16());
#else
SHVString8 strT(str.ToStr8());
#endif

	idx = (int)::SendMessage(GetHandle(),CB_INSERTSTRING,(WPARAM)-1,(LPARAM)strT.GetSafeBuffer());

	if (idx != CB_ERR)
		::SendMessage(GetHandle(),CB_SETITEMDATA,(WPARAM)idx,(LPARAM)(data ? data->CreateRef() : NULL));
	else if (data)
		data->ValidateRefCount();
}

/*************************************
 * SetDropdownHeight
 *************************************/
void SHVControlImplementerComboBoxWin32::SetDropdownHeight(SHVControlComboBox* owner, int lines)
{
SHVRect rect(owner->GetRect());
int itmHght;

	SHVASSERT(IsCreated());

	itmHght = (int)::SendMessage(GetHandle(),CB_GETITEMHEIGHT,0,0);

	if (itmHght > 0)
	{
		rect.SetHeight(rect.GetHeight() + itmHght*lines);
		owner->SetRect(rect);
	}
}

///\cond INTERNAL
/*************************************
 * WndProc
 *************************************/
LRESULT CALLBACK SHVControlImplementerComboBoxWin32::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
SHVControlComboBox* owner = (SHVControlComboBox*)GetWindowLongPtr(hWnd,GWLP_USERDATA);
SHVControlImplementerComboBoxWin32* self = (owner ? (SHVControlImplementerComboBoxWin32*)owner->GetImplementor() : NULL);
SHVControlComboBoxRef refToSelf;
LRESULT retVal = 0;

	switch (message)
	{
	case WM_COMMAND:
		retVal = CallWindowProc(self->OrigProc,hWnd, message, wParam, lParam);
		if (owner->GetSelected() != self->Selected)
		{
			self->Selected = owner->GetSelected();
			owner->PerformSelectedChanged();
		}
		break;
	case WM_DESTROY:
		::SendMessage(hWnd,CB_RESETCONTENT,0,0);
		retVal = CallWindowProc(self->OrigProc,hWnd, message, wParam, lParam);
		break;
	case CB_RESETCONTENT:
		{
		int count = (int)::SendMessage(hWnd,CB_GETCOUNT,0,0);

			while (count > 0)
			{
				count--;
				::SendMessage(hWnd,CB_DELETESTRING,(WPARAM)count,0);
			}
		}
		break;
	case CB_DELETESTRING:
		{
		SHVRefObject* ref = (SHVRefObject*)::SendMessage(hWnd,CB_GETITEMDATA,wParam,0);
			if (ref)
				ref->DestroyRef();
		}
		retVal = CallWindowProc(self->OrigProc,hWnd, message, wParam, lParam);
		break;
	default:
		retVal = CallWindowProc(self->OrigProc,hWnd, message, wParam, lParam);
		break;
	}
	return retVal;
}
///\endcond
