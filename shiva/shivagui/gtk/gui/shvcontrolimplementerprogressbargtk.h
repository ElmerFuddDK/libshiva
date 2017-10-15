#ifndef __SHIVA_GTKGUI_SHVCONTROLIMPLEMENTERPROGRESSBAR_H
#define __SHIVA_GTKGUI_SHVCONTROLIMPLEMENTERPROGRESSBAR_H

#include "../../../include/gui/shvcontrolprogressbar.h"
#include "shvcontrolimplementergtkwidget.h"



//-=========================================================================================================
/// SHVControlImplementerProgressBarGtk
/**
 * Implementation of a progress bar implementer for GTK
 */

class SHVControlImplementerProgressBarGtk : public SHVControlImplementerGtkWidget<SHVControlImplementerProgressBar>
{
public:

	SHVControlImplementerProgressBarGtk();

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
