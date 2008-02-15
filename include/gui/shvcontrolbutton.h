#ifndef __SHIVA_GUI_CONTROLBUTTON_H
#define __SHIVA_GUI_CONTROLBUTTON_H

#include "../include/shvcontrol.h"


// forward declare
class SHVControlImplementerButton;


//-=========================================================================================================
/// SHVControlButton - base button control class
/**
 */

class SHVControlButton : public SHVControl
{
public:


	enum Events {
		EventClicked = EventSubClass
	};


	// constructor
	inline SHVControlButton(SHVGUIManager* manager, SHVControlImplementer* implementor);
	virtual ~SHVControlButton();


	virtual int GetType();

	inline SHVControlButton* SetParent(SHVControlContainer* parent, int flags = FlagVisible);

	// Data handling
	virtual SHVBool GetData(SHVControlData* data);
	virtual SHVBool SetData(SHVControlData* data);

	inline SHVStringBuffer GetText();
	inline SHVControlButton* SetText(const SHVStringC& title, bool autoSize = true);


	// Perform click event
	virtual void PerformClicked();
	virtual void SubscribeClicked(SHVEventSubscriberBase* subscriber);


	// obtain pointer to the implementor
	inline SHVControlImplementerButton* GetImplementor();


private:

	///\cond INTERNAL
	SHVEventSubscriberBaseRef Clicked;
	///\endcond
};
typedef SHVRefObjectContainer<SHVControlButton> SHVControlButtonRef;


//-=========================================================================================================
/// SHVControlImplementerButton - base button control implementerclass
/**
 */

class SHVControlImplementerButton : public SHVControlImplementer
{
public:

	// Properties
	virtual SHVStringBuffer GetText() = 0;
	virtual void SetText(SHVControlButton* owner, const SHVStringC& text, bool autoSize) = 0;

};


// ============================================= implementation ============================================= //


/*************************************
 * Constructor
 *************************************/
SHVControlButton::SHVControlButton(SHVGUIManager* manager, SHVControlImplementer* implementor)
  : SHVControl(manager,implementor)
{}

/*************************************
 * SetParent
 *************************************/
SHVControlButton* SHVControlButton::SetParent(SHVControlContainer* parent, int flags)
{
	SHVControl::SetParent(parent,flags);
	return this;
}

/*************************************
 * GetText
 *************************************/
SHVStringBuffer SHVControlButton::GetText()
{
	return GetImplementor()->GetText();
}

/*************************************
 * SetText
 *************************************/
SHVControlButton* SHVControlButton::SetText(const SHVStringC& title, bool autoSize)
{
	GetImplementor()->SetText(this,title,autoSize);
	return this;
}

/*************************************
 * GetImplementor
 *************************************/
SHVControlImplementerButton* SHVControlButton::GetImplementor()
{
	return (SHVControlImplementerButton*)(SHVControlImplementer*)Implementor;
}


#endif
