#ifndef __SHIVA_WIN32GUI_H
#define __SHIVA_WIN32GUI_H


#include "shvcontrolimplementerwin32base.h"
#include "../../include/shvguimanager.h"
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

	inline static SHVConfig& GetGUIConfig(SHVControl* control);
	inline static HINSTANCE GetInstance(SHVControl* control);

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

#endif
