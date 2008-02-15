#ifndef __SHIVA_WIN32GUI_CONTROLIMPLEMENTERBASE_H
#define __SHIVA_WIN32GUI_CONTROLIMPLEMENTERBASE_H


#include "../../include/shvcontrolimplementer.h"

#ifndef __SHIVA_WIN32
# error This code only works for win32
#elif defined(__SHIVA_MFC)
# error This code does not work in MFC mode
#endif

// Redefine SetWindowLongPtr and GetWindowLongPtr to avoid stupid warnings on win32
#ifndef _WIN64
# undef SetWindowLongPtr
# undef GetWindowLongPtr
# define SetWindowLongPtr(x,y,z) SetWindowLong(x,y,(LONG)z)
# define GetWindowLongPtr(x,y) (LONG_PTR)GetWindowLong(x,y)
#endif


//-=========================================================================================================
/// SHVControlImplementerWin32Base - implementation of base implementer methods
/**
 */

class SHVControlImplementerWin32Base
{
public:


	SHVControlImplementerWin32Base();
	virtual ~SHVControlImplementerWin32Base();


	virtual SHVBool IsCreated();
	virtual SHVBool Reparent(SHVControl* owner, SHVControlImplementer* parent, int flags);
	virtual SHVBool Destroy(SHVControl* owner);

	virtual SHVRect GetRect(SHVControl* owner);
	virtual void SetRect(SHVControl* owner, const SHVRect& rect);

	virtual SHVBool SetFlag(SHVControl* owner, int flag, bool enable = true);
	virtual bool GetFlag(SHVControl* owner, int flag);

	virtual SHVFont* GetFont(SHVControl* owner);
	virtual SHVBool SetFont(SHVControl* owner, SHVFont* font, SHVInt newHeight);


	virtual HWND GetHandle();

	SHVStringBuffer GetText();
	void SetText(const SHVStringC& text);

protected:

	virtual void SetHandle(HWND handle);

private:

	HWND Window;

};


#endif
