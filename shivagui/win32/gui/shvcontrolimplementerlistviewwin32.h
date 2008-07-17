#ifndef __SHIVA_WIN32GUI_SHVCONTROLIMPLEMENTERLISTVIEW_H
#define __SHIVA_WIN32GUI_SHVCONTROLIMPLEMENTERLISTVIEW_H

#include "../../../include/gui/shvcontrollistview.h"
#include "shvcontrolimplementerwin32.h"

#ifndef __SHIVA_WIN32
# error This code only works for win32
#elif defined(__SHIVA_MFC)
# error This code does not work in MFC mode
#endif


//-=========================================================================================================
/// SHVControlImplementerListViewWin32 - listview implementation
/**
 */

class SHVControlImplementerListViewWin32 : public SHVControlImplementerWin32<SHVControlImplementerListViewCustomDraw>
{
public:
	SHVControlImplementerListViewWin32(int subType);


	// From SHVControlImplementer
	virtual int GetSubType(SHVControl* owner);
	virtual SHVBool Create(SHVControl* owner, SHVControlImplementer* parent, int flags);


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
	int SubType;
	SHVEventSubscriberBaseRef Subscriber;
	WNDPROC OrigProc;

	size_t ColCount;
	SHVInt SelectedIndex;

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	///\endcond
};

#endif
