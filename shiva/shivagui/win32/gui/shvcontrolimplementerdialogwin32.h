#ifndef __SHIVA_WIN32GUI_SHVCONTROLIMPLEMENTERMAINWINDOW_H
#define __SHIVA_WIN32GUI_SHVCONTROLIMPLEMENTERMAINWINDOW_H


#include "../../../include/gui/shvcontrolcontainer.h"
#include "shvcontrolimplementerwin32.h"

#ifndef __SHIVA_WIN32
# error This code only works for win32
#elif defined(__SHIVA_MFC)
# error This code does not work in MFC mode
#endif

#ifdef __SHIVA_POCKETPC
# include <aygshell.h>
# include "shvmenuwin32.h"
#endif


// Forward declare
class SHVMainThreadEventDispatcherWin32;

//-=========================================================================================================
/// SHVControlImplementerDialogWin32 - Dialog window implementation
/**
 * Use this class as a main window implementer in win32.
 */

class SHVControlImplementerDialogWin32 : public SHVControlImplementerWin32<SHVControlImplementerContainer>
{
public:


	SHVControlImplementerDialogWin32(int subType);


	// From SHVControlImplementer
	virtual int GetSubType(SHVControl* owner);

	virtual SHVBool Create(SHVControl* owner, SHVControlImplementer* parent, int flags);
	virtual void SetRect(SHVControl* owner, const SHVRect& rect);

	// from SHVControlImplementer
	virtual SHVBool SetFlag(SHVControl* owner, int flag, bool enable);
	virtual bool GetFlag(SHVControl* owner, int flag);


	// From SHVControlImplementerContainer
	virtual SHVRect GetRegionRect();
	virtual void SetSize(SHVControlContainer* owner, int widthInPixels, int heightInPixels, SHVControlContainer::PosModes mode);
	virtual SHVMenu* CreateMenu(SHVControlContainer* owner, SHVEventSubscriberBase* subscriber);

	virtual SHVStringBuffer GetTitle(SHVControlContainer* owner);
	virtual void SetTitle(SHVControlContainer* owner, const SHVStringC& title);

	virtual SHVColor* GetColor(SHVControlContainer* owner);
	virtual void SetColor(SHVControlContainer* owner, SHVColor* color);

	virtual void SetMinimumSize(SHVControlContainer* owner, int widthInPixels, int heightInPixels);
	virtual SHVPoint GetMinimumSizeInPixels(SHVControlContainer* owner);


	// Register class
	static void RegisterClass(SHVGUIManager* manager, HINSTANCE hInstance);


protected:

	virtual void SetResizable(bool resizable);

private:
	///\cond INTERNAL
	int SubType;
	SHVPoint MinSize;
	SHVPoint DecorationsSize;
	SHVColorRef Color;
	HWND FocusWnd;
	bool IgnoreResize;
#ifdef __SHIVA_POCKETPC
	SHVMenuCommandBarPocketPCPtr CmdWnd;
	SHACTIVATEINFO s_sai;
#endif

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	///\endcond
};

#endif
