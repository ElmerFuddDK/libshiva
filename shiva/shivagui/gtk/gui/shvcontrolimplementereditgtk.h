#ifndef __SHIVA_GTKGUI_SHVCONTROLIMPLEMENTEREDIT_H
#define __SHIVA_GTKGUI_SHVCONTROLIMPLEMENTEREDIT_H

#include "../../../include/gui/shvcontroledit.h"
#include "shvcontrolimplementergtkwidget.h"



//-=========================================================================================================
/// SHVControlImplementerEditGtk
/**
 * Implementation of a single line edit implementer for GTK
 */

class SHVControlImplementerEditGtk : public SHVControlImplementerGtkWidget<SHVControlImplementerEdit>
{
public:

	SHVControlImplementerEditGtk();

	// From SHVControlImplementer
	virtual int GetSubType(SHVControl* owner);
	virtual SHVBool Create(SHVControl* owner, SHVControlImplementer* parent, int flags);

	virtual SHVBool SetFlag(SHVControl* owner, int flag, bool enable);
	virtual bool GetFlag(SHVControl* owner, int flag);

	// From SHVControlImplementerEdit
	virtual SHVStringBuffer GetText();
	virtual void SetText(const SHVStringC& text);

	virtual int GetLimit();
	virtual void SetLimit(int limit);

	virtual void SetHeight(SHVControlEdit* owner, int lines); // only applicable on multiline boxes

	virtual void SetSelection(SHVControlEdit* owner, int pos, SHVInt selectFrom = SHVInt(), SHVControlEdit::ScrollModes scroll = SHVControlEdit::ScrollNone);


private:

	///\cond INTERNAL
	///\endcond
};

#endif
