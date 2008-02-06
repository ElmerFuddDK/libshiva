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

#include "shvcontrolimplementermainwindowwin32.h"
#include "shvmainthreadeventdispatcherwin32.h"
#include "shvwin32.h"

#include <commctrl.h>


#define SHVWIN32CLASS_MAINWND _T("SHV_MainWnd")

//=========================================================================================================
// SHVControlImplementerMainWindowWin32 - Main window implementation
//=========================================================================================================


/*************************************
 * Constructor
 *************************************/
SHVControlImplementerMainWindowWin32::SHVControlImplementerMainWindowWin32(HINSTANCE hinstance, SHVMainThreadEventDispatcherWin32* dispatcher) : SHVControlImplementerWin32<SHVControlImplementerContainer>()
{
	hInstance = hinstance;
	Dispatcher = dispatcher;
}

/*************************************
 * GetSubType
 *************************************/
int SHVControlImplementerMainWindowWin32::GetSubType(SHVControl* owner)
{
	return SHVControlContainer::SubTypeMainWindow;
}

/*************************************
 * Create(parent)
 *************************************/
SHVBool SHVControlImplementerMainWindowWin32::Create(SHVControl* owner, SHVControlImplementer* parent)
{
SHVBool retVal(parent == NULL && !IsCreated());

	SHVASSERT(owner && owner->GetImplementor() == this);

	if (retVal)
	{
#ifdef __SHIVA_WINCE
	HWND hWnd = FindWindow(SHVWIN32CLASS_MAINWND,NULL);
		///\todo add code to make sure the window is created with the same application as us
		if (hWnd) 
		{
			// set focus to foremost child window
			// The "| 0x01" is used to bring any owned windows to the foreground and
			// activate them.
			SetForegroundWindow((HWND)((ULONG) hWnd | 0x00000001));
			return SHVBool::False;
		}
#endif

#ifdef __SHIVA_WINCE
	DWORD styles = WS_VISIBLE;
#else
	DWORD styles = WS_OVERLAPPEDWINDOW;
#endif
		SetHandle(::CreateWindow(SHVWIN32CLASS_MAINWND, _T(""), styles,
			CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL));

		retVal = IsCreated();

		if (retVal)
		{
			SetWindowLongPtr(GetHandle(),0,(LONG_PTR)owner);
		}
	}

	return retVal;
}

/*************************************
 * GetRegionRect
 *************************************/
SHVRect SHVControlImplementerMainWindowWin32::GetRegionRect()
{
RECT nativeRect;

	SHVASSERT(IsCreated());

	::GetClientRect(GetHandle(),&nativeRect);

	return SHVRect(nativeRect.left,nativeRect.top,nativeRect.right,nativeRect.bottom);
}

/*************************************
 * GetTitle
 *************************************/
SHVStringBuffer SHVControlImplementerMainWindowWin32::GetTitle()
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
void SHVControlImplementerMainWindowWin32::SetTitle(const SHVStringC& title)
{
	SHVASSERT(IsCreated());

	SetWindowText(GetHandle(),title.GetSafeBuffer());
}

/*************************************
 * RegisterClass
 *************************************/
void SHVControlImplementerMainWindowWin32::RegisterClass(HINSTANCE hInstance)
{
INITCOMMONCONTROLSEX cctrlex;
#ifdef __SHIVA_WINCE
WNDCLASS wc;
#else
WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hIconSm			= 0;
#endif

	cctrlex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	cctrlex.dwICC = ICC_WIN95_CLASSES; // see http://msdn2.microsoft.com/en-us/library/bb775507.aspx for more info

	InitCommonControlsEx(&cctrlex);

	wc.style			= 0;//CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= (WNDPROC)SHVControlImplementerMainWindowWin32::WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= sizeof(void*);
	wc.hInstance		= hInstance;
	wc.hIcon			= 0;
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)(COLOR_BACKGROUND);
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= SHVWIN32CLASS_MAINWND;

#ifdef __SHIVA_WINCE
	::RegisterClass(&wc);
#else
	::RegisterClassEx(&wc);
#endif
}

///\cond INTERNAL
/*************************************
 * WndProc
 *************************************/
LRESULT CALLBACK SHVControlImplementerMainWindowWin32::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
SHVControlContainer* owner = (SHVControlContainer*)GetWindowLongPtr(hWnd,0);
SHVControlImplementerMainWindowWin32* self = (owner ? (SHVControlImplementerMainWindowWin32*)owner->GetImplementor() : NULL);

	switch (message) 
	{
	case WM_SHV_DISPATCHMESSAGES:
		self->Dispatcher->DispatchEvents();
		break;
	case WM_CLOSE:
		self->Dispatcher->CloseApp();
		break;
	case WM_PAINT:
		{
		PAINTSTRUCT ps;
		HDC hdc;
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SIZE:
		if (owner && owner->GetModuleList().IsRegistered())
		{
			owner->ResizeControls();
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
///\endcond
