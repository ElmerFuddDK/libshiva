#ifndef __SHIVA_WIN32GUI_SHVCONTROLIMPLEMENTERCHECKBOX_H
#define __SHIVA_WIN32GUI_SHVCONTROLIMPLEMENTERCHECKBOX_H


#include "../../../include/gui/shvcontrolcheckbox.h"
#include "shvcontrolimplementerwin32.h"

#ifndef __SHIVA_WIN32
# error This code only works for win32
#elif defined(__SHIVA_MFC)
# error This code does not work in MFC mode
#endif


//-=========================================================================================================
/// SHVControlImplementerCheckboxWin32 - checkbox implementation
/**
 */

class SHVControlImplementerCheckboxWin32 : public SHVControlImplementerWin32<SHVControlImplementerCheckbox>
{
public:


	SHVControlImplementerCheckboxWin32(int subType);


	// From SHVControlImplementer
	virtual int GetSubType(SHVControl* owner);

	virtual SHVBool Create(SHVControl* owner, SHVControlImplementer* parent, int flags);


	// From SHVControlImplementerCheckbox
	virtual SHVStringBuffer GetText();
	virtual void SetText(SHVControlCheckbox* owner, const SHVStringC& text, bool autoSize);

	virtual SHVInt GetChecked();
	virtual void SetChecked(SHVControlCheckbox* owner, SHVInt state);


private:
	///\cond INTERNAL
	WNDPROC OrigProc;

	int SubType;


	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	///\endcond
};

#endif
