#ifndef __SHIVA_GUI_CONTROLLABEL_H
#define __SHIVA_GUI_CONTROLLABEL_H

#include "../include/shvcontrol.h"

// forward declares
class SHVControlImplementerLabel;

//-=========================================================================================================
/// SHVControlLabel - label control
/**
 * Simple label control 
 */


class SHVControlLabel: public SHVControl
{
public:
	virtual int GetType();

	virtual SHVBool GetData(SHVControlData* data);
	virtual SHVBool SetData(SHVControlData* data);
	inline void SetText(const SHVStringC& text);
	inline SHVStringBuffer GetText();

	inline SHVControlImplementerLabel* GetImplementor();
};
typedef SHVRefObjectContainer<SHVControlLabel> SHVControlLabelRef;


//-=========================================================================================================
/// SHVControlImplementerLabel - label implementer
/**
 * Interface that has to be implemented for the specific platform
 */

class SHVControlImplementerLabel : public SHVControlImplementer
{
public:

	virtual SHVStringBuffer GetText() = 0;
	virtual void SetText(const SHVStringC& title) = 0;

};


/*************************************
 * GetText
 *************************************/
SHVStringBuffer SHVControlLabel::GetText()
{
	return GetImplementor()->GetText();
}

/*************************************
 * SetText
 *************************************/
void SHVControlLabel::SetText(const SHVStringC& title)
{
	GetImplementor()->SetText(title);
}


/*************************************
 * GetImplementor
 *************************************/
SHVControlImplementerLabel* SHVControlLabel::GetImplementor()
{
	return (SHVControlImplementerLabel*)(SHVControlImplementer*)Implementor;
}

#endif