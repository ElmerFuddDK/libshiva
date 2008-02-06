#include "stdafx.h"
#include "../../../include/platformspc.h"

#include "shvcontrolimplementerwin32base.h"
#include "shvwin32.h"

//=========================================================================================================
// SHVControlImplementerWin32Base - implementation of base implementer methods
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVControlImplementerWin32Base::SHVControlImplementerWin32Base()
{
	Window = Win32::InvalidHandle();
}

/*************************************
 * Destructor
 *************************************/
SHVControlImplementerWin32Base::~SHVControlImplementerWin32Base()
{
	Destroy(NULL);
 }

/*************************************
 * IsCreated
 *************************************/
SHVBool SHVControlImplementerWin32Base::IsCreated()
{
	return (Window != Win32::InvalidHandle());
}

/*************************************
 * Reparent
 *************************************/
SHVBool SHVControlImplementerWin32Base::Reparent(SHVControl* owner, SHVControlImplementer* parent)
{
HWND prevParent;

	SHVASSERT(IsCreated() && parent->IsCreated());

	prevParent = ::SetParent(Window,Win32::GetHandle(parent));

	return prevParent != NULL;
}

/*************************************
 * Destroy
 *************************************/
SHVBool SHVControlImplementerWin32Base::Destroy(SHVControl* owner)
{
SHVBool retVal(IsCreated());

	if (retVal)
	{
		::DestroyWindow(Window);
		Window = Win32::InvalidHandle();
	}

	return retVal;
}

/*************************************
 * GetRect
 *************************************/
SHVRect SHVControlImplementerWin32Base::GetRect(SHVControl* owner)
{
RECT nativeRect;

	SHVASSERT(IsCreated());

	::GetWindowRect(Window,&nativeRect);

	return SHVRect(nativeRect.left,nativeRect.top,nativeRect.right,nativeRect.bottom);
}

/*************************************
 * SetRect
 *************************************/
void SHVControlImplementerWin32Base::SetRect(SHVControl* owner, const SHVRect& rect)
{
	SHVASSERT(IsCreated());
	::MoveWindow(Window,rect.GetLeft(),rect.GetTop(),rect.GetWidth(),rect.GetHeight(),TRUE);
}

/*************************************
 * SetFlag
 *************************************/
SHVBool SHVControlImplementerWin32Base::SetFlag(SHVControl* owner, int flag, bool enable)
{
SHVBool retVal(IsCreated());

	if (retVal && (flag & SHVControl::FlagVisible))
	{
		::ShowWindow(Window, enable ? SW_SHOW : SW_HIDE);
	}

	if (retVal && (flag & SHVControl::FlagDisabled))
	{
		::EnableWindow(Window, !enable);
	}

	return retVal;
}

/*************************************
 * GetFlag
 *************************************/
bool SHVControlImplementerWin32Base::GetFlag(SHVControl* owner, int flag)
{
bool retVal(IsCreated());
DWORD styles = (retVal ? ::GetWindowLong(Window,GWL_STYLE) : 0);

	if (retVal && (flag & SHVControl::FlagVisible))
	{
		retVal = ((styles & WS_VISIBLE) ? true : false);
	}

	if (retVal && (flag & SHVControl::FlagDisabled))
	{
		retVal = ((styles & WS_DISABLED) ? true : false);
	}

	return retVal;
}

/*************************************
 * GetHandle
 *************************************/
HWND SHVControlImplementerWin32Base::GetHandle()
{
	return Window;
}

/*************************************
 * SetHandle
 *************************************/
void SHVControlImplementerWin32Base::SetHandle(HWND handle)
{
	if (Window == handle)
		return;

	SHVASSERT(!IsCreated());
	Window = handle;
}
