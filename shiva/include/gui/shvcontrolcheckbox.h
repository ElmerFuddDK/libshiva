#ifndef __SHIVA_GUI_CONTROLCHECKBOX_H
#define __SHIVA_GUI_CONTROLCHECKBOX_H

#include "shvcontrol.h"


// forward declare
class SHVControlImplementerCheckbox;


//-=========================================================================================================
/// SHVControlCheckbox - base checkbox control class
/**
 */

class SHVControlCheckbox : public SHVControl
{
public:


	enum Events {
		EventChanged = EventSubClass
	};

	enum SubTypes {
		SubTypeNormal = SubTypeDefault,
		SubTypeTristate
	};

	enum States {
		StateUnchecked = 0,
		StateChecked = 1
	};


	// constructor
	inline SHVControlCheckbox(SHVGUIManager* manager, SHVControlImplementer* implementor);
	virtual ~SHVControlCheckbox();


	virtual int GetType();

	inline SHVControlCheckbox* SetParent(SHVControlContainer* parent, int flags = FlagVisible);

	// Data handling
	virtual SHVBool GetData(SHVDataBinder* data);
	virtual SHVBool SetData(SHVDataBinder* data);

	inline SHVStringBuffer GetText();
	inline SHVControlCheckbox* SetText(const SHVStringC& title, bool autoSize = true);

	inline SHVInt GetChecked();
	inline SHVControlCheckbox* SetChecked(SHVInt state);


	// Perform changed event
	virtual void PerformChanged();
	virtual void SubscribeChanged(SHVEventSubscriberBase* subscriber);


	// obtain pointer to the implementor
	inline SHVControlImplementerCheckbox* GetImplementor();


private:

	///\cond INTERNAL
	SHVEventSubscriberBaseRef Changed;
	///\endcond
};
typedef SHVRefObjectContainer<SHVControlCheckbox> SHVControlCheckboxRef;


//-=========================================================================================================
/// SHVControlImplementerCheckbox - base checkbox control implementerclass
/**
 */

class SHVControlImplementerCheckbox : public SHVControlImplementer
{
public:

	// Properties
	virtual SHVStringBuffer GetText() = 0;
	virtual void SetText(SHVControlCheckbox* owner, const SHVStringC& text, bool autoSize) = 0;

	virtual SHVInt GetChecked() = 0;
	virtual void SetChecked(SHVControlCheckbox* owner, SHVInt state) = 0;

};


// ============================================= implementation ============================================= //


/*************************************
 * Constructor
 *************************************/
SHVControlCheckbox::SHVControlCheckbox(SHVGUIManager* manager, SHVControlImplementer* implementor)
  : SHVControl(manager,implementor)
{}

/*************************************
 * SetParent
 *************************************/
SHVControlCheckbox* SHVControlCheckbox::SetParent(SHVControlContainer* parent, int flags)
{
	SHVControl::SetParent(parent,flags);
	return this;
}

/*************************************
 * GetText
 *************************************/
SHVStringBuffer SHVControlCheckbox::GetText()
{
	return GetImplementor()->GetText();
}

/*************************************
 * SetText
 *************************************/
SHVControlCheckbox* SHVControlCheckbox::SetText(const SHVStringC& title, bool autoSize)
{
	GetImplementor()->SetText(this,title,autoSize);
	return this;
}

/*************************************
 * GetChecked
 *************************************/
SHVInt SHVControlCheckbox::GetChecked()
{
	return GetImplementor()->GetChecked();
}

/*************************************
 * SetChecked
 *************************************/
SHVControlCheckbox* SHVControlCheckbox::SetChecked(SHVInt state)
{
	GetImplementor()->SetChecked(this,state);
	return this;
}

/*************************************
 * GetImplementor
 *************************************/
SHVControlImplementerCheckbox* SHVControlCheckbox::GetImplementor()
{
	return (SHVControlImplementerCheckbox*)(SHVControlImplementer*)Implementor;
}


#endif
