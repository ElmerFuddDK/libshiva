#ifndef __SHIVA_WIN32GUI_SHVCONTROLIMPLEMENTERBUTTON_H
#define __SHIVA_WIN32GUI_SHVCONTROLIMPLEMENTERBUTTON_H


#include "../../../include/gui/shvcontrolbutton.h"
#include "shvcontrolimplementergtkwidget.h"

#ifndef __SHIVA_GTK
# error This code only works for gtk
#endif


//-=========================================================================================================
/// SHVControlImplementerButtonGtk - button implementation
/**
 */

class SHVControlImplementerButtonGtk : public SHVControlImplementerGtkWidget<SHVControlImplementerButton>
{
public:


	SHVControlImplementerButtonGtk();


	// From SHVControlImplementer
	virtual int GetSubType(SHVControl* owner);

	virtual SHVBool Create(SHVControl* owner, SHVControlImplementer* parent, int flags);


	// From SHVControlImplementerButton
	virtual SHVStringBuffer GetText();
	virtual void SetText(SHVControlButton* owner, const SHVStringC& text, bool autoSize);


private:
	///\cond INTERNAL
	static void on_clicked(GtkButton *button, gpointer user_data);
	///\endcond
};

#endif
