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
#include "../../../include/threadutils/shvthreadbase.h"
#include "../../../include/utils/shvdir.h"
#include "../../../include/utils/shvbuffer.h"

#include "../../../include/frameworkimpl/shvmainthreadeventdispatcherconsole.h"
#include "../../../include/framework/shveventstdin.h"

#if defined(__SHIVA_WINCE)
# include <aygshell.h>
# include "../../../include/threadutils/shvmutexlocker.h"
# define WM_SHV_DISPATCHMESSAGES    0x8001
# define WM_SHV_PRINT               0x8002
static SHVMainThreadEventDispatcherConsole* evDispatcherConsole;
#elif defined(__SHIVA_WIN32)
# include <io.h>
# include <fcntl.h>
# include "../../../include/utils/shvdllbase.h"
# include "../../../include/utils/shvstringconv.h"
# define EventInternalSdin -1
#else
# include <unistd.h>
# include <fcntl.h>
# include <errno.h>
#endif
#include <string.h>


//=========================================================================================================
// SHVMainThreadEventDispatcherConsole class - Dispatcher for console user IO
//=========================================================================================================
/// \class SHVMainThreadEventDispatcherConsole shvmainthreadeventdispatcherconsole.h "shiva/include/frameworkimpl/shvmainthreadeventdispatcherconsole.h"

/*************************************
 * Constructor
 *************************************/
SHVMainThreadEventDispatcherConsole::SHVMainThreadEventDispatcherConsole()
{
#if defined(__SHIVA_WINCE)
	wndConsole = NULL;
	edtProc = NULL;
	edtConsole = NULL;
	Font = NULL;
	boundaries.cx = boundaries.cy = 0;
	if (!evDispatcherConsole)
	{
		evDispatcherConsole = this;
		if (!CreateDlg())
			evDispatcherConsole = NULL;
	}
#elif defined(__SHIVA_WIN32)
TCHAR name[MAX_PATH];

	Initializing = false;
	QuirksMode = false;

	if (!::GetConsoleTitle(name,MAX_PATH))
	{
	BOOL (WINAPI *attachConsole)(DWORD dwProcessId) = NULL;
	SHVDllBase kernel32;

		///\todo Figure out how to attach to a console on win32 from a gui app when using console dispatcher
		//if (!kernel32.Load(_S("kernel32")) ||!kernel32.Resolve((void**)&attachConsole,_S("AttachConsole")))
		//	attachConsole = NULL;
		SHVUNUSED_PARAM(attachConsole); // for future use
		QuirksMode = true;

		if (!attachConsole || !(*attachConsole)(-1))
		{
			::AllocConsole();
		}
		// else attached to an existing console

		// redirect stdin/out/err
		freopen("CON", "w", stdout);
		freopen("CON", "w", stderr);
		freopen("CONIN$", "r", stdin);
	}

	::DuplicateHandle(GetCurrentProcess(),GetStdHandle(STD_INPUT_HANDLE),GetCurrentProcess(),&StdinHandle,0,FALSE,DUPLICATE_SAME_ACCESS);

#else
	// Initialize pipe signaller for the select statement
	pipe(PipeSignal);

	// disable blocking mode on the pipes - close on failure
	if (fcntl(PipeSignal[0], F_SETFL, O_NONBLOCK) < 0)
	{
		close(PipeSignal[0]);
		PipeSignal[0] = 0;
	}
	if (fcntl(PipeSignal[1], F_SETFL, O_NONBLOCK) < 0)
	{
		close(PipeSignal[1]);
		PipeSignal[1] = 0;
	}

	QueueSignalled = false;
#endif
}

/*************************************
 * Destructor
 *************************************/
SHVMainThreadEventDispatcherConsole::~SHVMainThreadEventDispatcherConsole()
{
#if defined(__SHIVA_WINCE)
	if (evDispatcherConsole == this)
		evDispatcherConsole = NULL;
#elif defined(__SHIVA_WIN32)
	if (Initializing)
	{
		if (QuirksMode)
			::FreeConsole();
		else
			::CloseHandle(StdinHandle);
		for (int i=10;StdinThread.IsRunning();i+=10)
			SHVThreadBase::Sleep(i);
	}
	SHVASSERT(!StdinThread.IsRunning());
#else
	if (PipeSignal[0])
		close(PipeSignal[0]);
	if (PipeSignal[1])
		close(PipeSignal[1]);
#endif
}

/*************************************
 * SetupDefaults
 *************************************/
void SHVMainThreadEventDispatcherConsole::SetupDefaults(SHVModuleList& modules)
{
#if defined(__SHIVA_WINCE)
	ModuleList = &modules;
#elif defined(__SHIVA_WIN32)
	selfSubs = new SHVEventSubscriber(this,&modules);
	ModuleList = &modules;
#else
	SHVUNUSED_PARAM(modules);
#endif
}

/*************************************
 * SignalDispatcher
 *************************************/
void SHVMainThreadEventDispatcherConsole::SignalDispatcher()
{
#if defined(__SHIVA_WINCE)
	::PostMessage(wndConsole,WM_SHV_DISPATCHMESSAGES,0,0);
#elif defined(__SHIVA_WIN32)
	Signal.Unlock();
#else
	Mutex.Lock();
	if (Running() && !QueueSignalled)
	{
		QueueSignalled = true;
		write(PipeSignal[1],"1",1);
	}
	Mutex.Unlock();
#endif
}

/*************************************
 * InitializeEventLoop
 *************************************/
SHVBool SHVMainThreadEventDispatcherConsole::InitializeEventLoop()
{
#if defined(__SHIVA_WINCE)
	return (evDispatcherConsole == this);
#elif defined(__SHIVA_WIN32)
	Initializing = true;
	Signal.Lock();
	StdinThread.Start(this,&SHVMainThreadEventDispatcherConsole::StdinFunc);
	return StdinThread.IsRunning();
#else
	StdinPos = 0;
	return (PipeSignal[0] && PipeSignal[1] ? SHVBool::True : SHVBool::False);
#endif
}

/*************************************
 * RunEventLoop
 *************************************/
void SHVMainThreadEventDispatcherConsole::RunEventLoop()
{
#ifndef __SHIVA_WIN32
fd_set rfds;
int nextFD, retVal;
char dummyBuffer[50];
SHVBufferRef readBuf;
size_t bufReadPos = 0;
#elif defined(__SHIVA_WINCE)
BOOL msgRetVal;
MSG msg;
#endif

	DispatchEvents();

#if defined(__SHIVA_WIN32) && !defined(__SHIVA_WINCE)
	Initializing = false;
#endif

	while ( Running() )
	{
#if defined(__SHIVA_WINCE)

		msgRetVal = GetMessage(&msg, NULL, 0, 0);

		if (msgRetVal == -1)
		{
			continue;
		}
		else if (msgRetVal == 0)
		{
			DestroyWindow(wndConsole);
			wndConsole = NULL;
			continue;
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
#elif defined(__SHIVA_WIN32)
		Signal.Lock();
		DispatchEvents();
#else
		// fill file decriptor list for select
		FD_ZERO(&rfds);
		FD_SET(PipeSignal[0], &rfds);
		nextFD = PipeSignal[0]+1;
		FD_SET(STDIN_FILENO,&rfds);
		
		if (STDIN_FILENO > nextFD)
			nextFD = STDIN_FILENO+1;
	
		retVal = select(nextFD, &rfds, NULL, NULL, NULL);
		
		if (!retVal || !Running()) // nothing happened, or terminating
		{
			continue;
		}
		else if (retVal == -1) // ERROR!
		{
			if (errno == EINTR) // we got a signal
				continue;
			printf("Error in main event loop\n");
			_exit(-1);
		}
		
		// clear the signal pipe if present
		if (FD_ISSET(PipeSignal[0],&rfds))
		{
			// If this read is blocking we will be in trouble ... oh well
			Mutex.Lock();
			QueueSignalled = false;
			read(PipeSignal[0], dummyBuffer, 50);
			Mutex.Unlock();
			
			DispatchEvents();
		}
		
		// Check for stdin
		if (FD_ISSET(STDIN_FILENO,&rfds))
		{
			if (bufReadPos == StdinBufSize || readBuf.IsNull())
			{
				readBuf = new SHVBufferChunk<StdinBufSize>();
				::memset(readBuf->GetBuffer(),0,StdinBufSize);
				bufReadPos = 0;
				
				StdinStream.AddBuffer(readBuf);
			}

			retVal = read(STDIN_FILENO, readBuf->GetBuffer()+bufReadPos, StdinBufSize-bufReadPos);
			
			if (retVal <=0)
			{
				printf("Error in main event loop whilst processing stdin\n");
				_exit(-1);
			}
			else
			{
				bufReadPos += retVal;
				HandleStdin();
			}
		}
#endif
	}

#if defined(__SHIVA_WIN32) && !defined(__SHIVA_WINCE)
	if (QuirksMode)
		::FreeConsole();
	else
		::CloseHandle(StdinHandle);
	for (int i=10;StdinThread.IsRunning();i+=10)
		SHVThreadBase::Sleep(i);
#endif
}

/*************************************
 * StopEventLoop
 *************************************/
void SHVMainThreadEventDispatcherConsole::StopEventLoop(SHVBool errors)
{
	SignalDispatcher();
	if (!errors)
	{
	SHVString errStr = Queue->GetModuleList().GetStartupErrors();
		SHVConsole::ErrPrintf(_S("\n\nRegistering failed:\n\"%s\"\n\n"), errStr.GetSafeBuffer());
	}
}

/*************************************
 * HandleStdin
 *************************************/
void SHVMainThreadEventDispatcherConsole::HandleStdin()
{
size_t newline;
#if defined(__SHIVA_WIN32)
	if (true)
#else
	if (SHVConsole::NativeEncodingIsUTF8())
#endif
	{
	SHVStringUTF8 str;

		while ( (newline = StdinStream.SeekByte('\n', StdinPos)) < StdinStream.GetSize())
		{
		size_t bufSize = newline - StdinPos;
			str.SetBufferSize(bufSize);
			StdinStream.ReadStringUTF8(str,newline - StdinPos,StdinPos);
			StdinPos = newline + 1;
			
#if defined(__SHIVA_WIN32) && !defined(__SHIVA_WINCE)
			if (bufSize > 1 && str.GetBuffer()[bufSize-1] == '\r')
				str.GetBuffer()[bufSize-1] = '\0';
			this->selfSubs->EmitNow(*ModuleList,new SHVEvent(this,EventInternalSdin,SHVInt(),new SHVEventStdin(NULL,str.ReleaseBuffer())));
#else
			Queue->GetModuleList().EmitEvent(new SHVEventStdin(NULL,str.ReleaseBuffer()));
#endif
			
			StdinStream.Truncate(StdinPos);
		}
	}
	else
	{
	SHVString8 str;

		while ( (newline = StdinStream.SeekByte('\n', StdinPos)) < StdinStream.GetSize())
		{
		size_t bufSize = newline - StdinPos;
			str.SetBufferSize(bufSize);
			StdinStream.ReadString8(str,newline - StdinPos,StdinPos);
			StdinPos = newline + 1;
			
#if defined(__SHIVA_WIN32) && !defined(__SHIVA_WINCE)
			if (bufSize > 1 && str.GetBuffer()[bufSize-1] == '\r')
			{
				str.GetBuffer()[bufSize-1] = '\0';
			}
			{
			SHVStringUTF8 strUTF8;
			SHVStringConv conv(SHVStringConv::Enc8,SHVStringConv::EncUtf8);
			size_t len;
			
				conv.SetDosEncoding(GetConsoleCP());
			
				if (*conv.Convert((const SHVByte*)str.GetBufferConst(),NULL,len,&len) == 0)
				{
					conv.Convert((const SHVByte*)str.GetBufferConst(),strUTF8.SetBufferSize(len+1),len,&len);
					strUTF8.GetBuffer()[len] = '\0';
					this->selfSubs->EmitNow(*ModuleList,new SHVEvent(this,EventInternalSdin,SHVInt(),new SHVEventStdin(NULL,strUTF8.ReleaseBuffer())));
				}
				else
				{
					SHVASSERT(false);
				}
			}
#else
			Queue->GetModuleList().EmitEvent(new SHVEventStdin(NULL,str.ToStrUTF8()));
#endif
			
			StdinStream.Truncate(StdinPos);
		}
	}
}

/*************************************
 * OnEvent
 *************************************/
void SHVMainThreadEventDispatcherConsole::OnEvent(SHVEvent* event)
{
#if defined(__SHIVA_WINCE)
	SHVUNUSED_PARAM(event);
#elif defined(__SHIVA_WIN32)
	if (event->GetCaller() == this && SHVEvent::Equals(event,EventInternalSdin))
	{
		Queue->GetModuleList().EmitEvent((SHVEvent*)event->GetObject());
	}
#else
	SHVUNUSED_PARAM(event);
#endif
}


///\cond INTERNAL
#if defined(__SHIVA_WINCE)
/***************************
 * Print
 ***************************/
void SHVMainThreadEventDispatcherConsole::Print(const SHVString16C str)
{
	if (evDispatcherConsole && evDispatcherConsole->boundaries.cx > 0 && evDispatcherConsole->boundaries.cy > 0)
	{
		evDispatcherConsole->PrintInternal(str);
	}
}
void SHVMainThreadEventDispatcherConsole::PrintInternal(const SHVString16C str)
{
SHVMutexLocker l(Lock);
long pos = 0, oldPos = 0;
SHVString16 empty;
SHVString16 tmp;
size_t tmpLen;

	if (StringBuffers.GetCount() == 0)
	{
		StringBuffers.AddTail(empty.ReleaseBuffer());
	}

	for(pos=str.Find((const SHVWChar*)L"\n"); pos >= 0 && pos < (long)str.GetLength(); oldPos = pos+1, pos=str.Find((const SHVWChar*)L"\n",oldPos) )
	{
		tmp = str.Mid((size_t)oldPos,(size_t)(pos-oldPos));
		while (StringBuffers.GetLast().GetLength() + tmp.GetLength() > (size_t)boundaries.cx)
		{
			tmpLen = boundaries.cx - StringBuffers.GetLast().GetLength();
			StringBuffers.GetLast() += tmp.Left(tmpLen);
			tmp = tmp.Mid(tmpLen);
			StringBuffers.AddTail(empty.ReleaseBuffer());
		}
		StringBuffers.GetLast() += tmp;
		StringBuffers.AddTail(empty.ReleaseBuffer());
	}

	// Add the final bit after newline
	if (oldPos < (long)str.GetLength())
		StringBuffers.GetLast() += str.Mid((size_t)oldPos);

	if (wndConsole)
		::PostMessage(wndConsole,WM_SHV_PRINT,0,0);
}

/***************************
 * CreateDlg
 ***************************/
SHVBool SHVMainThreadEventDispatcherConsole::CreateDlg()
{
SHVString16C title(SHVString16C::FromWin32(L"SHIVA Console"));
int titleLength = title.GetLength();
HLOCAL templateHandle;
void* dlgTemplate;
char* buffer;
int len = AlignDWord( sizeof(DLGTEMPLATE) + sizeof(WORD) * 4
	+ ((titleLength) ? ((titleLength+1)*2 + sizeof(WORD)) : 0)
	);

	HWND oldWindow = FindWindowW(NULL,title.GetSafeBufferWin32());
	///\todo add code to make sure the window is created with the same application as us
	if (oldWindow) 
	{
		// set focus to foremost child window
		// The "| 0x01" is used to bring any owned windows to the foreground and
		// activate them.
		SetForegroundWindow((HWND)((ULONG) oldWindow | 0x00000001));
		return SHVBool::False;
	}

	templateHandle = LocalAlloc(LHND, len);

	dlgTemplate = LocalLock(templateHandle);
	
	memset(dlgTemplate,0,len);

	LPDLGTEMPLATE temp = (LPDLGTEMPLATE)dlgTemplate;
	temp->x  = 0;
	temp->y  = 0;
	temp->cx = 240;
	temp->cy = 180;
	temp->cdit  = 0; // number of controls
	temp->style = WS_CAPTION | WS_SYSMENU | WS_POPUP | WS_OVERLAPPED;
	temp->dwExtendedStyle = WS_EX_DLGMODALFRAME;

	// skip the dlgtemplate data from buffer
	buffer = (char*)dlgTemplate;
	buffer += sizeof(DLGTEMPLATE);

	// set menu and skip
	*(WORD*)buffer = 0;
	buffer += sizeof(WORD);

	// set class and skip
	*(WORD*)buffer = 0;
	buffer += sizeof(WORD);

	// set title and skip
	if (titleLength)
	{
	WCHAR* titleWChar = (WCHAR*)buffer;

		wcscpy(titleWChar,title.GetSafeBufferWin32());
	}
	else
	{
		*(WORD*)buffer = 0;
		buffer += sizeof(WORD);
	}

#ifdef __SHIVA_POCKETPC
	memset (&s_sai, 0, sizeof(s_sai));
	s_sai.cbSize = sizeof(s_sai);
#endif

	wndConsole = CreateDialogIndirectW(GetModuleHandle(NULL), (LPDLGTEMPLATE)dlgTemplate, NULL, &SHVMainThreadEventDispatcherConsole::WinceDlgProc);

	LocalUnlock(templateHandle);
	LocalFree(templateHandle);


	SetWindowLongPtr(wndConsole,GWLP_USERDATA,(LONG_PTR)this);

#ifdef __SHIVA_POCKETPC
	{
	SHMENUBARINFO mbi;

		memset(&mbi, 0, sizeof(SHMENUBARINFO));
		mbi.cbSize     = sizeof(SHMENUBARINFO);
		mbi.hwndParent = wndConsole;
		mbi.nToolBarId = 100;
		mbi.hInstRes   = GetModuleHandle(NULL);
		mbi.nBmpId     = 0;
		mbi.cBmpImages = 0;
		mbi.dwFlags    = SHCMBF_EMPTYBAR;

		SHCreateMenuBar(&mbi);
		if (mbi.hwndMB)
		{
		RECT menubarRect, winRect;
			GetWindowRect(wndConsole, &winRect);
			GetWindowRect(mbi.hwndMB, &menubarRect);
			winRect.bottom -= (menubarRect.bottom - menubarRect.top);
			MoveWindow(wndConsole, winRect.left, winRect.top, winRect.right, winRect.bottom, FALSE);
		}
	}
#endif

	// font stuff
	{
	HDC dc = ::GetDC(NULL);
	int dcBackup = ::SaveDC(dc);
	SIZE sz;
	LOGFONTW lf;

#if defined(__SHIVA_WINCE) && (_WIN32_WCE < 500)
		///\todo Implement a way to get the real message font from the system on windows CE
		memset(&lf, 0, sizeof(LOGFONT));
		lf.lfWeight = FW_NORMAL;
		lf.lfHeight = 15; // reasonable size
#else
	HFONT stockFont = (HFONT)::GetStockObject(SYSTEM_FONT);

		///\todo Test if this method works for older wince's
	
		// do something!
		SHVVERIFY(::GDIGetObject(stockFont, sizeof(LOGFONT), &lf));
#endif
		lf.lfPitchAndFamily = FIXED_PITCH|FF_MODERN;
		wcscpy(lf.lfFaceName, L"MS Shell Dlg");
		Font = ::CreateFontIndirect(&lf);
		::SendMessage(wndConsole,WM_SETFONT,(WPARAM)Font,0);

		::SelectObject(dc,Font);
		
		SHVVERIFY(::GetTextExtentPointW(dc,L" ",1,&sz));
		fontHeight = sz.cy + 2;
		fontWidth = sz.cx;

		::RestoreDC(dc,dcBackup);
		::ReleaseDC(NULL,dc);

	}

	// edit box
	edtConsole = ::CreateWindowExW(0,L"EDIT", L"", ES_LEFT|ES_AUTOHSCROLL|WS_TABSTOP|WS_CHILD|WS_VISIBLE,
			0, 0, 0, fontHeight, wndConsole, NULL, GetModuleHandle(NULL), NULL);
	edtProc = (WNDPROC)GetWindowLongPtr(edtConsole,GWLP_WNDPROC);
	SetWindowLongPtr(edtConsole,GWLP_WNDPROC,(LONG_PTR)&SHVMainThreadEventDispatcherConsole::WinceEditProc);

	::SendMessage(edtConsole,WM_SETFONT,(WPARAM)Font,0);

	Resize();
	::ShowWindow(wndConsole,SW_SHOW);
	::UpdateWindow(wndConsole);

	return SHVBool::True;
}

/*************************************
 * Resize
 *************************************/
void SHVMainThreadEventDispatcherConsole::Resize()
{
RECT rect;
	::GetClientRect(wndConsole,&rect);
	boundaries.cx = (rect.right - rect.left) / fontWidth;
	boundaries.cy = (rect.bottom - rect.top) / fontHeight;
	::MoveWindow(edtConsole,rect.left,rect.bottom-fontHeight,rect.right-rect.left,fontHeight,TRUE);
}

/*************************************
 * WinceDlgProc
 *************************************/
INT_PTR CALLBACK SHVMainThreadEventDispatcherConsole::WinceDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
SHVMainThreadEventDispatcherConsole* self = evDispatcherConsole;
INT_PTR retVal = 0;

	switch (message) 
	{
	case WM_INITDIALOG:
		// Create a Done button and size it.  
		{
		SHINITDLGINFO shidi;
			shidi.dwMask = SHIDIM_FLAGS;
			shidi.dwFlags = SHIDIF_DONEBUTTON | SHIDIF_SIPDOWN | SHIDIF_SIZEDLGFULLSCREEN;
			shidi.hDlg = hWnd;
			SHInitDialog(&shidi);
		}
		return TRUE; 

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			self->ModuleList->CloseApp();
			return TRUE;
		}
		break;

	// normal message types
	case WM_SHV_DISPATCHMESSAGES:
		self->DispatchEvents();
		break;
	case WM_SHV_PRINT:
		InvalidateRect(hWnd,NULL,TRUE);
		break;
	case WM_CLOSE:
		self->ModuleList->CloseApp();
		break;
	case WM_ERASEBKGND:
		retVal = DefWindowProc(hWnd, message, wParam, lParam);
#ifdef __SHIVA_POCKETPC
	case WM_SETTINGCHANGE:
		if (evDispatcherConsole)
			SHHandleWMSettingChange(hWnd, wParam, lParam, &evDispatcherConsole->s_sai);
		break;
	case WM_ACTIVATE:
		// Notify shell of our activate message
		if (self)
		{
			SHHandleWMActivate(hWnd, wParam, lParam, &evDispatcherConsole->s_sai, FALSE);
			::SetFocus(self->edtConsole);
		}
		break;
#else
	case WM_ACTIVATE:
		if (self)
			::SetFocus(self->edtConsole);
		break;
#endif
	case WM_PAINT:
		{
		PAINTSTRUCT ps;
		HDC hdc;
			hdc = BeginPaint(hWnd, &ps);

			if (evDispatcherConsole)
			{
			RECT rect;
			SHVListIterator<SHVString16,SHVStringBuffer16> itr(evDispatcherConsole->StringBuffers);
			bool first = true;
			int dcBackup = ::SaveDC(hdc);

				::SelectObject(hdc,self->Font);

				// obtain boundaries
				::GetClientRect(hWnd,&rect);
				if (evDispatcherConsole->edtConsole)
				{
				RECT wndRect, edtRect;
					::GetWindowRect(hWnd,&wndRect);
					::GetWindowRect(evDispatcherConsole->edtConsole,&edtRect);
					rect.bottom = edtRect.top - wndRect.top;
				}

				// print the text
				rect.top = rect.bottom - evDispatcherConsole->fontHeight;
				for (;rect.bottom > 0 && evDispatcherConsole->StringBuffers.MovePrev(itr.Pos()); rect.bottom -= evDispatcherConsole->fontHeight, rect.top -= evDispatcherConsole->fontHeight)
				{
					if (first && itr.Get().IsEmpty())
					{
						if (!evDispatcherConsole->StringBuffers.MovePrev(itr.Pos()))
							break;
					}
					::DrawTextW(hdc,itr.Get().GetSafeBufferWin32(),(int)itr.Get().GetLength(),&rect,DT_TOP|DT_SINGLELINE);
					first = false;
				}

				while (itr.Pos() && evDispatcherConsole->StringBuffers.GetHeadPosition() != itr.Pos() && evDispatcherConsole->StringBuffers.GetCount())
				{
					evDispatcherConsole->StringBuffers.RemoveHead();
				}

				::RestoreDC(hdc,dcBackup);
			}

			EndPaint(hWnd, &ps);
		}
		break;
	case WM_WINDOWPOSCHANGED:
		if (self)
		{
		WINDOWPOS* wpos = (WINDOWPOS*)lParam;
			if (self->edtConsole && !(wpos->flags&SWP_FRAMECHANGED) && (!(wpos->flags&SWP_NOMOVE) || !(wpos->flags&SWP_NOSIZE)))
			{
				self->Resize();
			}
			InvalidateRect(hWnd,NULL,TRUE);
		}
		retVal = DefWindowProc(hWnd, message, wParam, lParam);
		break;
	default:
		retVal = DefWindowProc(hWnd, message, wParam, lParam);
	}

	return retVal;
}

/*************************************
 * WinceDlgProc
 *************************************/
LRESULT CALLBACK SHVMainThreadEventDispatcherConsole::WinceEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
LRESULT retVal = 0;

	switch (message) 
	{
	case WM_CHAR:
		if (wParam == VK_RETURN)
		{
		SHVString16 str;
			str.SetBufferSize(::GetWindowTextLength(hWnd)+1);
			::GetWindowTextW(hWnd,str.GetBufferWin32(),::GetWindowTextLength(hWnd)+1);
			evDispatcherConsole->Queue->GetModuleList().EmitEvent(new SHVEventStdin(NULL,str.ToStrUTF8()));
			::SetWindowTextW(hWnd,L"");
			break;
		}
	default:
		if (evDispatcherConsole && evDispatcherConsole->edtProc)
			retVal = CallWindowProc(evDispatcherConsole->edtProc, hWnd, message, wParam, lParam);
		else
			retVal = DefWindowProc(hWnd, message, wParam, lParam);
	}

	return retVal;
}

#elif defined(__SHIVA_WIN32)
/*************************************
 * StdinFunc
 *************************************/
void SHVMainThreadEventDispatcherConsole::StdinFunc()
{
SHVBufferRef readBuf;
SHVWChar readBufW[StdinBufSize+1];
SHVStringUTF8 readBufUTF8;
SHVStringConv conv(SHVStringConv::Enc16,SHVStringConv::EncUtf8);
size_t bufReadPos = 0;
int retVal;

	readBufUTF8.SetBufferSize(StdinBufSize*4+1);

	StdinPos = 0;
	while (Running() || Initializing)
	{
	DWORD bytesRead;
	bool eofFlag = (ReadConsoleW(StdinHandle,readBufW,StdinBufSize,&bytesRead,NULL) ? false : true);
		if (eofFlag && bytesRead < 0)
			bytesRead = 0;
		retVal = (int)bytesRead;
		
		if (retVal <=0)
		{
			if (!Running())
				continue;
			printf("Error in main event loop whilst processing stdin\n");
			break;
		}
		else
		{
		size_t utf8len,utf8pos;
			readBufW[retVal] = 0;
			if (*conv.Convert((const SHVByte*)readBufW,NULL,0,&utf8len) == 0)
			{
				if (utf8len >= readBufUTF8.GetBufferLen())
					readBufUTF8.SetBufferSize(utf8len+1);
				conv.Convert((const SHVByte*)readBufW,readBufUTF8.GetBuffer(),utf8len);
				readBufUTF8.GetBuffer()[utf8len] = '\0';

				for (utf8pos=0; utf8pos<utf8len;)
				{
					if (bufReadPos == StdinBufSize || readBuf.IsNull())
					{
						readBuf = new SHVBufferChunk<StdinBufSize+1>();
						StdinStream.AddBuffer(readBuf);
						bufReadPos = 0;
					}
					if ( (StdinBufSize-bufReadPos) >= (utf8len-utf8pos) )
					{
						::memcpy(readBuf->GetBuffer()+bufReadPos,readBufUTF8.GetBufferConst()+utf8pos,(utf8len-utf8pos));
						bufReadPos += (utf8len-utf8pos);
						utf8pos = utf8len;
					}
					else
					{
						::memcpy(readBuf->GetBuffer()+bufReadPos,readBufUTF8.GetBufferConst()+utf8pos,(StdinBufSize-bufReadPos));
						utf8pos += (StdinBufSize-bufReadPos);
						bufReadPos = StdinBufSize;
					}
					readBuf->GetBuffer()[bufReadPos] = '\0'; // clear any unwanted Xtra newline
				}

				HandleStdin();
			}
			else
			{
				///\todo error handling
			}
		}
	}
}
#endif
///\endcond
