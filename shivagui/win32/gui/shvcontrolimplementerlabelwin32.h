#ifndef __SHIVA_WIN32GUI_SHVCONTROLIMPLEMENTERLABEL_H
#define __SHIVA_WIN32GUI_SHVCONTROLIMPLEMENTERLABEL_H

#include "../../../include/gui/shvcontrollabel.h"
#include "shvcontrolimplementerwin32.h"

#ifndef __SHIVA_WIN32
# error This code only works for win32
#elif defined(__SHIVA_MFC)
# error This code does not work in MFC mode
#endif


class SHVControlImplementerLabelWin32 : public SHVControlImplementerWin32<SHVControlImplementerLabelCustomDraw>
{
public:
	SHVControlImplementerLabelWin32(int subType);

	// From SHVControlImplementer
	virtual int GetSubType(SHVControl* owner);
	virtual SHVBool Create(SHVControl* owner, SHVControlImplementer* parent, int flags);

	// From SHVControlImplementerLabel
	virtual SHVStringBuffer GetText();
	virtual void SetText(SHVControlLabel* owner, const SHVStringC& text, bool autoSize);


	// From SHVControlImplementerLabelCustomDraw
	virtual void SubscribeDraw(SHVEventSubscriberBase* subscriber);

private:

	///\cond INTERNAL
	int SubType;
	SHVEventSubscriberBaseRef Subscriber;
	WNDPROC OrigProc;

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	///\endcond
};

#endif
