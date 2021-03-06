#ifndef __SHIVA_GUI_CONTROLTAB_H
#define __SHIVA_GUI_CONTROLTAB_H

#include "shvcontrol.h"


// forward declare
class SHVControlImplementerTab;
class SHVFormBase;


//-=========================================================================================================
/// SHVControlTab - base tab control class
/**
 */

class SHVControlTab : public SHVControl
{
public:

	
	enum Events {
		EventSelectedPageChanged = EventSubClass
	};


	// constructor
	inline SHVControlTab(SHVGUIManager* manager, SHVControlImplementer* implementor);
	virtual ~SHVControlTab();


	virtual int GetType();

	inline SHVControlTab* SetParent(SHVControlContainer* parent, int flags = FlagVisible);


	// Data handling
	virtual SHVBool GetData(SHVDataBinder* data);
	virtual SHVBool SetData(SHVDataBinder* data);

	inline SHVControlContainer* AddPage(const SHVStringC name, size_t atIndex = SIZE_T_MAX);
	inline SHVControlContainer* GetPage(size_t index);
	inline void RemovePage(size_t index);
	inline void RemovePageByContainer(SHVControlContainer* container);
	inline void Clear();

	inline size_t GetSelectedPage();
	inline void SelectPage(size_t index);

	inline void SetPageForm(size_t index, SHVFormBase* form);
	inline SHVFormBase* GetPageForm(size_t index);

	inline SHVStringBuffer GetPageName(size_t index);

	inline size_t GetPageCount();


	// Perform selected page event
	virtual void PerformSelectedPageChanged();
	virtual void SubscribeSelectedPageChanged(SHVEventSubscriberBase* subscriber);


	// obtain pointer to the implementor
	inline SHVControlImplementerTab* GetImplementor();


private:

	///\cond INTERNAL
	SHVEventSubscriberBaseRef SelectedPageChanged;
	///\endcond
};
typedef SHVRefObjectContainer<SHVControlTab> SHVControlTabRef;


//-=========================================================================================================
/// SHVControlImplementerTab - base button control implementerclass
/**
 */

class SHVControlImplementerTab : public SHVControlImplementer
{
public:

	// Properties
	virtual SHVControlContainer* AddPage(SHVControlTab* owner, const SHVStringC name, size_t atIndex = SIZE_T_MAX) = 0;
	virtual SHVControlContainer* GetPage(SHVControlTab* owner, size_t index) = 0;
	virtual void RemovePage(SHVControlTab* owner, size_t index) = 0;
	virtual void RemovePageByContainer(SHVControlTab* owner, SHVControlContainer* container) = 0;
	virtual void Clear(SHVControlTab* owner) = 0;

	virtual size_t GetSelectedPage(SHVControlTab* owner) = 0;
	virtual void SelectPage(SHVControlTab* owner, size_t index) = 0;

	virtual void SetPageForm(SHVControlTab* owner, size_t index, SHVFormBase* form) = 0;
	virtual SHVFormBase* GetPageForm(SHVControlTab* owner, size_t index) = 0;

	virtual SHVStringBuffer GetPageName(SHVControlTab* owner, size_t index) = 0;

	virtual size_t GetPageCount(SHVControlTab* owner) = 0;

};


// ============================================= implementation ============================================= //


/*************************************
 * Constructor
 *************************************/
SHVControlTab::SHVControlTab(SHVGUIManager* manager, SHVControlImplementer* implementor)
  : SHVControl(manager,implementor)
{}

/*************************************
 * SetParent
 *************************************/
SHVControlTab* SHVControlTab::SetParent(SHVControlContainer* parent, int flags)
{
	SHVControl::SetParent(parent,flags);
	return this;
}

/*************************************
 * AddPage
 *************************************/
SHVControlContainer* SHVControlTab::AddPage(const SHVStringC name, size_t atIndex)
{
	return GetImplementor()->AddPage(this,name,atIndex);
}

/*************************************
 * GetPage
 *************************************/
SHVControlContainer* SHVControlTab::GetPage(size_t index)
{
	return GetImplementor()->GetPage(this,index);
}

/*************************************
 * RemovePage
 *************************************/
void SHVControlTab::RemovePage(size_t index)
{
	GetImplementor()->RemovePage(this,index);
}

/*************************************
 * RemovePageByContainer
 *************************************/
void SHVControlTab::RemovePageByContainer(SHVControlContainer* container)
{
	GetImplementor()->RemovePageByContainer(this,container);
}

/*************************************
 * Clear
 *************************************/
void SHVControlTab::Clear()
{
	GetImplementor()->Clear(this);
}

/*************************************
 * GetSelectedPage
 *************************************/
size_t SHVControlTab::GetSelectedPage()
{
	return GetImplementor()->GetSelectedPage(this);
}

/*************************************
 * SelectPage
 *************************************/
void SHVControlTab::SelectPage(size_t index)
{
	GetImplementor()->SelectPage(this,index);
}

/*************************************
 * SetPageForm
 *************************************/
void SHVControlTab::SetPageForm(size_t index, SHVFormBase* form)
{
	GetImplementor()->SetPageForm(this,index,form);
}

/*************************************
 * GetPageForm
 *************************************/
SHVFormBase* SHVControlTab::GetPageForm(size_t index)
{
	return GetImplementor()->GetPageForm(this,index);
}

/*************************************
 * GetPageName
 *************************************/
SHVStringBuffer SHVControlTab::GetPageName(size_t index)
{
	return GetImplementor()->GetPageName(this,index);
}

/*************************************
 * GetPageCount
 *************************************/
size_t SHVControlTab::GetPageCount()
{
	return GetImplementor()->GetPageCount(this);
}

/*************************************
 * GetImplementor
 *************************************/
SHVControlImplementerTab* SHVControlTab::GetImplementor()
{
	return (SHVControlImplementerTab*)(SHVControlImplementer*)Implementor;
}


#endif
