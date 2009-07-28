#ifndef __SHIVA_GTKGUI_SHVCONTROLIMPLEMENTERLABEL_H
#define __SHIVA_GTKGUI_SHVCONTROLIMPLEMENTERLABEL_H

#include "../../../include/gui/shvcontrollabel.h"
#include "shvcontrolimplementergtkwidget.h"



//-=========================================================================================================
/// SHVControlImplementerLabelGtk
/**
 * Implementation of a label implementer for GTK
 */

class SHVControlImplementerLabelGtk : public SHVControlImplementerGtkWidget<SHVControlImplementerLabelCustomDraw>
{
public:

	SHVControlImplementerLabelGtk(int subType);

	// From SHVControlImplementer
	virtual int GetSubType(SHVControl* owner);
	virtual SHVBool Create(SHVControl* owner, SHVControlImplementer* parent, int flags);

	// From SHVControlImplementerLabel
	virtual SHVStringBuffer GetText();
	virtual void SetText(SHVControlLabel* owner, const SHVStringC& text, bool autoSize);


	// From SHVControlImplementerLabelCustomDraw
	virtual void SubscribeDraw(SHVEventSubscriberBase* subscriber);


private:

	///\cond INTERNAL
	int SubType;
	SHVEventSubscriberBaseRef Subscriber;
	///\endcond
};

#endif
