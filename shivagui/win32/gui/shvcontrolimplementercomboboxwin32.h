#ifndef __SHIVA_WIN32GUI_SHVCONTROLIMPLEMENTERCOMBOBOX_H
#define __SHIVA_WIN32GUI_SHVCONTROLIMPLEMENTERCOMBOBOX_H


#include "../../../include/gui/shvcontrolcombobox.h"
#include "shvcontrolimplementerwin32.h"

#ifndef __SHIVA_WIN32
# error This code only works for win32
#elif defined(__SHIVA_MFC)
# error This code does not work in MFC mode
#endif


//-=========================================================================================================
/// SHVControlImplementerComboBoxWin32
/**
 */

class SHVControlImplementerComboBoxWin32 : public SHVControlImplementerWin32<SHVControlImplementerComboBox>
{
public:


	SHVControlImplementerComboBoxWin32(int subType);


	// From SHVControlImplementer
	virtual int GetSubType(SHVControl* owner);

	virtual SHVBool Create(SHVControl* owner, SHVControlImplementer* parent, int flags);


	// From SHVControlImplementerComboBox
	virtual SHVStringBuffer GetText(SHVControlComboBox* owner);
	virtual void SetText(SHVControlComboBox* owner, const SHVStringC& text);

	virtual SHVInt GetSelected(SHVControlComboBox* owner);
	virtual void SetSelected(SHVControlComboBox* owner, SHVInt index);

	virtual size_t GetItemCount(SHVControlComboBox* owner);
	virtual void ClearItems(SHVControlComboBox* owner);
	virtual SHVStringBuffer GetItemText(SHVControlComboBox* owner, size_t index);
	virtual SHVRefObject* GetItemData(SHVControlComboBox* owner, size_t index);
	virtual void AddItem(SHVControlComboBox* owner, const SHVStringC str, SHVRefObject* data = NULL);

	virtual void SetDropdownHeight(SHVControlComboBox* owner, int lines);


private:
	///\cond INTERNAL
	WNDPROC OrigProc;

	int SubType;
	SHVInt Selected;

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	///\endcond
};

#endif
