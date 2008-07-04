#ifndef __SHIVA_GUI_CONTROLCOMBOBOX_H
#define __SHIVA_GUI_CONTROLCOMBOBOX_H

#include "shvcontrol.h"


// forward declare
class SHVControlImplementerComboBox;


//-=========================================================================================================
/// SHVControlComboBox - base combobox control class
/**
 */

class SHVControlComboBox : public SHVControl
{
public:


	// defines and constants
	enum SubTypes {
		SubTypeComboBox = SubTypeDefault,
		SubTypeComboBoxList
	};

	enum Events {
		EventSelectedChanged = EventSubClass
	};


	// constructor
	inline SHVControlComboBox(SHVGUIManager* manager, SHVControlImplementer* implementor);
	virtual ~SHVControlComboBox();


	virtual int GetType();

	inline SHVControlComboBox* SetParent(SHVControlContainer* parent, int flags = FlagVisible);

	// Data handling
	virtual SHVBool GetData(SHVDataBinder* data);
	virtual SHVBool SetData(SHVDataBinder* data);

	inline size_t GetItemCount();
	inline void ClearItems();
	inline SHVStringBuffer GetItemText(size_t index);
	inline SHVRefObject* GetItemData(size_t index);
	inline SHVControlComboBox* AddItem(const SHVStringC str, SHVRefObject* data = NULL);

	inline SHVStringBuffer GetText();
	inline SHVControlComboBox* SetText(const SHVStringC& text);

	inline SHVInt GetSelected();
	inline SHVControlComboBox* SetSelected(SHVInt index);

	inline SHVControlComboBox* SetDropdownHeight(int lines);


	// Perform changed event
	virtual void PerformSelectedChanged();
	virtual void SubscribeSelectedChanged(SHVEventSubscriberBase* subscriber);


	// obtain pointer to the implementor
	inline SHVControlImplementerComboBox* GetImplementor();


private:

	///\cond INTERNAL
	SHVEventSubscriberBaseRef SelectedChanged;
	///\endcond
};
typedef SHVRefObjectContainer<SHVControlComboBox> SHVControlComboBoxRef;


//-=========================================================================================================
/// SHVControlImplementerComboBox - base combobox control implementerclass
/**
 */

class SHVControlImplementerComboBox : public SHVControlImplementer
{
public:

	// Properties
	virtual SHVStringBuffer GetText(SHVControlComboBox* owner) = 0;
	virtual void SetText(SHVControlComboBox* owner, const SHVStringC& text) = 0;

	virtual SHVInt GetSelected(SHVControlComboBox* owner) = 0;
	virtual void SetSelected(SHVControlComboBox* owner, SHVInt index) = 0;

	virtual size_t GetItemCount(SHVControlComboBox* owner) = 0;
	virtual void ClearItems(SHVControlComboBox* owner) = 0;
	virtual SHVStringBuffer GetItemText(SHVControlComboBox* owner, size_t index) = 0;
	virtual SHVRefObject* GetItemData(SHVControlComboBox* owner, size_t index) = 0;
	virtual void AddItem(SHVControlComboBox* owner, const SHVStringC str, SHVRefObject* data = NULL) = 0;

	virtual void SetDropdownHeight(SHVControlComboBox* owner, int lines) = 0;
};


// ============================================= implementation ============================================= //


/*************************************
 * Constructor
 *************************************/
SHVControlComboBox::SHVControlComboBox(SHVGUIManager* manager, SHVControlImplementer* implementor)
  : SHVControl(manager,implementor)
{}

/*************************************
 * SetParent
 *************************************/
SHVControlComboBox* SHVControlComboBox::SetParent(SHVControlContainer* parent, int flags)
{
	SHVControl::SetParent(parent,flags);
	return this;
}

/*************************************
 * GetText
 *************************************/
SHVStringBuffer SHVControlComboBox::GetText()
{
	return GetImplementor()->GetText(this);
}

/*************************************
 * SetText
 *************************************/
SHVControlComboBox* SHVControlComboBox::SetText(const SHVStringC& title)
{
	GetImplementor()->SetText(this,title);
	return this;
}

/*************************************
 * GetSelected
 *************************************/
SHVInt SHVControlComboBox::GetSelected()
{
	return GetImplementor()->GetSelected(this);
}

/*************************************
 * SetSelected
 *************************************/
SHVControlComboBox* SHVControlComboBox::SetSelected(SHVInt index)
{
	GetImplementor()->SetSelected(this,index);
	return this;
}

/*************************************
 * SetDropdownHeight
 *************************************/
SHVControlComboBox* SHVControlComboBox::SetDropdownHeight(int lines)
{
	GetImplementor()->SetDropdownHeight(this,lines);
	return this;
}

/*************************************
 * GetItemCount
 *************************************/
size_t SHVControlComboBox::GetItemCount()
{
	return GetImplementor()->GetItemCount(this);
}

/*************************************
 * ClearItems
 *************************************/
void SHVControlComboBox::ClearItems()
{
	GetImplementor()->ClearItems(this);
}

/*************************************
 * GetItemText
 *************************************/
SHVStringBuffer SHVControlComboBox::GetItemText(size_t index)
{
	return GetImplementor()->GetItemText(this,index);
}

/*************************************
 * GetItemData
 *************************************/
SHVRefObject* SHVControlComboBox::GetItemData(size_t index)
{
	return GetImplementor()->GetItemData(this,index);
}

/*************************************
 * AddItem
 *************************************/
SHVControlComboBox* SHVControlComboBox::AddItem(const SHVStringC str, SHVRefObject* data)
{
	GetImplementor()->AddItem(this,str,data);
	return this;
}

/*************************************
 * GetImplementor
 *************************************/
SHVControlImplementerComboBox* SHVControlComboBox::GetImplementor()
{
	return (SHVControlImplementerComboBox*)(SHVControlImplementer*)Implementor;
}


#endif
