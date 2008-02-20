#ifndef __SHIVA_GUI_CONTROLCONTAINER_H
#define __SHIVA_GUI_CONTROLCONTAINER_H

#include "shvcontrollayout.h"
#include "../../include/utils/shvvector.h"
#include "../../include/framework/shvconfig.h"
#include "utils/shvcolor.h"
#include "utils/shvpoint.h"


// forward declare
class SHVControlImplementerContainer;


//-=========================================================================================================
/// SHVControlContainer - base control container class
/**
 * Inherit from this class when you want your control to implement a control that is able to contain other
 * controls.
 */

class SHVControlContainer : public SHVControl
{
public:


	// defines and constants
	enum SubTypes {
		SubTypeWindow = SHVControl::SubTypeDefault,
		SubTypeCustomDraw,
		SubTypeMainWindow,
		SubTypeDialog,
	};
	enum EventTypes
	{
		EventPreDestroy
	};


	// construcor
	inline SHVControlContainer(SHVGUIManager* manager, SHVControlImplementer* implementor);
	virtual ~SHVControlContainer();


	virtual int GetType();

	inline SHVControlContainer* SetParent(SHVControlContainer* parent, int flags = FlagVisible);


	// Create top level window
	virtual SHVBool Create();


	// Data handling
	virtual SHVBool GetData(SHVControlData* data);
	virtual SHVBool SetData(SHVControlData* data);

	inline SHVStringBuffer GetTitle();
	inline SHVControlContainer* SetTitle(const SHVStringC& title);

	inline SHVControlContainer* SetMinimumSize(int widthInChars, int heightInChars);
	inline SHVPoint GetMinimumSizeInPixels();

	inline SHVColor* GetColor();
	inline SHVControlContainer* SetColor(SHVColor* color);

	virtual void SetLayoutEngine(SHVControlLayout* engine);
	virtual SHVControlLayout* GetLayoutEngine();


	// Obtaining the rectangle defining the region controls can be placed in
	inline SHVRect GetRegionRect();
	

	// control handling
	virtual void Clear();
	virtual SHVBool ContainsControl(SHVControl* cntrl);
	inline void ResizeControls();
	inline size_t CalculateControlCount();
	inline SHVControl* GetControl(size_t index);

	// event handling
	virtual SHVBool PreDestroy();
	virtual void SubscribePreDestroy(SHVEventSubscriberBase* subs);


	// obtain pointer to the implementor
	inline SHVControlImplementerContainer* GetImplementor();


private:
friend class SHVControl;

	virtual SHVBool AddControl(SHVControl* cntrl);
	virtual SHVBool RemoveControl(SHVControl* cntrl);
	
	
	///\cond INTERNAL
	SHVControlLayoutRef LayoutEngine;
	SHVVectorBase Controls;
	SHVEventSubscriberBaseRef PreDestroySubscriber;
	enum { ControlGrowSize = 10 };
	///\endcond
};
typedef SHVRefObjectContainer<SHVControlContainer> SHVControlContainerRef;


//-=========================================================================================================
/// SHVControlImplementerContainer - base control container implementerclass
/**
 */

class SHVControlImplementerContainer : public SHVControlImplementer
{
public:

	// Properties
	virtual SHVRect GetRegionRect() = 0;

	virtual SHVStringBuffer GetTitle() = 0;
	virtual void SetTitle(const SHVStringC& title) = 0;

	virtual SHVColor* GetColor(SHVControlContainer* owner) = 0;
	virtual void SetColor(SHVControlContainer* owner, SHVColor* color) = 0;

	virtual void SetMinimumSize(SHVControlContainer* owner, int widthInChars, int heightInChars) = 0;
	virtual SHVPoint GetMinimumSizeInPixels(SHVControlContainer* owner) = 0;

};



//-=========================================================================================================
/// SHVControlImplementerContainerCustomDraw - container implementer for custom drawn container windows
/**
 * Interface that has to be implemented for the specific platform
 */

class SHVControlImplementerContainerCustomDraw : public SHVControlImplementerContainer
{
public:

	virtual void SubscribeDraw(SHVEventSubscriberBase* subscriber) = 0;

};




// ============================================= implementation ============================================= //


/*************************************
 * Constructor
 *************************************/
SHVControlContainer::SHVControlContainer(SHVGUIManager* manager, SHVControlImplementer* implementor)
  : SHVControl(manager,implementor)
{}

/*************************************
 * SetParent
 *************************************/
SHVControlContainer* SHVControlContainer::SetParent(SHVControlContainer* parent, int flags)
{
	SHVControl::SetParent(parent,flags);
	return this;
}

/*************************************
 * GetTitle
 *************************************/
SHVStringBuffer SHVControlContainer::GetTitle()
{
	return GetImplementor()->GetTitle();
}

/*************************************
 * SetTitle
 *************************************/
SHVControlContainer* SHVControlContainer::SetTitle(const SHVStringC& title)
{
	GetImplementor()->SetTitle(title);
	return this;
}

/*************************************
 * SetMinimumSize
 *************************************/
SHVControlContainer* SHVControlContainer::SetMinimumSize(int widthInChars, int heightInChars)
{
	GetImplementor()->SetMinimumSize(this,widthInChars,heightInChars);
	return this;
}

/*************************************
 * SetMinimumSize
 *************************************/
SHVPoint SHVControlContainer::GetMinimumSizeInPixels()
{
	return GetImplementor()->GetMinimumSizeInPixels(this);
}

/*************************************
 * GetColor
 *************************************/
SHVColor* SHVControlContainer::GetColor()
{
SHVColor* retVal = GetImplementor()->GetColor(this);

	if (!retVal && GetParent())
		retVal = GetParent()->GetColor();

	return retVal;
}

/*************************************
 * SetColor
 *************************************/
SHVControlContainer* SHVControlContainer::SetColor(SHVColor* color)
{
	GetImplementor()->SetColor(this,color);
	return this;
}


/*************************************
 * GetRegionRect
 *************************************/
SHVRect SHVControlContainer::GetRegionRect()
{
	return GetImplementor()->GetRegionRect();
}

/*************************************
 * ResizeControls
 *************************************/
void SHVControlContainer::ResizeControls()
{
	if (!LayoutEngine.IsNull())
		LayoutEngine->ResizeControls(this);
}

/*************************************
 * CalculateControlCount
 *************************************/
/// Calculates the amount of controls in the container (quick)
size_t SHVControlContainer::CalculateControlCount()
{
	return Controls.CalculateCount();
}

/*************************************
 * GetControl
 *************************************/
/// Returns a control in the control list at a given index
/**
 * The controls are contained in a compressed vector. This means
 * the index can change and the iteration process is not thread
 * safe. The index will change if you remove a control before
 * the index.
 */
SHVControl* SHVControlContainer::GetControl(size_t index)
{
	return (SHVControl*)Controls[index];
}

/*************************************
 * GetImplementor
 *************************************/
SHVControlImplementerContainer* SHVControlContainer::GetImplementor()
{
	return (SHVControlImplementerContainer*)(SHVControlImplementer*)Implementor;
}


#endif
