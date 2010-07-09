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

#include "shvcontrolimplementercontainerwindowwin32.h"
#include "shvcontrolimplementertabwin32.h"
#include "shvmainthreadeventdispatcherwin32.h"
#include "shvwin32.h"
#include "utils/shvdrawwin32.h"


#define SHVWIN32CLASS_CONTAINERWND _T("SHV_CntWnd")
#define SHVWIN32ATOM_CONTAINERWND _S("shv_containerwndatom")

//=========================================================================================================
// SHVControlImplementerContainerWindowWin32
//=========================================================================================================


/*************************************
 * Constructor
 *************************************/
SHVControlImplementerContainerWindowWin32::SHVControlImplementerContainerWindowWin32(int subType) : SHVControlImplementerWin32<SHVControlImplementerContainerCustomDraw>()
{
	SubType = subType;
}

/*************************************
 * GetSubType
 *************************************/
int SHVControlImplementerContainerWindowWin32::GetSubType(SHVControl* owner)
{
	return SubType;
}

/*************************************
 * Create(parent)
 *************************************/
SHVBool SHVControlImplementerContainerWindowWin32::Create(SHVControl* owner, SHVControlImplementer* parent, int flags)
{
	if (!IsCreated() && parent && parent->IsCreated())
	{
		SetHandle(::CreateWindowEx(WS_EX_CONTROLPARENT,SHVWIN32CLASS_CONTAINERWND, _T(""), WS_CHILD|Win32::MapFlags(flags),
			CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, Win32::GetHandle(parent), NULL, Win32::GetInstance(owner), NULL));

		if (IsCreated())
		{
			owner->SetFont(NULL,false);

			SetWindowLongPtr(GetHandle(),0,(LONG_PTR)owner);
		}

		return IsCreated();
	}

	return SHVBool::False;
}

/*************************************
 * Destroy
 *************************************/
SHVBool SHVControlImplementerContainerWindowWin32::Destroy(SHVControl* owner)
{
	if (IsCreated() && GetSubType(owner) == SHVControlContainer::SubTypeTabPage)
	{
	HWND tabHandle = ::GetParent(GetHandle());
	SHVBool retVal(SHVControlImplementerWin32<SHVControlImplementerContainerCustomDraw>::Destroy(owner));
		SHVControlImplementerTabWin32::RemovePageByHandle(tabHandle,(SHVControlContainer*)owner);
		return retVal;
	}
	
	return SHVControlImplementerWin32<SHVControlImplementerContainerCustomDraw>::Destroy(owner);
}

/*************************************
 * GetRegionRect
 *************************************/
SHVRect SHVControlImplementerContainerWindowWin32::GetRegionRect()
{
RECT nativeRect;

	SHVASSERT(IsCreated());

	::GetClientRect(GetHandle(),&nativeRect);

	return SHVRect(nativeRect.left,nativeRect.top,nativeRect.right,nativeRect.bottom);
}

/*************************************
 * SetSize
 *************************************/
void SHVControlImplementerContainerWindowWin32::SetSize(SHVControlContainer* owner, int widthInPixels, int heightInPixels, SHVControlContainer::PosModes mode)
{
	if (SubType != SHVControlContainer::SubTypeTabPage)
	{
	SHVRect parentRect(owner->GetParent()->GetRect());
	SHVRect rect(GetRect(owner));

		SHVASSERT(IsCreated());

		rect.SetWidth(widthInPixels);
		rect.SetHeight(heightInPixels);

		switch (mode)
		{
		case SHVControlContainer::PosCenterScreen: // equals centerwindow on child containers
		case SHVControlContainer::PosCenterWindow:
			rect.SetX( (parentRect.GetWidth()-rect.GetWidth())/2 );
			rect.SetY( (parentRect.GetHeight()-rect.GetHeight())/2 );
			break;
		default:
			break;
		}

		SetRect(owner,rect);
	}
}

/*************************************
 * GetTitle
 *************************************/
SHVStringBuffer SHVControlImplementerContainerWindowWin32::GetTitle(SHVControlContainer* owner)
{
SHVString retVal;

	SHVASSERT(IsCreated());


	if (SubType == SHVControlContainer::SubTypeTabPage)
	{
	SHVControlTab* tab = (SHVControlTab*)GetWindowLongPtr(::GetParent(GetHandle()),GWLP_USERDATA);
	SHVControlImplementerTabWin32* parent = (SHVControlImplementerTabWin32*)tab->GetImplementor();
		retVal = parent->GetPageNameByContainer(tab,owner);
	}
	else
	{
		retVal.SetBufferSize( GetWindowTextLength(GetHandle())+1 );
		GetWindowText(GetHandle(),(TCHAR*)retVal.GetBuffer(), (int)retVal.GetBufferLen());
	}
	return retVal.ReleaseBuffer();
}

/*************************************
 * SetTitle
 *************************************/
void SHVControlImplementerContainerWindowWin32::SetTitle(SHVControlContainer* owner, const SHVStringC& title)
{
	SHVASSERT(IsCreated());

	if (SubType == SHVControlContainer::SubTypeTabPage)
	{
	SHVControlTab* tab = (SHVControlTab*)GetWindowLongPtr(::GetParent(GetHandle()),GWLP_USERDATA);
	SHVControlImplementerTabWin32* parent = (SHVControlImplementerTabWin32*)tab->GetImplementor();
		parent->SetPageNameByContainer(tab,owner,title);
	}
	else
	{
		SetWindowText(GetHandle(),(const TCHAR*)title.GetSafeBuffer());
	}
}

/*************************************
 * GetColor
 *************************************/
SHVColor* SHVControlImplementerContainerWindowWin32::GetColor(SHVControlContainer* owner)
{
	return Color;
}

/*************************************
 * SetColor
 *************************************/
void SHVControlImplementerContainerWindowWin32::SetColor(SHVControlContainer* owner, SHVColor* color)
{
	Color = color;

	if (IsCreated())
		::InvalidateRect(GetHandle(),NULL,TRUE);
}

/*************************************
 * RegisterClass
 *************************************/
void SHVControlImplementerContainerWindowWin32::RegisterClass(SHVGUIManager* manager, HINSTANCE hInstance)
{
ATOM clss;
#ifdef __SHIVA_WINCE
WNDCLASS wc;
#else
WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hIconSm			= 0;
#endif

	wc.style			= 0;
	wc.lpfnWndProc		= (WNDPROC)SHVControlImplementerContainerWindowWin32::WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= sizeof(void*);
	wc.hInstance		= hInstance;
	wc.hIcon			= 0;
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)(COLOR_BACKGROUND);
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= SHVWIN32CLASS_CONTAINERWND;

#ifdef __SHIVA_WINCE
	SHVVERIFY(clss = ::RegisterClass(&wc));
#else
	SHVVERIFY(clss = ::RegisterClassEx(&wc));
#endif
	manager->GetConfig().Set(SHVWIN32ATOM_CONTAINERWND, clss);
}

/*************************************
 * SetResizable
 *************************************/
void SHVControlImplementerContainerWindowWin32::SetResizable(bool resizable)
{
}

/*************************************
 * SetMinimumSize
 *************************************/
void SHVControlImplementerContainerWindowWin32::SetMinimumSize(SHVControlContainer* owner, int widthInPixels, int heightInPixels)
{
	SHVASSERT(SubType != SHVControlContainer::SubTypeTabPage);
	MinSize.x = widthInPixels;
	MinSize.y = heightInPixels;
}

/*************************************
 * GetMinimumSizeInPixels
 *************************************/
SHVPoint SHVControlImplementerContainerWindowWin32::GetMinimumSizeInPixels(SHVControlContainer* owner)
{
	return MinSize;
}

/*************************************
 * SubscribeDraw
 *************************************/
void SHVControlImplementerContainerWindowWin32::SubscribeDraw(SHVEventSubscriberBase* subscriber)
{
	SHVASSERT(SubType != SHVControlContainer::SubTypeTabPage);
	Subscriber = subscriber;
}

///\cond INTERNAL
/*************************************
 * WndProc
 *************************************/
LRESULT CALLBACK SHVControlImplementerContainerWindowWin32::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
SHVControlContainer* owner = (SHVControlContainer*)GetWindowLongPtr(hWnd,0);
SHVControlImplementerContainerWindowWin32* self = (owner ? (SHVControlImplementerContainerWindowWin32*)owner->GetImplementor() : NULL);
SHVControlContainerRef refToSelf;
LRESULT result = 0;

	switch (message) 
	{

	// message types that needs to be forwarded to the child control it came from
	case WM_NOTIFY:
		{
		LPNMHDR nhdr = (LPNMHDR)lParam;
			if (nhdr && nhdr->hwndFrom != hWnd)
				result = ::SendMessage(nhdr->hwndFrom,WM_SHV_NOTIFY,wParam,lParam);
		}
		break;
	case WM_DRAWITEM:
		{
		LPDRAWITEMSTRUCT drawstr = (LPDRAWITEMSTRUCT)lParam;
			if (drawstr && drawstr->hwndItem != hWnd)
				result = ::SendMessage(drawstr->hwndItem,WM_SHV_DRAWITEM,wParam,lParam);
		}
		break;

	// normal message types
	case WM_PAINT:
		{
		PAINTSTRUCT ps;
		HDC hdc;
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_ERASEBKGND:
		{
		bool drawn = false;
			if (owner)
			{
			SHVDrawWin32Ref draw = Win32::CreateDraw(owner,(HDC)wParam);
			SHVColor* color = owner->GetColor();

				if (color)
				{
					drawn = true;
					draw->DrawRectFilled(draw->GetClientRect(owner),color);
				}
		
				if (!self->Subscriber.IsNull())
				{
					drawn = true;
					refToSelf = owner; // ensure the validity of the object through this function
					self->Subscriber->EmitNow(owner->GetModuleList(),new SHVEventData<SHVDrawEventData>(SHVDrawEventData(draw),NULL,SHVControl::EventDraw,NULL,owner));
				}

			}
			
			if (drawn)
				result = 1;
			else
				result = DefWindowProc(hWnd, message, wParam, lParam);
			break;
		}
	case WM_DESTROY:
		SHVASSERT(!self->IsCreated());
		owner->Clear();
		self->EmitControlEvent(owner,SHVGUIManager::CtrlEventContainerDestroy);
		break;
	case WM_SIZE:
		///\todo Add minimum resize code for platforms that do not support WM_SIZING
		if (owner && owner->GetModuleList().IsRegistered())
		{
			owner->ResizeControls();
		}
		break;
	default:
		result = DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	return result;
}
///\endcond
