#include "stdafx.h"
#include "../../../include/platformspc.h"

#include "shvcontrolwrapperwin32.h"
#include "shvcontrolimplementerwin32.h"
#include "utils/shvdrawwin32.h"

//=========================================================================================================
// SHVControlWrapperWin32 - wrapper for HWND's
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVControlWrapperWin32::SHVControlWrapperWin32(SHVGUIManager* manager, HWND hWnd) : SHVControl(manager,new SHVControlImplementerWrapperWin32())
{
	SetHandle(hWnd);
}

/*************************************
 * Destructor
 *************************************/
SHVControlWrapperWin32::~SHVControlWrapperWin32()
{
	SetHandle(NULL);
}

/*************************************
 * SetHandle
 *************************************/
SHVControlWrapperWin32* SHVControlWrapperWin32::SetHandle(HWND hWnd)
{
SHVControlImplementerWrapperWin32* ctrlImpl = (SHVControlImplementerWrapperWin32*)GetImplementor();
	ctrlImpl->Window = hWnd;
	return this;
}

/*************************************
 * GetType
 *************************************/
int SHVControlWrapperWin32::GetType()
{
	return SHVControl::TypeCustom;
}

/*************************************
 * GetData
 *************************************/
SHVBool SHVControlWrapperWin32::GetData(SHVControlData* data)
{
	return SHVBool::False;
}

/*************************************
 * SetData
 *************************************/
SHVBool SHVControlWrapperWin32::SetData(SHVControlData* data)
{
	return SHVBool::False;
}


//=========================================================================================================
// SHVControlImplementerWrapperWin32
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVControlImplementerWrapperWin32::SHVControlImplementerWrapperWin32() : SHVControlImplementerWin32<SHVControlImplementer>()
{
}

/*************************************
 * GetRect
 *************************************/
SHVRect SHVControlImplementerWrapperWin32::GetRect(SHVControl* owner)
{
RECT nativeRect;
HWND parent = ::GetParent(GetHandle());

	SHVASSERT(IsCreated() && owner);

	::GetWindowRect(GetHandle(),&nativeRect);

	if (parent)
	{
	POINT topleft, bottomright;

		topleft.x = nativeRect.left;
		topleft.y = nativeRect.top;
		bottomright.x = nativeRect.right;
		bottomright.y = nativeRect.bottom;

		ScreenToClient(parent,&topleft);
		ScreenToClient(parent,&bottomright);

		return SHVRect(topleft.x,topleft.y,bottomright.x,bottomright.y);
	}

	return SHVDrawWin32::MapRect(nativeRect);
}

/*************************************
 * GetSubType
 *************************************/
int SHVControlImplementerWrapperWin32::GetSubType(SHVControl* owner)
{
	return SHVControl::SubTypeDefault;
}

/*************************************
 * Create
 *************************************/
SHVBool SHVControlImplementerWrapperWin32::Create(SHVControl* owner, SHVControlImplementer* parent, int flags)
{
	return SHVBool::False;
}
