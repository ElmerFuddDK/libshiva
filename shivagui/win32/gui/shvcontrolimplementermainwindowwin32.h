#ifndef __SHIVA_WIN32GUI_SHVCONTROLIMPLEMENTERMAINWINDOW_H
#define __SHIVA_WIN32GUI_SHVCONTROLIMPLEMENTERMAINWINDOW_H


#include "../../include/shvcontrolcontainer.h"
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

	virtual SHVBool Create(SHVControl* owner, SHVControlImplementer* parent);


	// From SHVControlImplementerContainer
	virtual SHVRect GetRegionRect();

	virtual SHVStringBuffer GetTitle();
	virtual void SetTitle(const SHVStringC& title);


	// Register class
	static void RegisterClass(HINSTANCE hInstance);

private:
	///\cond INTERNAL
	HINSTANCE hInstance;
	SHVMainThreadEventDispatcherWin32* Dispatcher;

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	///\endcond
};

#endif
