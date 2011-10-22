#ifndef __SHIVA_GTKGUI_SHVCONTROLIMPLEMENTERLISTVIEW_H
#define __SHIVA_GTKGUI_SHVCONTROLIMPLEMENTERLISTVIEW_H

#include "../../../include/gui/shvcontrollistview.h"
#include "../../../include/utils/shvvector.h"
#include "../../../include/utils/shvvectorref.h"
#include "shvcontrolimplementergtkwidget.h"



//-=========================================================================================================
/// SHVControlImplementerListViewGtk
/**
 * Implementation of a listview implementer for GTK
 */

class SHVControlImplementerListViewGtk : public SHVControlImplementerGtkWidget<SHVControlImplementerListViewCustomDraw>
{
public:

	SHVControlImplementerListViewGtk(int subType);

	// From SHVControlImplementer
	virtual int GetSubType(SHVControl* owner);
	virtual SHVBool Create(SHVControl* owner, SHVControlImplementer* parent, int flags);
	virtual SHVBool Destroy(SHVControl* owner);

	virtual SHVBool SetFont(SHVControl* owner, SHVFont* font, bool resetHeight);

	// From SHVControlImplementerListView
	virtual size_t GetItemCount(SHVControlListView* owner);
	virtual void ClearItems(SHVControlListView* owner);
	virtual SHVStringBuffer GetItemText(SHVControlListView* owner, size_t index, size_t col = 0);
	virtual SHVRefObject* GetItemData(SHVControlListView* owner, size_t index);
	virtual void AddItem(SHVControlListView* owner, const SHVStringC str, SHVRefObject* data = NULL);
	virtual void SetItemText(SHVControlListView* owner, const SHVStringC text, size_t index, size_t col = 0);

	virtual void ClearColumns(SHVControlListView* owner);
	virtual void AddColumn(SHVControlListView*  owner, const SHVStringC colName, SHVInt width);

	virtual SHVInt GetSelected(SHVControlListView* owner);
	virtual void SetSelected(SHVControlListView* owner, SHVInt index);


	// From SHVControlImplementerListViewCustomDraw
	virtual void SubscribeDraw(SHVEventSubscriberBase* subscriber);


private:

	///\cond INTERNAL
	struct CachedCol
	{
		SHVString ColName; SHVInt Width;
		inline CachedCol(const SHVStringC colName, SHVInt width) : ColName(colName), Width(width) {}
	};
	SHVVector<CachedCol> CachedCols;
	SHVVectorRef<SHVRefObject> ItemObjects;
	GtkWidget* TreeView;
//	static gboolean expose_event(GtkWidget *widget, GdkEvent* event, gpointer user_data);
	void RecreateModel(bool clearRows = false);

	int SubType;
	SHVEventSubscriberBaseRef Subscriber;
	///\endcond
};

#endif
