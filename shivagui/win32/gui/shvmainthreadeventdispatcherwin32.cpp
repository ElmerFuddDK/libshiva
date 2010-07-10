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
#include "shvmainthreadeventdispatcherwin32.h"
#include "shvwin32.h"
#include "shvcontrolimplementermainwindowwin32.h"
#include "../../../include/utils/shvdir.h"

#ifndef __SHIVA_WINCE
# include <locale.h>
#endif



//-=========================================================================================================
// SHVMainWndWin32 - Main window implementation
//-=========================================================================================================


/*************************************
 * Constructor
 *************************************/
SHVMainThreadEventDispatcherWin32::SHVMainThreadEventDispatcherWin32()
{
	GUIManager = NULL;
	ClosePending = false;
}

/*************************************
 * Destructor
 *************************************/
SHVMainThreadEventDispatcherWin32::~SHVMainThreadEventDispatcherWin32()
{
}

/*************************************
 * SetupDefaults
 *************************************/
void SHVMainThreadEventDispatcherWin32::SetupDefaults(SHVModuleList& modules)
{
	// Set default instance handle
	modules.GetConfig(SHVModuleList::CfgGUI).SetPtr(SHVGUIManager::CfgInstanceHandle,::GetModuleHandle(NULL));

#ifndef __SHIVA_WINCE
	// set up POSIX locale according to win32 locale
	setlocale(LC_ALL, "");
#endif
}

/*************************************
 * SignalDispatcher
 *************************************/
void SHVMainThreadEventDispatcherWin32::SignalDispatcher()
{
	::PostMessage(Win32::GetHandle(MainWindow),WM_SHV_DISPATCHMESSAGES,0,0);
}

/*************************************
 * InitializeEventLoop
 *************************************/
SHVBool SHVMainThreadEventDispatcherWin32::InitializeEventLoop()
{
SHVBool retVal;
HINSTANCE hInstance = (HINSTANCE)Queue->GetModuleList().GetConfig(SHVModuleList::CfgGUI).FindPtr(SHVGUIManager::CfgInstanceHandle)->ToPtr();

	// register main window class
	SHVControlImplementerMainWindowWin32::RegisterClass(hInstance);

	GUIManager = new SHVGUIManagerWin32(Queue->GetModuleList());
	MainWindow = new SHVControlContainer(GUIManager,new SHVControlImplementerMainWindowWin32(hInstance,this));

	GUIManager->SetMainWindow(MainWindow);
	MainWindow->SetFont(NULL,false);

	retVal = Queue->GetModuleList().AddModule(GUIManager);

	if (retVal)
	{
		retVal = MainWindow->Create();
		MainWindow->SetFont(NULL,false);
	}

	return retVal;
}

/*************************************
 * RunEventLoop
 *************************************/
void SHVMainThreadEventDispatcherWin32::RunEventLoop()
{
MSG msg;
HWND wnd, topwnd;
bool processed;
BOOL msgRetVal;

	while (Running()) 
	{
		msgRetVal = GetMessage(&msg, NULL, 0, 0);

		if (msgRetVal == -1)
		{
			continue;
		}
		else if (msgRetVal == 0)
		{
			MainWindow->Close();
			continue;
		}

		if (!GUIManager->PreTranslateMessage(&msg))
		{
			processed = false;

			// check for tab stop
			switch (msg.message)
			{
			case WM_KEYDOWN:
				
				if (msg.wParam == VK_TAB && !(GetKeyState(VK_CONTROL) & 0x8000))
				{

					// find top level wnd
					for (wnd=msg.hwnd;wnd && ::GetWindowLong(wnd,GWL_STYLE);wnd = ::GetParent(wnd))
						topwnd = wnd;

					wnd = ::GetNextDlgTabItem(topwnd,msg.hwnd,(GetKeyState(VK_SHIFT) & 0x8000 ? TRUE : FALSE));
					if (wnd)
					{
						processed = true;
						::SetFocus(wnd);
					}
				}
				break;
			}


			if (!processed)
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

	// Noone should have references to the window at this point besides us
	GUIManager->SetMainWindow(NULL);
	SHVASSERT(MainWindow->GetRefCount() == 1);

	MainWindow = NULL;
	GUIManager = NULL;
}

/*************************************
 * StopEventLoop
 *************************************/
void SHVMainThreadEventDispatcherWin32::StopEventLoop(SHVBool errors)
{
	if (!errors) // errors during startup
	{
	SHVString errStr = Queue->GetModuleList().GetStartupErrors();
		MessageBox(NULL,(const TCHAR*)errStr.GetSafeBuffer(),_T("Registering failed:"),MB_ICONERROR);
	}
		
	::PostMessage(Win32::GetHandle(MainWindow),WM_DESTROY,0,0);
}

/*************************************
 * OnEvent
 *************************************/
void SHVMainThreadEventDispatcherWin32::OnEvent(SHVEvent* event)
{
}

///\cond INTERNAL
/*************************************
 * DispatchEvents
 *************************************/
void SHVMainThreadEventDispatcherWin32::DispatchEvents()
{
	SHVMainThreadEventDispatcher::DispatchEvents();
}

/*************************************
 * CloseApp
 *************************************/
void SHVMainThreadEventDispatcherWin32::CloseApp()
{
	if (!ClosePending && !Queue->GetModuleList().IsUnregistering() && Queue->GetModuleList().IsRegistered())
	{
		ClosePending = true;
		Queue->GetModuleList().CloseApp();
	}
}
///\endcond
