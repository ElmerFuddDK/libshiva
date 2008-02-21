#ifndef __SHIVA_WIN32GUI_H
#define __SHIVA_WIN32GUI_H


#include "shvcontrolimplementerwin32base.h"
#include "shvguimanagerwin32.h"
#include "../../include/shvcontrol.h"
#include "../../include/shvcontrolimplementer.h"

#ifndef __SHIVA_WIN32
# error This code only works for win32
#elif defined(__SHIVA_MFC)
# error This code does not work in MFC mode
#endif


#define WM_SHV_DISPATCHMESSAGES    WM_USER+100
#define WM_SHV_PRETRANSLATEMESSAGE WM_USER+101


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
	inline static void ConvertNewlines(SHVString& str);

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
	return ((SHVControlImplementerWin32Base*)control->GetImplementor()->GetNative())->GetHandle();
}

/*************************************
 * GetHandle
 *************************************/
HWND Win32::GetHandle(SHVControlImplementer* implementer)
{
	return ((SHVControlImplementerWin32Base*)implementer->GetNative())->GetHandle();
}

/*************************************
 * GetHandle
 *************************************/
HWND Win32::GetMainWndHandle(SHVControl* control)
{
	return ((SHVControlImplementerWin32Base*)control->GetManager()->GetMainWindow()->GetImplementor()->GetNative())->GetHandle();
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
	return (HINSTANCE)GetGUIConfig(control).FindPtr(SHVGUIManager::CfgInstanceHandle).ToPtr();
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
	return (str.Find(_T("\n")) >= 0);
}

/*************************************
 * ConvertNewlinesC
 *************************************/
SHVStringBuffer Win32::ConvertNewlinesC(const SHVStringC str)
{
SHVString retVal(str);

	retVal.Replace(_T("\n"),_T("\r\n"));

	return retVal.ReleaseBuffer();
}

/*************************************
 * ConvertNewlines
 *************************************/
void Win32::ConvertNewlines(SHVString& str)
{
	str.Replace(_T("\n"),_T("\r\n"));
}

#endif
