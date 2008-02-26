#ifndef __SHIVA_WIN32GUI_SHVCONTROLIMPLEMENTERCONTAINERWINDOW_H
#define __SHIVA_WIN32GUI_SHVCONTROLIMPLEMENTERCONTAINERWINDOW_H


#include "../../include/shvcontrolcontainer.h"
#include "shvcontrolimplementerwin32.h"

#ifndef __SHIVA_WIN32
# error This code only works for win32
#elif defined(__SHIVA_MFC)
# error This code does not work in MFC mode
#endif


//-=========================================================================================================
/// SHVControlImplementerContainerWindowWin32 - Container sub window
/**
 */

class SHVControlImplementerContainerWindowWin32 : public SHVControlImplementerWin32<SHVControlImplementerContainerCustomDraw>
{
public:


	SHVControlImplementerContainerWindowWin32(int subType);


	// From SHVControlImplementer
	virtual int GetSubType(SHVControl* owner);

	virtual SHVBool Create(SHVControl* owner, SHVControlImplementer* parent, int flags);


	// From SHVControlImplementerContainer
	virtual SHVRect GetRegionRect();
	virtual void SetSize(SHVControlContainer* owner, int widthInPixels, int heightInPixels, SHVControlContainer::PosModes mode);

	virtual SHVStringBuffer GetTitle();
	virtual void SetTitle(const SHVStringC& title);

	virtual SHVColor* GetColor(SHVControlContainer* owner);
	virtual void SetColor(SHVControlContainer* owner, SHVColor* color);

	virtual void SetMinimumSize(SHVControlContainer* owner, int widthInPixels, int heightInPixels);
	virtual SHVPoint GetMinimumSizeInPixels(SHVControlContainer* owner);

	// From SHVControlImplementerContainerCustomDraw
	virtual void SubscribeDraw(SHVEventSubscriberBase* subscriber);


	// Register class
	static void RegisterClass(SHVGUIManager* manager, HINSTANCE hInstance);


protected:

	virtual void SetResizable(bool resizable);

private:
	///\cond INTERNAL
	SHVPoint MinSize;
	int SubType;
	SHVColorRef Color;
	SHVEventSubscriberBaseRef Subscriber;

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	///\endcond
};

#endif
