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


	enum SubTypes {
		SubTypeNormal = SubTypeDefault,
		SubTypeCustomDraw
	};


	inline SHVControlLabel(SHVGUIManager* manager, SHVControlImplementer* implementor);

	virtual int GetType();

	inline SHVControlLabel* SetParent(SHVControlContainer* parent, int flags = FlagVisible);

	virtual SHVBool GetData(SHVControlData* data);
	virtual SHVBool SetData(SHVControlData* data);

	inline SHVControlLabel* SetText(const SHVStringC& text, bool autoSize = true);
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
	virtual void SetText(SHVControlLabel* owner, const SHVStringC& title, bool autoSize) = 0;

};



//-=========================================================================================================
/// SHVControlImplementerLabelCustomDraw - label implementer for custom drawn labels
/**
 * Interface that has to be implemented for the specific platform
 */

class SHVControlImplementerLabelCustomDraw : public SHVControlImplementerLabel
{
public:

	virtual void SubscribeDraw(SHVEventSubscriberBase* subscriber) = 0;

};



// ============================================= implementation ============================================= //


/*************************************
 * Constructor
 *************************************/
SHVControlLabel::SHVControlLabel(SHVGUIManager* manager, SHVControlImplementer* implementor)
  : SHVControl(manager,implementor)
{}


/*************************************
 * SetParent
 *************************************/
SHVControlLabel* SHVControlLabel::SetParent(SHVControlContainer* parent, int flags)
{
	SHVControl::SetParent(parent,flags);
	return this;
}

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
SHVControlLabel* SHVControlLabel::SetText(const SHVStringC& title, bool autoSize)
{
	GetImplementor()->SetText(this,title,autoSize);
	return this;
}


/*************************************
 * GetImplementor
 *************************************/
SHVControlImplementerLabel* SHVControlLabel::GetImplementor()
{
	return (SHVControlImplementerLabel*)(SHVControlImplementer*)Implementor;
}

#endif
