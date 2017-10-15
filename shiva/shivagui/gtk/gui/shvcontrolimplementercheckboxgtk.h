#ifndef __SHIVA_WIN32GUI_SHVCONTROLIMPLEMENTERCHECKBOX_H
#define __SHIVA_WIN32GUI_SHVCONTROLIMPLEMENTERCHECKBOX_H


#include "../../../include/gui/shvcontrolcheckbox.h"
#include "shvcontrolimplementergtkwidget.h"

#ifndef __SHIVA_GTK
# error This code only works for gtk
#endif


//-=========================================================================================================
/// SHVControlImplementerCheckboxGtk - checkbox implementation
/**
 */

class SHVControlImplementerCheckboxGtk : public SHVControlImplementerGtkWidget<SHVControlImplementerCheckbox>
{
public:


	SHVControlImplementerCheckboxGtk(int subType);


	// From SHVControlImplementer
	virtual int GetSubType(SHVControl* owner);

	virtual SHVBool Create(SHVControl* owner, SHVControlImplementer* parent, int flags);


	// From SHVControlImplementerCheckbox
	virtual SHVStringBuffer GetText();
	virtual void SetText(SHVControlCheckbox* owner, const SHVStringC& text, bool autoSize);
	
	virtual SHVInt GetChecked();
	virtual void SetChecked(SHVControlCheckbox* owner, SHVInt state);


private:
	///\cond INTERNAL
	static void on_toggled(GtkToggleButton *togglebutton, gpointer user_data);
	
	int SubType;
	bool InToggled;
	///\endcond
};

#endif
