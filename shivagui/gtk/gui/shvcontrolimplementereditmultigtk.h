#ifndef __SHIVA_GTKGUI_SHVCONTROLIMPLEMENTEREDITMULTI_H
#define __SHIVA_GTKGUI_SHVCONTROLIMPLEMENTEREDITMULTI_H

#include "../../../include/gui/shvcontroledit.h"
#include "shvcontrolimplementergtkwidget.h"



//-=========================================================================================================
/// SHVControlImplementerEditGtk
/**
 * Implementation of a multi line edit implementer for GTK
 */

class SHVControlImplementerEditMultiGtk : public SHVControlImplementerGtkWidget<SHVControlImplementerEdit>
{
public:

	SHVControlImplementerEditMultiGtk();

	// From SHVControlImplementer
	virtual int GetSubType(SHVControl* owner);
	virtual SHVBool Create(SHVControl* owner, SHVControlImplementer* parent, int flags);
	virtual SHVBool Destroy(SHVControl* owner);

	virtual SHVBool SetFlag(SHVControl* owner, int flag, bool enable);
	virtual bool GetFlag(SHVControl* owner, int flag);

	virtual SHVBool SetFont(SHVControl* owner, SHVFont* font, bool resetHeight);

	// From SHVControlImplementerEdit
	virtual SHVStringBuffer GetText();
	virtual void SetText(const SHVStringC& text);

	virtual int GetLimit();
	virtual void SetLimit(int limit);

	virtual void SetHeight(SHVControlEdit* owner, int lines); // only applicable on multiline boxes

	// From SHVControlImplementerGtkWidget
	virtual int CalculateNewHeight(SHVControl* owner, SHVFont* font);

private:

	///\cond INTERNAL
	GtkWidget* TextView;
	int Lines;
	///\endcond
};

#endif
