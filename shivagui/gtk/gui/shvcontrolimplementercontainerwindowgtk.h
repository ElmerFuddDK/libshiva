#ifndef __SHIVA_GTKGUI_SHVCONTROLIMPLEMENTERCONTAINERWINDOW_H
#define __SHIVA_GTKGUI_SHVCONTROLIMPLEMENTERCONTAINERWINDOW_H


#include "../../../include/gui/shvcontrolcontainer.h"
#include "shvcontrolimplementergtkwidget.h"
#include "utils/shvfontgtk.h"

#ifndef __SHIVA_GTK
# error This code only works for gtk
#else
# include <gtk/gtk.h>
#endif


//-=========================================================================================================
/// SHVControlImplementerContainerWindowGtk - Container window implementation
/**
 * Use this class as a main window implementer in gtk.
 */

class SHVControlImplementerContainerWindowGtk : public SHVControlImplementerGtkWidget<SHVControlImplementerContainerCustomDraw>
{
public:


	SHVControlImplementerContainerWindowGtk(int subType);
	~SHVControlImplementerContainerWindowGtk();


	// From SHVControlImplementer
	virtual int GetSubType(SHVControl* owner);

	virtual SHVBool Create(SHVControl* owner, SHVControlImplementer* parent, int flags);
	virtual SHVBool Destroy(SHVControl* owner);

	// From SHVControlImplementerContainer
	virtual SHVRect GetRegionRect();
	virtual void SetSize(SHVControlContainer* owner, int widthInPixels, int heightInPixels, SHVControlContainer::PosModes mode);

	virtual SHVStringBuffer GetTitle(SHVControlContainer* control);
	virtual void SetTitle(SHVControlContainer* control, const SHVStringC& title);

	virtual SHVColor* GetColor(SHVControlContainer* owner);
	virtual void SetColor(SHVControlContainer* owner, SHVColor* color);

	virtual void SetMinimumSize(SHVControlContainer* owner, int widthInPixels, int heightInPixels);
	virtual SHVPoint GetMinimumSizeInPixels(SHVControlContainer* owner);


	// From SHVControlImplementerContainerCustomDraw
	virtual void SubscribeDraw(SHVEventSubscriberBase* subscriber);


protected:

	virtual void SetResizable(bool resizable);

private:
	///\cond INTERNAL
	static void on_size_allocate(GtkWidget * widget, GtkAllocation *allocation, gpointer data);
	static gboolean on_delete_event(GtkWidget *widget, GdkEvent *event, gpointer data);
	static gboolean expose_event(GtkWidget *widget, GdkEvent* event, gpointer user_data);
	
	int SubType;
	SHVString Title;
	SHVPoint MinSize;
	
	SHVRect SizedRect;
	
	SHVEventSubscriberBaseRef Subscriber;
	SHVColorRef Color;
	SHVFontGtkRef Font;
	///\endcond
};

#endif
