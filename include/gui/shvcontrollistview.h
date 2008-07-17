#ifndef __SHIVA_GUI_CONTROLLISTVIEW_H
#define __SHIVA_GUI_CONTROLLISTVIEW_H

#include "shvcontrol.h"
#include "utils/shvdraw.h"


// forward declare
class SHVControlImplementerListView;
class SHVDrawEventDataListView;


//-=========================================================================================================
/// SHVControlListView - base listview control class
/**
 */

class SHVControlListView : public SHVControl
{
public:


	// defines and constants
	enum SubTypes {
		SubTypeNormal = SubTypeDefault,
		SubTypeCustomDraw
	};

	enum Events {
		EventDrawItem = EventSubClass,
		EventSelectedChanged
	};


	// constructor
	inline SHVControlListView(SHVGUIManager* manager, SHVControlImplementer* implementor);
	virtual ~SHVControlListView();


	virtual int GetType();

	inline SHVControlListView* SetParent(SHVControlContainer* parent, int flags = FlagVisible);

	// Data handling
	virtual SHVBool GetData(SHVDataBinder* data);
	virtual SHVBool SetData(SHVDataBinder* data);

	inline size_t GetItemCount();
	inline void ClearItems();
	inline SHVStringBuffer GetItemText(size_t index, size_t col = 0);
	inline SHVRefObject* GetItemData(size_t index);
	inline SHVControlListView* AddItem(const SHVStringC str, SHVRefObject* data = NULL);
	inline SHVControlListView* SetItemText(const SHVStringC text, size_t index, size_t col = 0);

	inline SHVControlListView* ClearColumns();
	inline SHVControlListView* AddColumn(const SHVStringC colName, SHVInt width = SHVInt());

	inline SHVInt GetSelected();
	inline SHVControlListView* SetSelected(SHVInt index);

	// Perform changed event
	virtual void PerformSelectedChanged();
	virtual void SubscribeSelectedChanged(SHVEventSubscriberBase* subscriber);


	// obtain pointer to the implementor
	inline SHVControlImplementerListView* GetImplementor();


	// obtain the list view specific draw event data
	static inline SHVDrawEventDataListView* DrawDataFromEvent(SHVEvent* event);


private:

	///\cond INTERNAL
	SHVEventSubscriberBaseRef SelectedChanged;
	///\endcond
};
typedef SHVRefObjectContainer<SHVControlListView> SHVControlListViewRef;



//-=========================================================================================================
/// SHVDrawEventDataListView - owner draw data specific to list view
/**
 */
class SHVDrawEventDataListView : SHVDrawEventData
{
public:

	inline SHVDrawEventDataListView(SHVDraw* draw);

	int RowID, ColumnID;
	bool IsSelected;
	SHVRect CellRect;

};


//-=========================================================================================================
/// SHVControlImplementerListView - base listview control implementerclass
/**
 */

class SHVControlImplementerListView : public SHVControlImplementer
{
public:

	// Properties
	virtual size_t GetItemCount(SHVControlListView* owner) = 0;
	virtual void ClearItems(SHVControlListView* owner) = 0;
	virtual SHVStringBuffer GetItemText(SHVControlListView* owner, size_t index, size_t col = 0) = 0;
	virtual SHVRefObject* GetItemData(SHVControlListView* owner, size_t index) = 0;
	virtual void AddItem(SHVControlListView* owner, const SHVStringC str, SHVRefObject* data = NULL) = 0;
	virtual void SetItemText(SHVControlListView* owner, const SHVStringC text, size_t index, size_t col = 0) = 0;

	virtual void ClearColumns(SHVControlListView* owner) = 0;
	virtual void AddColumn(SHVControlListView*  owner, const SHVStringC colName, SHVInt width = SHVInt()) = 0;

	virtual SHVInt GetSelected(SHVControlListView* owner) = 0;
	virtual void SetSelected(SHVControlListView* owner, SHVInt index) = 0;

};



//-=========================================================================================================
/// SHVControlImplementerListViewCustomDraw - listview implementer for custom drawn listviews
/**
 * Interface that has to be implemented for the specific platform
 */

class SHVControlImplementerListViewCustomDraw : public SHVControlImplementerListView
{
public:

	virtual void SubscribeDraw(SHVEventSubscriberBase* subscriber) = 0;

};



// ============================================= implementation ============================================= //


/*************************************
 * Constructor
 *************************************/
SHVControlListView::SHVControlListView(SHVGUIManager* manager, SHVControlImplementer* implementor)
  : SHVControl(manager,implementor)
{}

/*************************************
 * SetParent
 *************************************/
SHVControlListView* SHVControlListView::SetParent(SHVControlContainer* parent, int flags)
{
	SHVControl::SetParent(parent,flags);
	return this;
}

/*************************************
 * GetItemCount
 *************************************/
size_t SHVControlListView::GetItemCount()
{
	return GetImplementor()->GetItemCount(this);
}

/*************************************
 * ClearItems
 *************************************/
void SHVControlListView::ClearItems()
{
	GetImplementor()->ClearItems(this);
}

/*************************************
 * GetItemText
 *************************************/
SHVStringBuffer SHVControlListView::GetItemText(size_t index, size_t col)
{
	return GetImplementor()->GetItemText(this,index,col);
}

/*************************************
 * GetItemData
 *************************************/
SHVRefObject* SHVControlListView::GetItemData(size_t index)
{
	return GetImplementor()->GetItemData(this,index);
}

/*************************************
 * AddItem
 *************************************/
SHVControlListView* SHVControlListView::AddItem(const SHVStringC str, SHVRefObject* data)
{
	GetImplementor()->AddItem(this,str,data);
	return this;
}

/*************************************
 * SetItemText
 *************************************/
SHVControlListView* SHVControlListView::SetItemText(const SHVStringC text, size_t index, size_t col)
{
	GetImplementor()->SetItemText(this,text,index,col);
	return this;
}

/*************************************
 * ClearColumns
 *************************************/
SHVControlListView* SHVControlListView::ClearColumns()
{
	GetImplementor()->ClearColumns(this);
	return this;
}

/*************************************
 * AddColumn
 *************************************/
SHVControlListView* SHVControlListView::AddColumn(const SHVStringC colName, SHVInt width)
{
	GetImplementor()->AddColumn(this,colName,width);
	return this;
}

/*************************************
 * GetSelected
 *************************************/
SHVInt SHVControlListView::GetSelected()
{
	return GetImplementor()->GetSelected(this);
}

/*************************************
 * SetSelected
 *************************************/
SHVControlListView* SHVControlListView::SetSelected(SHVInt index)
{
	GetImplementor()->SetSelected(this,index);
	return this;
}

/*************************************
 * GetImplementor
 *************************************/
SHVControlImplementerListView* SHVControlListView::GetImplementor()
{
	return (SHVControlImplementerListView*)(SHVControlImplementer*)Implementor;
}

/*************************************
 * DrawDataFromEvent
 *************************************/
SHVDrawEventDataListView* SHVControlListView::DrawDataFromEvent(SHVEvent* event)
{
	return (SHVDrawEventDataListView*)event->GetData();
}

/*************************************
 * SHVDrawEventDataListView constructor
 *************************************/
SHVDrawEventDataListView::SHVDrawEventDataListView(SHVDraw* draw) : SHVDrawEventData(draw)
{
	RowID = ColumnID = 0;
	IsSelected = false;
}

#endif
