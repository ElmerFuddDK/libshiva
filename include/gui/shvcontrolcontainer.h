#ifndef __SHIVA_GUI_CONTROLCONTAINER_H
#define __SHIVA_GUI_CONTROLCONTAINER_H

#include "shvcontrollayout.h"
#include "../../include/utils/shvvector.h"
#include "../../include/framework/shvconfig.h"


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
		SubTypeMainWindow,
		SubTypeDialog,
	};


	// construcor
	inline SHVControlContainer(SHVGUIManager* manager, SHVControlImplementer* implementor);
	virtual ~SHVControlContainer();


	virtual int GetType();


	// Create top level window
	virtual SHVBool Create();


	// Data handling
	virtual SHVBool GetData(SHVControlData* data);
	virtual SHVBool SetData(SHVControlData* data);

	inline SHVStringBuffer GetTitle();
	inline void SetTitle(const SHVStringC& title);

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


	// obtain pointer to the implementor
	inline SHVControlImplementerContainer* GetImplementor();


private:
friend class SHVControl;

	virtual SHVBool AddControl(SHVControl* cntrl);
	virtual SHVBool RemoveControl(SHVControl* cntrl);
	
	
	///\cond INTERNAL
	SHVControlLayoutRef LayoutEngine;
	SHVVectorBase Controls;
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

};


// ============================================= implementation ============================================= //


/*************************************
 * Constructor
 *************************************/
SHVControlContainer::SHVControlContainer(SHVGUIManager* manager, SHVControlImplementer* implementor)
  : SHVControl(manager,implementor)
{}

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
void SHVControlContainer::SetTitle(const SHVStringC& title)
{
	GetImplementor()->SetTitle(title);
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
