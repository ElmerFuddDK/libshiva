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
#include "utils/shvdrawwin32.h"
#include "shvmenuwin32.h"

#include <commctrl.h>
#ifdef __SHIVA_POCKETPC
# include <sipapi.h>
# define MENU_HEIGHT 26
#endif


#define SHVWIN32CLASS_MAINWND L"SHV_MainWnd"

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

	FocusWnd = NULL;
	IgnoreResize = false;

	Color = SHVColorWin32::FromSysColor(COLOR_3DFACE);
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
SHVBool SHVControlImplementerMainWindowWin32::Create(SHVControl* owner, SHVControlImplementer* parent, int flags)
{
SHVBool retVal(parent == NULL && !IsCreated());

	SHVASSERT(owner && owner->GetImplementor() == this);

	if (retVal)
	{
#ifdef __SHIVA_WINCE
	HWND hWnd = FindWindowW(SHVWIN32CLASS_MAINWND,NULL);
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
	DWORD styles = WS_VISIBLE|Win32::MapFlags(flags);
	WCHAR* title = L"SHIVA Application";
# ifdef __SHIVA_POCKETPC
		memset (&s_sai, 0, sizeof(s_sai));
		s_sai.cbSize = sizeof(s_sai);
# endif
#else
	DWORD styles = WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|Win32::MapFlags(flags);
	const WCHAR* title = L"";
#endif
		SetHandle(::CreateWindowExW(WS_EX_CONTROLPARENT,SHVWIN32CLASS_MAINWND, title, styles,
			CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL));

		retVal = IsCreated();

		if (retVal)
		{
		RECT winRect, clientRect;
#ifdef __SHIVA_POCKETPC
			CmdWnd = new SHVMenuCommandBarPocketPC(GetHandle(),hInstance,(SHVGUIManagerWin32*)owner->GetManager());

			///\todo Add a better way to determine the correct size of the initial window
			if (CmdWnd->hCmdWnd)
			{
			RECT menubarRect;
				GetWindowRect(GetHandle(), &winRect);
				GetWindowRect(CmdWnd->hCmdWnd, &menubarRect);
				winRect.bottom -= 2*(menubarRect.bottom - menubarRect.top);
				MoveWindow(GetHandle(), winRect.left, winRect.top, winRect.right, winRect.bottom, FALSE);
			}
			else
			{
				CmdWnd = NULL;
			}
#endif

			SetWindowLongPtr(GetHandle(),0,(LONG_PTR)owner);

			::GetClientRect(GetHandle(),&clientRect);
			::GetWindowRect(GetHandle(),&winRect);

			DecorationsSize.x = (winRect.right - winRect.left) - (clientRect.right - clientRect.left);
			DecorationsSize.y = (winRect.bottom - winRect.top) - (clientRect.bottom - clientRect.top);

		}
	}

	return retVal;
}

/*************************************
 * SetRect
 *************************************/
void SHVControlImplementerMainWindowWin32::SetRect(SHVControl* owner, const SHVRect& rect)
{
	IgnoreResize = true;
	SHVControlImplementerWin32<SHVControlImplementerContainer>::SetRect(owner, rect);
	IgnoreResize = false;
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
 * SetSize
 *************************************/
void SHVControlImplementerMainWindowWin32::SetSize(SHVControlContainer* owner, int widthInPixels, int heightInPixels, SHVControlContainer::PosModes mode)
{
#ifndef __SHIVA_POCKETPC
SHVRect rect(GetRect(owner));

	SHVASSERT(IsCreated());

	rect.SetWidth(widthInPixels+DecorationsSize.x);
	rect.SetHeight(heightInPixels+DecorationsSize.y);

	switch (mode)
	{
	case SHVControlContainer::PosCenterWindow: // centerwindow is the same as screen for main window
	case SHVControlContainer::PosCenterScreen:
		///\todo implement CenterScreen for main window
		break;
	default:
		break;
	}

	SetRect(owner,rect);
#endif
}

/*************************************
 * CreateMenu
 *************************************/
SHVMenu* SHVControlImplementerMainWindowWin32::CreateMenu(SHVControlContainer* owner, SHVEventSubscriberBase* subscriber)
{
#ifdef __SHIVA_POCKETPC
SHVMenuContainerPocketPC* retVal = NULL;
	if (!CmdWnd.IsNull())
	{
		retVal = new SHVMenuContainerPocketPC((SHVGUIManagerWin32*)owner->GetManager(),subscriber,owner);
		CmdWnd->InitializeMenu(retVal);
	}
	return retVal;
#else
	return new SHVMenuWin32((SHVGUIManagerWin32*)owner->GetManager(),SHVMenu::TypeControlContainer,subscriber,owner);
#endif
}

/*************************************
 * GetTitle
 *************************************/
SHVStringBuffer SHVControlImplementerMainWindowWin32::GetTitle(SHVControlContainer* owner)
{
	SHVASSERT(IsCreated());
	return GetWindowTextBase();
}

/*************************************
 * SetTitle
 *************************************/
void SHVControlImplementerMainWindowWin32::SetTitle(SHVControlContainer* owner, const SHVStringC& title)
{
	SHVASSERT(IsCreated());
	SetWindowTextBase(title);
}

/*************************************
 * GetColor
 *************************************/
SHVColor* SHVControlImplementerMainWindowWin32::GetColor(SHVControlContainer* owner)
{
	return Color;
}

/*************************************
 * SetColor
 *************************************/
void SHVControlImplementerMainWindowWin32::SetColor(SHVControlContainer* owner, SHVColor* color)
{
	Color = color;

	if (IsCreated())
		::InvalidateRect(GetHandle(),NULL,TRUE);
}

/*************************************
 * SetMinimumSize
 *************************************/
void SHVControlImplementerMainWindowWin32::SetMinimumSize(SHVControlContainer* owner, int widthInPixels, int heightInPixels)
{
	MinSize.x = widthInPixels;
	MinSize.y = heightInPixels;
}

/*************************************
 * GetMinimumSizeInPixels
 *************************************/
SHVPoint SHVControlImplementerMainWindowWin32::GetMinimumSizeInPixels(SHVControlContainer* owner)
{
	return MinSize;
}

/*************************************
 * RegisterClass
 *************************************/
void SHVControlImplementerMainWindowWin32::RegisterClass(HINSTANCE hInstance)
{
INITCOMMONCONTROLSEX cctrlex;
#ifdef __SHIVA_WINCE
WNDCLASSW wc;
#else
WNDCLASSEXW wc;
	wc.cbSize = sizeof(WNDCLASSEXW);
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
	wc.hIcon			= ::LoadIcon(hInstance,MAKEINTRESOURCE(128)); // IDR_MAINFRAME
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)(COLOR_BACKGROUND);
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= SHVWIN32CLASS_MAINWND;

#ifdef __SHIVA_WINCE
	::RegisterClassW(&wc);
#else
	::RegisterClassExW(&wc);
#endif
}

/*************************************
 * SetResizable
 *************************************/
void SHVControlImplementerMainWindowWin32::SetResizable(bool resizable)
{
	SHVControlImplementerWin32Base::SetResizable(resizable);
}

///\cond INTERNAL
/*************************************
 * WndProc
 *************************************/
LRESULT CALLBACK SHVControlImplementerMainWindowWin32::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
SHVControlContainer* owner = (SHVControlContainer*)GetWindowLongPtr(hWnd,0);
SHVControlImplementerMainWindowWin32* self = (owner ? (SHVControlImplementerMainWindowWin32*)owner->GetImplementor() : NULL);
LRESULT retVal = 0;
SHVControlContainerRef refToSelf;

	switch (message) 
	{

	// message types that needs to be forwarded to the child control it came from
	case WM_NOTIFY:
		{
		LPNMHDR nhdr = (LPNMHDR)lParam;
			if (nhdr && nhdr->hwndFrom != hWnd)
				retVal = ::SendMessage(nhdr->hwndFrom,WM_SHV_NOTIFY,wParam,lParam);
		}
		break;
	case WM_DRAWITEM:
		{
		LPDRAWITEMSTRUCT drawstr = (LPDRAWITEMSTRUCT)lParam;
			if (drawstr && drawstr->hwndItem != hWnd)
				retVal = ::SendMessage(drawstr->hwndItem,WM_SHV_DRAWITEM,wParam,lParam);
		}
		break;

	case WM_SETCURSOR:
		if (owner && ((SHVGUIManagerWin32*)owner->GetManager())->WaitCursorMode())
		{
			::SetCursor(::LoadCursor(NULL,IDC_WAIT));
			retVal = TRUE;
		}
		else
		{
			retVal = DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	// normal message types
	case WM_SHV_DISPATCHMESSAGES:
		self->Dispatcher->DispatchEvents();
		break;
	case WM_CLOSE:
		self->Dispatcher->CloseApp();
		break;
	case WM_ERASEBKGND:
		{
		bool drawn = false;

			if (owner)
			{
			SHVDrawWin32Ref draw = Win32::CreateDraw(owner,(HDC)wParam);

				if (!self->Color.IsNull())
				{
					drawn = true;
					draw->DrawRectFilled(draw->GetClientRect(owner),self->Color);
				}
			}
			
			if (drawn)
				retVal = 1;
			else
				retVal = DefWindowProc(hWnd, message, wParam, lParam);
		}
	case WM_PAINT:
		{
		PAINTSTRUCT ps;
		HDC hdc;
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_ACTIVATE:
#ifdef __SHIVA_POCKETPC
		// Notify shell of our activate message
		if (self)
			SHHandleWMActivate(hWnd, wParam, lParam, &self->s_sai, FALSE);
#else
		if (self)
		{
			switch (LOWORD(wParam))
			{
			case WA_CLICKACTIVE:
			case WA_ACTIVE:
				if (!self->FocusWnd || !::IsWindow(self->FocusWnd) || !::IsChild(hWnd,self->FocusWnd) || !(::GetWindowLong(self->FocusWnd,GWL_STYLE)&WS_TABSTOP))
					self->FocusWnd = ::GetNextDlgTabItem(hWnd,hWnd,FALSE);
				if (self->FocusWnd)
					::SetFocus(self->FocusWnd);
				break;
			case WA_INACTIVE:
				if (::GetFocus())
					self->FocusWnd = ::GetFocus();
				break;
			}
		}
#endif
     	break;
	case WM_SETFOCUS:
		if (self)
		{
			if (!self->FocusWnd || !::IsWindow(self->FocusWnd) || !::IsChild(hWnd,self->FocusWnd) || !(::GetWindowLong(self->FocusWnd,GWL_STYLE)&WS_TABSTOP))
				self->FocusWnd = ::GetNextDlgTabItem(hWnd,hWnd,FALSE);
			if (self->FocusWnd)
				::SetFocus(self->FocusWnd);
		}
		break;
	case WM_SETTINGCHANGE:
#ifdef __SHIVA_POCKETPC
		if (self)
			SHHandleWMSettingChange(hWnd, wParam, lParam, &self->s_sai);
#endif
		break;
	case WM_DESTROY:
#ifdef __SHIVA_POCKETPC
		self->CmdWnd = NULL;
#endif
		SHVASSERT(!self->IsCreated());
		owner->Clear();
		PostQuitMessage(0);
		break;
	case WM_COMMAND:
#ifdef __SHIVA_POCKETPC
		if (self && !self->CmdWnd.IsNull())
			self->CmdWnd->OnCommandMsg(hWnd,wParam,lParam);
#endif
		if (HIWORD(wParam) == EN_CHANGE)
		{
			::SendMessage((HWND)lParam,WM_COMMAND,wParam,lParam);
		}
		retVal = DefWindowProc(hWnd, message, wParam, lParam);
		break;
#ifdef WM_SIZING
	case WM_SIZING:
		retVal = DefWindowProc(hWnd, message, wParam, lParam);
		if ( self && ( self->MinSize.x || self->MinSize.y ) )
		{
		LPRECT rect = (LPRECT)lParam;
		SHVPoint minSize( self->MinSize.x + self->DecorationsSize.x, self->MinSize.y + self->DecorationsSize.y );

			// Check for min width
			if ( (rect->right - rect->left) < minSize.x )
			{
				switch (wParam)
				{
				case WMSZ_RIGHT:
				case WMSZ_BOTTOMRIGHT:
				case WMSZ_TOPRIGHT:
					rect->right = rect->left + minSize.x;
					break;
				case WMSZ_LEFT:
				case WMSZ_BOTTOMLEFT:
				case WMSZ_TOPLEFT:
					rect->left = rect->right - minSize.x;
					break;
				}
			}

			// Check for min height
			if ( (rect->bottom - rect->top) < minSize.y )
			{
				switch (wParam)
				{
				case WMSZ_BOTTOM:
				case WMSZ_BOTTOMLEFT:
				case WMSZ_BOTTOMRIGHT:
					rect->bottom = rect->top + minSize.y;
					break;
				case WMSZ_TOP:
				case WMSZ_TOPLEFT:
				case WMSZ_TOPRIGHT:
					rect->top = rect->bottom - minSize.y;
					break;
				}
			}

			retVal = TRUE;
		}
#endif
	case WM_SIZE:
		///\todo Add a mechanism for handling minsize when WM_SIZING is not supported (WinCE)
		if (owner && owner->GetModuleList().IsRegistered())
		{
			owner->ResizeControls();
		}
		break;
	case WM_WINDOWPOSCHANGED:
		if (owner && owner->GetModuleList().IsRegistered())
		{
		WINDOWPOS* wpos = (WINDOWPOS*)lParam;
			if (!self->IgnoreResize && !(wpos->flags&SWP_FRAMECHANGED) && (!(wpos->flags&SWP_NOMOVE) || !(wpos->flags&SWP_NOSIZE)))
				self->EmitControlEvent(owner,SHVGUIManager::CtrlEventContainerRectChanged);
		}
		retVal = DefWindowProc(hWnd, message, wParam, lParam);
		break;
	default:
		retVal = DefWindowProc(hWnd, message, wParam, lParam);
	}
	return retVal;
}
///\endcond
