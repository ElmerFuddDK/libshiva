#ifndef __SHIVA_GUI_CONTROLPROGRESSBAR_H
#define __SHIVA_GUI_CONTROLPROGRESSBAR_H

#include "shvcontrol.h"

// forward declares
class SHVControlImplementerProgressBar;

//-=========================================================================================================
/// SHVControlProgressBar - progress bar control
/**
 * Simple progress bar control
 */


class SHVControlProgressBar : public SHVControl
{
public:


	inline SHVControlProgressBar(SHVGUIManager* manager, SHVControlImplementer* implementor);

	virtual int GetType();

	inline SHVControlProgressBar* SetParent(SHVControlContainer* parent, int flags = FlagVisible);

	virtual SHVBool GetData(SHVDataBinder* data);
	virtual SHVBool SetData(SHVDataBinder* data);

	inline SHVControlProgressBar* SetRange(int start, int end);
	inline int GetRangeStart();
	inline int GetRangeEnd();

	inline SHVControlProgressBar* SetProgress(int progress);
	inline SHVControlProgressBar* IncrementProgress(int by = 1);
	inline int GetProgress();

	inline SHVControlImplementerProgressBar* GetImplementor();
};
typedef SHVRefObjectContainer<SHVControlProgressBar> SHVControlProgressBarRef;



//-=========================================================================================================
/// SHVControlImplementerProgressBar - progress bar implementer
/**
 * Interface that has to be implemented for the specific platform
 */

class SHVControlImplementerProgressBar : public SHVControlImplementer
{
public:

	virtual void SetRange(SHVControlProgressBar* owner, int start, int end) = 0;
	virtual int GetRangeStart() = 0;
	virtual int GetRangeEnd() = 0;

	virtual void SetProgress(SHVControlProgressBar* owner, int progress) = 0;
	virtual void IncrementProgress(SHVControlProgressBar* owner, int by) = 0;
	virtual int GetProgress() = 0;

};



// ============================================= implementation ============================================= //


/*************************************
 * Constructor
 *************************************/
SHVControlProgressBar::SHVControlProgressBar(SHVGUIManager* manager, SHVControlImplementer* implementor)
  : SHVControl(manager,implementor)
{}


/*************************************
 * SetParent
 *************************************/
SHVControlProgressBar* SHVControlProgressBar::SetParent(SHVControlContainer* parent, int flags)
{
	SHVControl::SetParent(parent,flags);
	return this;
}

/*************************************
 * SetRange
 *************************************/
SHVControlProgressBar* SHVControlProgressBar::SetRange(int start, int end)
{
	GetImplementor()->SetRange(this,start,end);
	return this;
}

/*************************************
 * GetRangeStart
 *************************************/
int SHVControlProgressBar::GetRangeStart()
{
	return GetImplementor()->GetRangeStart();
}

/*************************************
 * GetRangeEnd
 *************************************/
int SHVControlProgressBar::GetRangeEnd()
{
	return GetImplementor()->GetRangeEnd();
}

/*************************************
 * SetProgress
 *************************************/
SHVControlProgressBar* SHVControlProgressBar::SetProgress(int progress)
{
	GetImplementor()->SetProgress(this,progress);
	return this;
}

/*************************************
 * IncrementProgress
 *************************************/
SHVControlProgressBar* SHVControlProgressBar::IncrementProgress(int by)
{
	GetImplementor()->IncrementProgress(this,by);
	return this;
}

/*************************************
 * GetProgress
 *************************************/
int SHVControlProgressBar::GetProgress()
{
	return GetImplementor()->GetProgress();
}


/*************************************
 * GetImplementor
 *************************************/
SHVControlImplementerProgressBar* SHVControlProgressBar::GetImplementor()
{
	return (SHVControlImplementerProgressBar*)(SHVControlImplementer*)Implementor;
}

#endif
