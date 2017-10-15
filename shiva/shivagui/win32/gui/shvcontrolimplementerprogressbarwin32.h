#ifndef __SHIVA_WIN32GUI_SHVCONTROLIMPLEMENTERPROGRESSBAR_H
#define __SHIVA_WIN32GUI_SHVCONTROLIMPLEMENTERPROGRESSBAR_H

#include "../../../include/gui/shvcontrolprogressbar.h"
#include "shvcontrolimplementerwin32.h"

#ifndef __SHIVA_WIN32
# error This code only works for win32
#elif defined(__SHIVA_MFC)
# error This code does not work in MFC mode
#endif


class SHVControlImplementerProgressBarWin32 : public SHVControlImplementerWin32<SHVControlImplementerProgressBar>
{
public:
	SHVControlImplementerProgressBarWin32();

	// From SHVControlImplementer
	virtual int GetSubType(SHVControl* owner);
	virtual SHVBool Create(SHVControl* owner, SHVControlImplementer* parent, int flags);

	// From SHVControlImplementerProgressBar
	virtual void SetRange(SHVControlProgressBar* owner, int start, int end);
	virtual int GetRangeStart();
	virtual int GetRangeEnd();

	virtual void SetProgress(SHVControlProgressBar* owner, int progress);
	virtual void IncrementProgress(SHVControlProgressBar* owner, int by);
	virtual int GetProgress();


private:

	///\cond INTERNAL
	SHVInt RangeStart, RangeEnd, Progress;
	///\endcond
};

#endif
