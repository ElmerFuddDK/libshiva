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
#include "../../../include/framework/shveventdata.h"

#include "shvcontrolimplementerlistviewwin32.h"
#include "shvwin32.h"
#include "utils/shvdrawwin32.h"

#include <commctrl.h>

#ifndef LVM_SETEXTENDEDLISTVIEWSTYLE
# define LVM_SETEXTENDEDLISTVIEWSTYLE (LVM_FIRST+54)
#endif
#ifndef LVS_EX_FULLROWSELECT
# define LVS_EX_FULLROWSELECT 32
#endif
#ifndef LVS_EX_HEADERDRAGDROP
# define LVS_EX_HEADERDRAGDROP 16
#endif


//=========================================================================================================
// SHVControlImplementerListViewWin32 - listview implementation
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVControlImplementerListViewWin32::SHVControlImplementerListViewWin32(int subType) : SHVControlImplementerWin32<SHVControlImplementerListViewCustomDraw>()
{
	SubType = subType;
	ColCount = 0;
}

/*************************************
 * Create
 *************************************/
SHVBool SHVControlImplementerListViewWin32::Create(SHVControl* owner, SHVControlImplementer* parent, int flags)
{
	if (!IsCreated() && parent && parent->IsCreated())
	{
	UINT win32flags = LVS_REPORT|LVS_SINGLESEL|LVS_SHOWSELALWAYS|LVS_ALIGNLEFT|LVS_NOSORTHEADER|WS_TABSTOP|WS_CHILD|Win32::MapFlags(flags);

		if (SubType == SHVControlListView::SubTypeCustomDraw)
			win32flags |= LVS_OWNERDRAWFIXED;

		SetHandle(CreateWindowExW(WS_EX_CLIENTEDGE, WC_LISTVIEW, L"", win32flags,
				0, 0, 0, 0, Win32::GetHandle(parent), NULL, Win32::GetInstance(owner), NULL));
	
		if (IsCreated())
		{
			OrigProc = (WNDPROC)GetWindowLongPtr(GetHandle(),GWLP_WNDPROC);
			SetWindowLongPtr(GetHandle(),GWLP_USERDATA,(LONG_PTR)owner);
			SetWindowLongPtr(GetHandle(),GWLP_WNDPROC,(LONG_PTR)&SHVControlImplementerListViewWin32::WndProc);

			owner->SetFont(NULL,true);

			::SendMessage(GetHandle(),LVM_SETEXTENDEDLISTVIEWSTYLE,LVS_EX_FULLROWSELECT,LVS_EX_FULLROWSELECT);
			::SendMessage(GetHandle(),LVM_SETEXTENDEDLISTVIEWSTYLE,LVS_EX_HEADERDRAGDROP,LVS_EX_HEADERDRAGDROP);
		}

		return IsCreated();
	}
	
	return SHVBool::False;
}

/*************************************
 * GetSubType
 *************************************/
int SHVControlImplementerListViewWin32::GetSubType(SHVControl* owner)
{
	return SubType;
}

/*************************************
 * GetItemCount
 *************************************/
size_t SHVControlImplementerListViewWin32::GetItemCount(SHVControlListView* owner)
{
	return (size_t)ListView_GetItemCount(GetHandle());
}

/*************************************
 * ClearItems
 *************************************/
void SHVControlImplementerListViewWin32::ClearItems(SHVControlListView* owner)
{
	ListView_DeleteAllItems(GetHandle());
}

/*************************************
 * GetItemText
 *************************************/
SHVStringBuffer SHVControlImplementerListViewWin32::GetItemText(SHVControlListView* owner, size_t index, size_t col)
{
#if defined(UNICODE)
SHVString16 retVal;
#else
SHVString8 retVal;
#endif
LVITEM item;
size_t len;
size_t result = 0;

	memset(&item, 0, sizeof(LVITEM));
	len = 64;

	item.iItem = (int)index;
	item.iSubItem = (int)col;

	do
	{
		len *= 2;
		retVal.SetBufferSize(len);

		item.cchTextMax = (int)len;
		item.pszText = retVal.GetBufferWin32();
		result  = (size_t)::SendMessage(GetHandle(), LVM_GETITEMTEXT, index, (LPARAM)&item);

	}
	while (result >= len-1);

#if defined(UNICODE) && __SHVSTRINGDEFAULT == 16
	return retVal.ReleaseBuffer();
#elif !defined(UNICODE) && __SHVSTRINGDEFAULT == 8
	return retVal.ReleaseBuffer();
#else
	return retVal.ToStrT();
#endif
}

/*************************************
 * GetItemData
 *************************************/
SHVRefObject* SHVControlImplementerListViewWin32::GetItemData(SHVControlListView* owner, size_t index)
{
LVITEM item;

	memset(&item, 0, sizeof(LVITEM));
	item.mask = LVIF_PARAM;

	item.iItem = (int)index;

	ListView_GetItem(GetHandle(),&item);

	return (SHVRefObject*)item.lParam;
}

/*************************************
 * AddItem
 *************************************/
void SHVControlImplementerListViewWin32::AddItem(SHVControlListView* owner, const SHVStringC str, SHVRefObject* data)
{
LVITEM item;
#if defined(UNICODE) && __SHVSTRINGDEFAULT == 16
const SHVStringC strT(str);
#elif !defined(UNICODE) && __SHVSTRINGDEFAULT == 8
const SHVStringC strT(str);
#elif defined(UNICODE)
SHVString16 strT(str.ToStr16());
#else
SHVString8 strT(str.ToStr8());
#endif

	item.mask = LVIF_TEXT|LVIF_PARAM;
	item.iItem = ListView_GetItemCount(GetHandle());
	item.iSubItem = 0;
	item.pszText = (LPTSTR)strT.GetSafeBuffer();
	item.state = 0;
	item.stateMask = 0;
	item.iImage = 0;
	item.lParam = (LPARAM)(data ? data->CreateRef() : NULL);

	ListView_InsertItem(GetHandle(),&item);
}

/*************************************
 * SetItemText
 *************************************/
void SHVControlImplementerListViewWin32::SetItemText(SHVControlListView* owner, const SHVStringC text, size_t index, size_t col)
{
#if defined(UNICODE) && __SHVSTRINGDEFAULT == 16
const SHVStringC textT(text);
#elif !defined(UNICODE) && __SHVSTRINGDEFAULT == 8
const SHVStringC textT(text);
#elif defined(UNICODE)
SHVString16 textT(text.ToStr16());
#else
SHVString8 textT(text.ToStr8());
#endif
	ListView_SetItemText(GetHandle(),(int)index,(int)col,(LPTSTR)textT.GetSafeBuffer());
}

/*************************************
 * ClearColumns
 *************************************/
void SHVControlImplementerListViewWin32::ClearColumns(SHVControlListView* owner)
{
	while (ColCount)
		ListView_DeleteColumn(GetHandle(),(int)--ColCount);
}

/*************************************
 * AddColumn
 *************************************/
void SHVControlImplementerListViewWin32::AddColumn(SHVControlListView* owner, const SHVStringC colName, SHVInt width)
{
LVCOLUMN column;
#if defined(UNICODE) && __SHVSTRINGDEFAULT == 16
const SHVStringC colNameT(colName);
#elif !defined(UNICODE) && __SHVSTRINGDEFAULT == 8
const SHVStringC colNameT(colName);
#elif defined(UNICODE)
SHVString16 colNameT(colName.ToStr16());
#else
SHVString8 colNameT(colName.ToStr8());
#endif

	column.mask = LVCF_TEXT|LVCF_FMT;
	column.pszText = (LPTSTR)colNameT.GetSafeBuffer();
	column.fmt = LVCFMT_LEFT;

	if (!width.IsNull())
	{
		column.mask |= LVCF_WIDTH;
		column.cx = width;
	}

	ListView_InsertColumn(GetHandle(),ColCount++,&column);
}

/*************************************
 * GetSelected
 *************************************/
SHVInt SHVControlImplementerListViewWin32::GetSelected(SHVControlListView* owner)
{
	return SelectedIndex;
}

/*************************************
 * SetSelected
 *************************************/
void SHVControlImplementerListViewWin32::SetSelected(SHVControlListView* owner, SHVInt index)
{
	if (!SelectedIndex.IsNull())
	{
	int row = SelectedIndex;
		SelectedIndex.SetToNull(); // suppress the selected changed event
		ListView_SetItemState(GetHandle(),row,0,LVIS_SELECTED);
	}

	if (!index.IsNull() && index >= SHVInt(0) && index < SHVInt((int)GetItemCount(owner)))
	{
		SelectedIndex = index;
		ListView_SetItemState(GetHandle(),SelectedIndex,LVIS_SELECTED,LVIS_SELECTED);
	}
}

/*************************************
 * SubscribeDraw
 *************************************/
void SHVControlImplementerListViewWin32::SubscribeDraw(SHVEventSubscriberBase* subscriber)
{
	Subscriber = subscriber;
}

///\cond INTERNAL
/*************************************
 * WndProc
 *************************************/
LRESULT CALLBACK SHVControlImplementerListViewWin32::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
SHVControlListView* owner = (SHVControlListView*)GetWindowLongPtr(hWnd,GWLP_USERDATA);
SHVControlImplementerListViewWin32* self = (owner ? (SHVControlImplementerListViewWin32*)owner->GetImplementor() : NULL);
SHVControlListViewRef refToSelf;
LRESULT retVal = 0;

	switch (message) 
	{
	case WM_SHV_NOTIFY:
		{
		LPNMHDR nmhdr = (LPNMHDR)lParam;
			if (nmhdr->code == LVN_DELETEITEM)
			{
			LPNMLISTVIEW nmlist = (LPNMLISTVIEW)lParam;
				if (nmlist->lParam)
					((SHVRefObject*)nmlist->lParam)->DestroyRef();
			}
			else if (nmhdr->code == LVN_ITEMCHANGED)
			{
			LPNMLISTVIEW nmlist = (LPNMLISTVIEW)lParam;
			SHVInt oldSelected(self->SelectedIndex);

				if ((nmlist->uNewState&LVIS_SELECTED))
					self->SelectedIndex = nmlist->iItem;
				else if (!self->SelectedIndex.IsNull() && self->SelectedIndex == SHVInt(nmlist->iItem) && !(nmlist->uNewState&LVIS_SELECTED))
					self->SelectedIndex.SetToNull();

				if (oldSelected != self->SelectedIndex)
					owner->PerformSelectedChanged();
			}
#ifndef WM_CONTEXTMENU
			else if (nmhdr->code == NM_RCLICK)
			{
				owner->PerformShowContextMenu();
			}
#endif
		}
		retVal = CallWindowProc(self->OrigProc,hWnd, message, wParam, lParam);
		break;
	case WM_SHV_DRAWITEM:
		if (owner)
		{
		LPDRAWITEMSTRUCT drawstr = (LPDRAWITEMSTRUCT)lParam;
		SHVDrawWin32Ref draw = Win32::CreateDraw(owner,drawstr->hDC);

			if (!self->Subscriber.IsNull())
			{
			SHVDrawEventDataListView eventData(draw);
			LV_ITEM lvi;
			LVCOLUMN lvc;
			RECT visibleRect;

				lvi.mask = LVIF_IMAGE|LVIF_STATE;
				lvi.iItem = drawstr->itemID;
				lvi.iSubItem = 0;
				lvi.stateMask = -1;
				ListView_GetItem(hWnd,&lvi);

				eventData.RowID = drawstr->itemID;
				eventData.IsSelected = ( (lvi.state&LVIS_DROPHILITED) || ((lvi.state&LVIS_SELECTED) && ((::GetFocus()==hWnd) || (::GetWindowLong(hWnd,GWL_STYLE)& LVS_SHOWSELALWAYS))));

				refToSelf = owner; // ensure the validity of the object through this function

				eventData.CellRect = SHVDrawWin32::MapRect(drawstr->rcItem);
				eventData.CellRect.SetRight(eventData.CellRect.GetLeft());
				::GetClientRect(hWnd,&visibleRect);

				for(int col=0; ListView_GetColumn(hWnd, col, &lvc); col++)
				{
					eventData.ColumnID = col;
					eventData.CellRect.SetLeft(eventData.CellRect.GetRight());
					eventData.CellRect.SetRight(eventData.CellRect.GetRight() + lvc.cx);
					
					if((eventData.CellRect.GetRight() < visibleRect.left)||
						(eventData.CellRect.GetLeft() > visibleRect.right))
						continue;

					self->Subscriber->Perform(new SHVEventData<SHVDrawEventDataListView>(eventData,NULL,SHVControlListView::EventDrawItem,SHVInt(),owner));
				}
				break;
			}
		}
		else
			retVal = CallWindowProc(self->OrigProc,hWnd, message, wParam, lParam);
		break;
#ifdef WM_CONTEXTMENU
	case WM_CONTEXTMENU:
		if (owner)
			owner->PerformShowContextMenu();
		// else continue
#endif
	default:
		retVal = CallWindowProc(self->OrigProc,hWnd, message, wParam, lParam);
	}
	return retVal;
}
///\endcond
