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
#include "shvmainthreadeventdispatcherwin32.h"
#include "shvwin32.h"
#include "utils/shvdrawwin32.h"


#define SHVWIN32CLASS_CONTAINERWND _T("SHV_CntWnd")
#define SHVWIN32ATOM_CONTAINERWND _T("SHV_ContainerWndAtom")

//=========================================================================================================
// SHVControlImplementerContainerWindowWin32
//=========================================================================================================


/*************************************
 * Constructor
 *************************************/
SHVControlImplementerContainerWindowWin32::SHVControlImplementerContainerWindowWin32(int subType) : SHVControlImplementerWin32<SHVControlImplementerContainerCustomDraw>()
{
	SubType = subType;
	MinWidthInChars = MinHeightInChars = 0;
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
		SetHandle(::CreateWindow(SHVWIN32CLASS_CONTAINERWND, _T(""), WS_CHILD|Win32::MapFlags(flags),
			CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, Win32::GetHandle(parent), NULL, Win32::GetInstance(owner), NULL));

		if (IsCreated())
		{
			SetWindowLongPtr(GetHandle(),0,(LONG_PTR)owner);
			MinSize = CalculateMinSize(owner,MinWidthInChars,MinHeightInChars);
		}

		return IsCreated();
	}

	return SHVBool::False;
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
 * GetTitle
 *************************************/
SHVStringBuffer SHVControlImplementerContainerWindowWin32::GetTitle()
{
SHVString retVal;

	SHVASSERT(IsCreated());

	retVal.SetBufferSize( GetWindowTextLength(GetHandle())+1 );
	GetWindowText(GetHandle(),(TCHAR*)retVal.GetBuffer(), (int)retVal.GetBufferLen());

	return retVal.ReleaseBuffer();
}

/*************************************
 * SetTitle
 *************************************/
void SHVControlImplementerContainerWindowWin32::SetTitle(const SHVStringC& title)
{
	SHVASSERT(IsCreated());

	SetWindowText(GetHandle(),title.GetSafeBuffer());
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
void SHVControlImplementerContainerWindowWin32::SetMinimumSize(SHVControlContainer* owner, int widthInChars, int heightInChars)
{
	MinWidthInChars = widthInChars;
	MinHeightInChars = heightInChars;

	if (IsCreated())
		MinSize = CalculateMinSize(owner,MinWidthInChars,MinHeightInChars);
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
LRESULT res = 0;

	switch (message) 
	{
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
					self->Subscriber->EmitNow(owner->GetModuleList(),new SHVEventData<SHVDrawRef>((SHVDraw*)draw,NULL,SHVControl::EventDraw,NULL,owner));
				}

			}
			
			if (drawn)
				return 1;
			else
				return DefWindowProc(hWnd, message, wParam, lParam);
		}
	case WM_DESTROY:
		SHVASSERT(!self->IsCreated());
		owner->Clear();
		break;
	case WM_SIZE:
		///\todo Add minimum resize code for platforms that do not support WM_SIZING
		if (owner && owner->GetModuleList().IsRegistered())
		{
			owner->ResizeControls();
		}
		break;
	default:
		res = DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	return res;
}
///\endcond
