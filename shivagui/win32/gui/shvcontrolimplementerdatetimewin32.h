#ifndef __SHIVA_WIN32GUI_SHVCONTROLIMPLEMENTERDATETIME_H
#define __SHIVA_WIN32GUI_SHVCONTROLIMPLEMENTERDATETIME_H


#include "../../../include/gui/shvcontroldatetime.h"
#include "shvcontrolimplementerwin32.h"

#ifndef __SHIVA_WIN32
# error This code only works for win32
#elif defined(__SHIVA_MFC)
# error This code does not work in MFC mode
#endif


//-=========================================================================================================
/// SHVControlImplementerDateTimeWin32 - date time implementation
/**
 */

class SHVControlImplementerDateTimeWin32 : public SHVControlImplementerWin32<SHVControlImplementerDateTime>
{
public:


	SHVControlImplementerDateTimeWin32(int subType);


	// From SHVControlImplementer
	virtual int GetSubType(SHVControl* owner);

	virtual SHVBool Create(SHVControl* owner, SHVControlImplementer* parent, int flags);


	// From SHVControlImplementerDateTime
	virtual SHVTime GetTime(SHVControlDateTime* owner);
	virtual void SetTime(SHVControlDateTime* owner, const SHVTime& time);


private:
	///\cond INTERNAL
	int SubType;

	WNDPROC OrigProc;

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	///\endcond
};

#endif
