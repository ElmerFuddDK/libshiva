#ifndef __SHIVA_WIN32GUI_SHVCONTROLIMPLEMENTERMAINWINDOW_H
#define __SHIVA_WIN32GUI_SHVCONTROLIMPLEMENTERMAINWINDOW_H


#include "../../../include/gui/shvcontrolcontainer.h"
#include "shvcontrolimplementerwin32.h"

#ifndef __SHIVA_WIN32
# error This code only works for win32
#elif defined(__SHIVA_MFC)
# error This code does not work in MFC mode
#endif


// Forward declare
class SHVMainThreadEventDispatcherWin32;

//-=========================================================================================================
/// SHVControlImplementerMainWindowWin32 - Main window implementation
/**
 * Use this class as a main window implementer in win32.
 */

class SHVControlImplementerMainWindowWin32 : public SHVControlImplementerWin32<SHVControlImplementerContainer>
{
public:


	SHVControlImplementerMainWindowWin32(HINSTANCE hinstance, SHVMainThreadEventDispatcherWin32* dispatcher);


	// From SHVControlImplementer
	virtual int GetSubType(SHVControl* owner);

	virtual SHVBool Create(SHVControl* owner, SHVControlImplementer* parent, int flags);


	// From SHVControlImplementerContainer
	virtual SHVRect GetRegionRect();
	virtual void SetSize(SHVControlContainer* owner, int widthInPixels, int heightInPixels, SHVControlContainer::PosModes mode);
	virtual SHVMenu* CreateMenu(SHVControlContainer* owner, SHVEventSubscriberBase* subscriber);

	virtual SHVStringBuffer GetTitle();
	virtual void SetTitle(const SHVStringC& title);

	virtual SHVColor* GetColor(SHVControlContainer* owner);
	virtual void SetColor(SHVControlContainer* owner, SHVColor* color);

	virtual void SetMinimumSize(SHVControlContainer* owner, int widthInPixels, int heightInPixels);
	virtual SHVPoint GetMinimumSizeInPixels(SHVControlContainer* owner);


	// Register class
	static void RegisterClass(HINSTANCE hInstance);


protected:

	virtual void SetResizable(bool resizable);

private:
	///\cond INTERNAL
	SHVPoint MinSize;
	SHVPoint DecorationsSize;
	HINSTANCE hInstance;
	SHVMainThreadEventDispatcherWin32* Dispatcher;
	SHVColorRef Color;

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	///\endcond
};

#endif
