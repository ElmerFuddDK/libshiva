#ifndef __SHIVA_GTKGUI_SHVCONTROLIMPLEMENTERCOMBOBOX_H
#define __SHIVA_GTKGUI_SHVCONTROLIMPLEMENTERCOMBOBOX_H

#include "../../../include/gui/shvcontrolcombobox.h"
#include "../../../include/utils/shvvectorref.h"
#include "shvcontrolimplementergtkwidget.h"



//-=========================================================================================================
/// SHVControlImplementerComboBoxGtk
/**
 * Implementation of a single line edit implementer for GTK
 */

class SHVControlImplementerComboBoxGtk : public SHVControlImplementerGtkWidget<SHVControlImplementerComboBox>
{
public:

	SHVControlImplementerComboBoxGtk(int subType);

	// From SHVControlImplementer
	virtual int GetSubType(SHVControl* owner);
	virtual SHVBool Create(SHVControl* owner, SHVControlImplementer* parent, int flags);

	virtual SHVBool SetFlag(SHVControl* owner, int flag, bool enable);
	virtual bool GetFlag(SHVControl* owner, int flag);

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
	int SubType;
	SHVVectorRef<SHVRefObject> Items;
	///\endcond
};

#endif
