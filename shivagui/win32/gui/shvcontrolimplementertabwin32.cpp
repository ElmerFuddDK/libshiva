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

#include "shvcontrolimplementertabwin32.h"
#include "shvwin32.h"
#include "utils/shvdrawwin32.h"
#include "utils/shvcolorwin32.h"
#include <commctrl.h>
#include "shvcontrolimplementercontainerwindowwin32.h"


/*************************************
 * Constructor
 *************************************/
SHVControlImplementerTabWin32::SHVControlImplementerTabWin32() : SHVControlImplementerWin32<SHVControlImplementerTab>()
{
	CurrentContainer = NULL;
}

/*************************************
 * Create
 *************************************/
SHVBool SHVControlImplementerTabWin32::Create(SHVControl* owner, SHVControlImplementer* parent, int flags)
{
	if (!IsCreated() && parent && parent->IsCreated())
	{
		SetHandle(CreateWindowEx(WS_EX_NOPARENTNOTIFY|WS_EX_CONTROLPARENT,WC_TABCONTROL, _T(""), WS_CHILD|WS_TABSTOP|Win32::MapFlags(flags)|TCS_TABS|TCS_SINGLELINE,
			0, 0, 0, 0, Win32::GetHandle(parent), NULL, Win32::GetInstance(owner), NULL));

		if (IsCreated())
		{
			OrigProc = (WNDPROC)GetWindowLongPtr(GetHandle(),GWLP_WNDPROC);
			SetWindowLongPtr(GetHandle(),GWLP_USERDATA,(LONG_PTR)owner);
			SetWindowLongPtr(GetHandle(),GWLP_WNDPROC,(LONG_PTR)&SHVControlImplementerTabWin32::WndProc);
		}
		
		owner->SetFont(NULL,true);

		return IsCreated();
	}
	
	return SHVBool::False;
}

/*************************************
 * GetSubType
 *************************************/
int SHVControlImplementerTabWin32::GetSubType(SHVControl* owner)
{
	return SHVControl::SubTypeDefault;
}

/*************************************
 * AddPage
 *************************************/
SHVControlContainer* SHVControlImplementerTabWin32::AddPage(SHVControlTab* owner, const SHVStringC name, size_t atIndex)
{
PageItem* item = new PageItem(name);
SHVControlImplementerContainerWindowWin32* impl;
TCITEM win32item;
bool firstTab = ( GetPageCount(owner) == 0 );

	item->Container = new SHVControlContainer(owner->GetManager(),impl = new SHVControlImplementerContainerWindowWin32(SHVControlContainer::SubTypeTabPage));
	Pages.Add(item);
	impl->Create(item->Container, this, 0);
	item->Container->SetColor(new SHVColorWin32(::GetSysColor(COLOR_MENU)));

	::memset(&win32item,0,sizeof(TCITEM));
	win32item.pszText = (LPTSTR)name.GetSafeBuffer();
	win32item.cchTextMax = (int)name.GetLength();
	win32item.mask = TCIF_TEXT;

	TabCtrl_InsertItem(GetHandle(),TabCtrl_GetItemCount(GetHandle()),&win32item);

	if (firstTab)
	{
		CurrentContainer = item->Container;
		SetContainerSize();
		CurrentContainer->SetFlag(SHVControl::FlagVisible);
		::InvalidateRect(GetHandle(),NULL,TRUE);
	}

	return item->Container;
}

/*************************************
 * GetPage
 *************************************/
SHVControlContainer* SHVControlImplementerTabWin32::GetPage(SHVControlTab* owner, size_t index)
{
	return (index < Pages.CalculateCount() ? Pages[index]->Container : NULL);
}

/*************************************
 * RemovePage
 *************************************/
void SHVControlImplementerTabWin32::RemovePage(SHVControlTab* owner, size_t index)
{
size_t count = Pages.CalculateCount();

	if 	(index < count)
	{
	PageItem* tab = Pages.Replace(index,NULL).ReleaseReference();
	size_t i = index;

		::SendMessage(GetHandle(),WM_SETREDRAW,FALSE,0);

		for (i++;i<count;i++)
		{
			Pages.Replace(i-1, Pages.Replace(i,NULL).ReleaseReference());
		}
		Pages.Truncate();

		if (CurrentContainer == tab->Container)
		{
			if (Pages.CalculateCount())
			{
				CurrentContainer = Pages[0]->Container;
				SetContainerSize();
				CurrentContainer->SetFlag(SHVControl::FlagVisible);
				TabCtrl_SetCurSel(GetHandle(),index ? 0 : 1);
			}
			else
			{
				CurrentContainer = NULL;
			}
		}

		TabCtrl_DeleteItem(GetHandle(),index);

		delete tab;

		::SendMessage(GetHandle(),WM_SETREDRAW,TRUE,0);
		::InvalidateRect(GetHandle(),NULL,TRUE);
		owner->PerformSelectedPageChanged();
	}
}

/*************************************
 * RemovePageByContainer
 *************************************/
void SHVControlImplementerTabWin32::RemovePageByContainer(SHVControlTab* owner, SHVControlContainer* container)
{
size_t pageCount = Pages.CalculateCount();

	for (size_t i=0; container && i<pageCount; i++)
	{
		if (Pages[i]->Container == container)
		{
			RemovePage(owner,i);
			container = NULL;
		}
	}
}

/*************************************
 * Clear
 *************************************/
void SHVControlImplementerTabWin32::Clear(SHVControlTab* owner)
{
	while (GetPageCount(owner))
	{
		RemovePage(owner,GetPageCount(owner)-1);
	}
}

/*************************************
 * GetSelectedPage
 *************************************/
size_t SHVControlImplementerTabWin32::GetSelectedPage(SHVControlTab* owner)
{
	return (size_t)TabCtrl_GetCurSel(GetHandle());
}

/*************************************
 * SelectPage
 *************************************/
void SHVControlImplementerTabWin32::SelectPage(SHVControlTab* owner, size_t index)
{
	if (index < GetPageCount(owner))
	{
		if (CurrentContainer)
			CurrentContainer->SetFlag(SHVControl::FlagVisible,false);
		TabCtrl_SetCurSel(GetHandle(),index);
		CurrentContainer = Pages[index]->Container;
		SetContainerSize();
		CurrentContainer->SetFlag(SHVControl::FlagVisible);
		::InvalidateRect(GetHandle(),NULL,TRUE);
	}
}

/*************************************
 * SetPageForm
 *************************************/
void SHVControlImplementerTabWin32::SetPageForm(SHVControlTab* owner, size_t index, SHVFormBase* form)
{
	if (index < GetPageCount(owner))
	{
		Pages[index]->Form = form;
	}
}

/*************************************
 * GetPageForm
 *************************************/
SHVFormBase* SHVControlImplementerTabWin32::GetPageForm(SHVControlTab* owner, size_t index)
{
	return (index < GetPageCount(owner) ? Pages[index]->Form : NULL);
}

/*************************************
 * GetPageName
 *************************************/
SHVStringBuffer SHVControlImplementerTabWin32::GetPageName(SHVControlTab* owner, size_t index)
{
	return SHVString(index < Pages.CalculateCount() ? Pages[index]->Name.GetBufferConst() : NULL).ReleaseBuffer();
}

/*************************************
 * GetPageCount
 *************************************/
size_t SHVControlImplementerTabWin32::GetPageCount(SHVControlTab* owner)
{
	return Pages.CalculateCount();
}

/*************************************
 * RemovePageByHandle
 *************************************/
void SHVControlImplementerTabWin32::RemovePageByHandle(HWND tabHandle, SHVControlContainer* container)
{
SHVControlTab* owner = (SHVControlTab*)GetWindowLongPtr(tabHandle,GWLP_USERDATA);
SHVControlImplementerTabWin32* self = (owner ? (SHVControlImplementerTabWin32*)owner->GetImplementor() : NULL);

	if (self && self->IsCreated())
		self->RemovePageByContainer(owner,container);
}

///\cond INTERNAL
/*************************************
 * SetContainerSize
 *************************************/
void SHVControlImplementerTabWin32::SetContainerSize()
{
	if (CurrentContainer)
	{
	RECT rect;

		::GetClientRect(GetHandle(),&rect);
		TabCtrl_AdjustRect(GetHandle(),FALSE,&rect);
		CurrentContainer->SetRect(SHVDrawWin32::MapRect(rect));
	}
}

/*************************************
 * SetPageNameByContainer
 *************************************/
void SHVControlImplementerTabWin32::SetPageNameByContainer(SHVControlTab* owner, SHVControlContainer* cont, const SHVStringC name)
{
size_t pagecount = Pages.CalculateCount();

	for (size_t i=0; i<pagecount; i++)
	{
		if (Pages[i]->Container == cont)
		{
		TCITEM item;

			Pages[i]->Name = name;

			::memset(&item,0,sizeof(TCITEM));
			item.pszText = (LPTSTR)name.GetSafeBuffer();
			item.cchTextMax = (int)name.GetLength();
			item.mask = TCIF_TEXT;

			TabCtrl_SetItem(GetHandle(),i,&item);
			break;
		}
	}
}

/*************************************
 * GetPageNameByContainer
 *************************************/
SHVStringBuffer SHVControlImplementerTabWin32::GetPageNameByContainer(SHVControlTab* owner, SHVControlContainer* cont)
{
size_t pagecount = Pages.CalculateCount();
SHVString retVal;

	for (size_t i=0; i<pagecount; i++)
	{
		if (Pages[i]->Container == cont)
		{
			retVal = GetPageName(owner,i);
			break;
		}
	}

	return retVal.ReleaseBuffer();
}

/*************************************
 * WndProc
 *************************************/
LRESULT CALLBACK SHVControlImplementerTabWin32::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
SHVControlTab* owner = (SHVControlTab*)GetWindowLongPtr(hWnd,GWLP_USERDATA);
SHVControlImplementerTabWin32* self = (owner ? (SHVControlImplementerTabWin32*)owner->GetImplementor() : NULL);
SHVControlTabRef refToSelf;
bool processed = false;
LRESULT res = 0;

	SHVASSERT(self);

	switch (message) 
	{
	case WM_KEYDOWN:
	case WM_LBUTTONDOWN:
		{
		int oldSel = TabCtrl_GetCurSel(hWnd);
			processed = true;
			res = CallWindowProc(self->OrigProc,hWnd, message, wParam, lParam);
			if (TabCtrl_GetCurSel(hWnd) != oldSel)
			{
			size_t curSel = (size_t)TabCtrl_GetCurSel(hWnd);
				if (self->CurrentContainer)
					self->CurrentContainer->SetFlag(SHVControl::FlagVisible,false);

				if (curSel < self->GetPageCount(owner))
				{
					self->CurrentContainer = self->Pages[curSel]->Container;
					self->SetContainerSize();
					self->CurrentContainer->SetFlag(SHVControl::FlagVisible,true);
				}
				else
				{
					self->CurrentContainer = NULL;
				}
				owner->PerformSelectedPageChanged();
			}
		}
		break;
#ifdef WM_SIZING
	case WM_SIZING:
		::SendMessage(hWnd,WM_SETREDRAW,FALSE,0);
		break;
#endif
	case WM_SIZE:
		processed = true;
		res = CallWindowProc(self->OrigProc,hWnd, message, wParam, lParam);
		self->SetContainerSize();
#ifdef WM_SIZING
		::SendMessage(hWnd,WM_SETREDRAW,TRUE,0);
#endif
		::InvalidateRect(hWnd,NULL,FALSE);
		break;
	case WM_DESTROY:
		SHVASSERT(!self->IsCreated());
		self->Pages.Clear();
		break;
#ifndef __SHIVA_WINCE
	case WM_ERASEBKGND:

		if (owner)
		{
		SHVDrawWin32Ref draw = Win32::CreateDraw(owner,(HDC)wParam);
		SHVColorRef color = owner->GetParent()->GetColor();

			if (!color.IsNull())
			{
			RECT rect;
			RECT rct2;
			HRGN rgn;

				processed = true;

				GetClientRect(hWnd, &rect);
				rct2 = rect;
				TabCtrl_AdjustRect(hWnd,FALSE,&rct2);
				//::ExcludeClipRect((HDC)wParam,rct2.left,rct2.top,rct2.right,rct2.bottom);
				draw->DrawRectFilled(SHVRect(rect.left,rect.top,rect.right,rct2.top), color);
				//draw->DrawRectFilled(SHVDrawWin32::MapRect(rect), color);
				::ExcludeClipRect((HDC)wParam,rect.left,rect.top,rect.right,rct2.top);
				res = CallWindowProc(self->OrigProc,hWnd, message, wParam, lParam);
				::ExtSelectClipRgn((HDC)wParam,rgn = ::CreateRectRgn(rect.left,rect.top,rect.right,rct2.top),RGN_OR);
				::DeleteObject(rgn);
			}
		}
		break;
#endif
/*	case WM_PAINT:
		if (owner && self->GetTabCount(owner))
		{
			if (self->CurrentContainer)
			{
				// stuff
			}
		}

		res = CallWindowProc(self->OrigProc,hWnd, message, wParam, lParam);

		if (owner && self->GetTabCount(owner))
		{
		HDC hdc = GetDC(hWnd);
		COLORREF cr = ::GetSysColor(COLOR_BTNFACE);
		SHVDrawWin32Ref draw = Win32::CreateDraw(owner,hdc);
		SHVColorWin32Ref color = (SHVColorWin32*)owner->GetParent()->GetColor();
		RECT r;
		HBRUSH brush = ::CreateSolidBrush( color.IsNull() ? cr : color->GetColor() );
		HGDIOBJ oldBrush;

			oldBrush = ::SelectObject(hdc,brush);
			::GetClientRect(hWnd,&r);
			::ExtFloodFill(hdc,r.right-3,3,cr,FLOODFILLSURFACE);
			::SelectObject(hdc,oldBrush);
			::ReleaseDC(hWnd,hdc);
			::DeleteObject(brush);
		}
		break;*/
	}

	if (!processed)
		res = CallWindowProc(self->OrigProc,hWnd, message, wParam, lParam);

	return res;
}
///\endcond
