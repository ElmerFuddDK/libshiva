#ifndef __SHIVA_WIN32GUI_SHVCONTROLIMPLEMENTERBUTTON_H
#define __SHIVA_WIN32GUI_SHVCONTROLIMPLEMENTERBUTTON_H


#include "../../include/shvcontrolbutton.h"
#include "shvcontrolimplementerwin32.h"

#ifndef __SHIVA_WIN32
# error This code only works for win32
#elif defined(__SHIVA_MFC)
# error This code does not work in MFC mode
#endif


//-=========================================================================================================
/// SHVControlImplementerButtonWin32 - button implementation
/**
 */

class SHVControlImplementerButtonWin32 : public SHVControlImplementerWin32<SHVControlImplementerButton>
{
public:


	SHVControlImplementerButtonWin32();


	// From SHVControlImplementer
	virtual int GetSubType(SHVControl* owner);

	virtual SHVBool Create(SHVControl* owner, SHVControlImplementer* parent, int flags);


	// From SHVControlImplementerContainer
	virtual SHVStringBuffer GetText();
	virtual void SetText(const SHVStringC& text);


private:
	///\cond INTERNAL
	WNDPROC OrigProc;

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	///\endcond
};

#endif
