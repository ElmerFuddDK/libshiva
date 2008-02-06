#ifndef __SHIVA_WIN32GUI_SHVCONTROLIMPLEMENTERLABEL_H
#define __SHIVA_WIN32GUI_SHVCONTROLIMPLEMENTERLABEL_H

#include "../../include/shvcontrollabel.h"
#include "shvcontrolimplementerwin32.h"

#ifndef __SHIVA_WIN32
# error This code only works for win32
#elif defined(__SHIVA_MFC)
# error This code does not work in MFC mode
#endif


class SHVControlImplementerLabelWin32 : public SHVControlImplementerWin32<SHVControlImplementerLabel>
{
public:
	SHVControlImplementerLabelWin32();

	// From SHVControlImplementer
	virtual int GetSubType(SHVControl* owner);
	virtual SHVBool Create(SHVControl* owner, SHVControlImplementer* parent);

	// From SHVControlImplementerLabel
	virtual SHVStringBuffer GetText();
	virtual void SetText(const SHVStringC& text);

};




#endif
