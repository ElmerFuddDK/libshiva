#include "stdafx.h"
#include "../../../include/platformspc.h"

#include "shvcontrolcontainerwrapperwin32.h"
#include "shvcontrolimplementerwin32.h"

//=========================================================================================================
// SHVControlContainerWrapperWin32 - wrapper for HWND's
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVControlContainerWrapperWin32::SHVControlContainerWrapperWin32(SHVGUIManager* manager, HWND hWnd) : SHVControlContainer(manager,new SHVControlImplementerContainerWrapperWin32())
{
	SetHandle(hWnd);
}

/*************************************
 * Destructor
 *************************************/
SHVControlContainerWrapperWin32::~SHVControlContainerWrapperWin32()
{
	SetHandle(NULL);
}

/*************************************
 * SetHandle
 *************************************/
SHVControlContainerWrapperWin32* SHVControlContainerWrapperWin32::SetHandle(HWND hWnd)
{
SHVControlImplementerContainerWrapperWin32* ctrlImpl = (SHVControlImplementerContainerWrapperWin32*)GetImplementor();

	ctrlImpl->Window = hWnd;

	return this;
}

/*************************************
 * GetType
 *************************************/
int SHVControlContainerWrapperWin32::GetType()
{
	return SHVControl::TypeContainer;
}

/*************************************
 * GetData
 *************************************/
SHVBool SHVControlContainerWrapperWin32::GetData(SHVDataBinder* data)
{
	return SHVBool::False;
}

/*************************************
 * SetData
 *************************************/
SHVBool SHVControlContainerWrapperWin32::SetData(SHVDataBinder* data)
{
	return SHVBool::False;
}

/*************************************
 * Create
 *************************************/
SHVBool SHVControlContainerWrapperWin32::Create(int flags)
{
	return SHVBool::False;
}



//=========================================================================================================
// SHVControlImplementerWrapperWin32
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVControlImplementerContainerWrapperWin32::SHVControlImplementerContainerWrapperWin32() : SHVControlImplementerWin32<SHVControlImplementerContainer>()
{
}

/*************************************
 * GetSubType
 *************************************/
int SHVControlImplementerContainerWrapperWin32::GetSubType(SHVControl* owner)
{
	return SHVControlContainer::SubTypeDefault;
}

/*************************************
 * Create
 *************************************/
SHVBool SHVControlImplementerContainerWrapperWin32::Create(SHVControl* owner, SHVControlImplementer* parent, int flags)
{
	return SHVBool::False;
}


/*************************************
 * GetRegionRect
 *************************************/
SHVRect SHVControlImplementerContainerWrapperWin32::GetRegionRect()
{
RECT nativeRect;

	SHVASSERT(IsCreated());

	::GetClientRect(GetHandle(),&nativeRect);

	return SHVRect(nativeRect.left,nativeRect.top,nativeRect.right,nativeRect.bottom);
}

/*************************************
 * SetSize
 *************************************/
void SHVControlImplementerContainerWrapperWin32::SetSize(SHVControlContainer* owner, int widthInPixels, int heightInPixels, SHVControlContainer::PosModes mode)
{
}

/*************************************
 * GetTitle
 *************************************/
SHVStringBuffer SHVControlImplementerContainerWrapperWin32::GetTitle()
{
SHVString retVal;

	return retVal.ReleaseBuffer();
}

/*************************************
 * SetTitle
 *************************************/
void SHVControlImplementerContainerWrapperWin32::SetTitle(const SHVStringC& title)
{
}

/*************************************
 * GetColor
 *************************************/
SHVColor* SHVControlImplementerContainerWrapperWin32::GetColor(SHVControlContainer* owner)
{
	return NULL;
}

/*************************************
 * SetColor
 *************************************/
void SHVControlImplementerContainerWrapperWin32::SetColor(SHVControlContainer* owner, SHVColor* color)
{
}

/*************************************
 * SetMinimumSize
 *************************************/
void SHVControlImplementerContainerWrapperWin32::SetMinimumSize(SHVControlContainer* owner, int widthInPixels, int heightInPixels)
{
}

/*************************************
 * GetMinimumSizeInPixels
 *************************************/
SHVPoint SHVControlImplementerContainerWrapperWin32::GetMinimumSizeInPixels(SHVControlContainer* owner)
{
	return SHVPoint(0,0);
}

/*************************************
 * SetResizable
 *************************************/
void SHVControlImplementerContainerWrapperWin32::SetResizable(bool resizable)
{
}
