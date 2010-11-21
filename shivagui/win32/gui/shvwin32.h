#ifndef __SHIVA_WIN32GUI_H
#define __SHIVA_WIN32GUI_H

#if !defined(__SHIVA_MFC)
# include "shvcontrolimplementerwin32base.h"
#endif
#include "shvguimanagerwin32.h"
#include "../../../include/gui/shvcontrol.h"
#include "../../../include/gui/shvcontrolimplementer.h"

#ifndef __SHIVA_WIN32
# error This code only works for win32
#endif

#if defined(__SHIVA_WINCE) && !defined(MulDiv)
# define MulDiv(num,numerator,denom) (num*numerator)/denom
#endif

#ifndef WS_EX_CONTROLPARENT
# define WS_EX_CONTROLPARENT 0
#endif

#ifndef WS_EX_NOPARENTNOTIFY
# define WS_EX_NOPARENTNOTIFY 0
#endif



#define WM_SHV_DISPATCHMESSAGES    0x8001
#define WM_SHV_PRETRANSLATEMESSAGE 0x8002
#define WM_SHV_NOTIFY              0x8003
#define WM_SHV_DRAWITEM            0x8004


class Win32
{
public:

	// constants
	inline static HWND InvalidHandle();

	inline static HWND GetHandle(SHVControl* control);
	inline static HWND GetHandle(SHVControlImplementer* implementer);
	inline static HWND GetMainWndHandle(SHVControl* control);

	inline static SHVConfig& GetGUIConfig(SHVControl* control);
	inline static HINSTANCE GetInstance(SHVControl* control);

	static int MapFlags(int shivaflags);

	inline static SHVDrawWin32* CreateDraw(SHVControl* control, HDC hDC);
	inline static SHVDrawWin32* CreateDrawPaint(SHVControl* control);

	inline static bool CheckForNewlines(const SHVStringC str);
	inline static SHVStringBuffer ConvertNewlinesC(const SHVStringC str);
	inline static void ConvertNewlinesBack(SHVString& str);

};


// ============================================= implementation ============================================= //

/*************************************
 * InvalidHandle
 *************************************/
HWND Win32::InvalidHandle()
{
	return NULL;
}

/*************************************
 * GetHandle
 *************************************/
HWND Win32::GetHandle(SHVControl* control)
{
#ifdef __SHIVA_MFC
	return InvalidHandle();
#else
	return ((SHVControlImplementerWin32Base*)control->GetImplementor()->GetNative())->GetHandle();
#endif
}

/*************************************
 * GetHandle
 *************************************/
HWND Win32::GetHandle(SHVControlImplementer* implementer)
{
#ifdef __SHIVA_MFC
	return InvalidHandle();
#else
	return ((SHVControlImplementerWin32Base*)implementer->GetNative())->GetHandle();
#endif
}

/*************************************
 * GetMainWndHandle
 *************************************/
HWND Win32::GetMainWndHandle(SHVControl* control)
{
#ifdef __SHIVA_MFC
	return InvalidHandle();
#else
	return ((SHVControlImplementerWin32Base*)control->GetManager()->GetMainWindow()->GetImplementor()->GetNative())->GetHandle();
#endif
}

/*************************************
 * GetGUIConfig
 *************************************/
SHVConfig& Win32::GetGUIConfig(SHVControl* control)
{
	return control->GetModuleList().GetConfig(SHVModuleList::CfgGUI);
}

/*************************************
 * GetInstance
 *************************************/
HINSTANCE Win32::GetInstance(SHVControl* control)
{
	return (HINSTANCE)GetGUIConfig(control).FindPtr(SHVGUIManager::CfgInstanceHandle)->ToPtr();
}

/*************************************
 * CreateDraw
 *************************************/
SHVDrawWin32* Win32::CreateDraw(SHVControl* control, HDC hDC)
{
	return ((SHVGUIManagerWin32*)control->GetManager())->CreateDraw(hDC);
}

/*************************************
 * CreateDrawPaint
 *************************************/
SHVDrawWin32* Win32::CreateDrawPaint(SHVControl* control)
{
	return (SHVDrawWin32*)((SHVGUIManagerWin32*)control->GetManager())->CreateDrawPaint(GetHandle(control));
}

/*************************************
 * CheckForNewlines
 *************************************/
bool Win32::CheckForNewlines(const SHVStringC str)
{
	return (str.Find(_S("\n")) >= 0);
}

/*************************************
 * ConvertNewlinesC
 *************************************/
SHVStringBuffer Win32::ConvertNewlinesC(const SHVStringC str)
{
SHVString retVal(str);

	retVal.Replace(_S("\n"),_S("\r\n"));

	return retVal.ReleaseBuffer();
}

/*************************************
 * ConvertNewlines
 *************************************/
void Win32::ConvertNewlinesBack(SHVString& str)
{
	str.Replace(_S("\r\n"),_S("\n"));
}

#endif
