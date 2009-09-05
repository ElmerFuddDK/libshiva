#ifndef __SHIVA_GTKGUI_SHVCONTROLIMPLEMENTERMAINWINDOW_H
#define __SHIVA_GTKGUI_SHVCONTROLIMPLEMENTERMAINWINDOW_H


#include "../../../include/gui/shvcontrolcontainer.h"
#include "utils/shvfontgtk.h"

#ifndef __SHIVA_GTK
# error This code only works for gtk
#else
# include <gtk/gtk.h>
#endif


//-=========================================================================================================
/// SHVControlImplementerDialogGtk - Dialog window implementation
/**
 * Use this class as a main window implementer in gtk.
 */

class SHVControlImplementerDialogGtk : public SHVControlImplementerContainer
{
public:


	SHVControlImplementerDialogGtk(int subType);
	~SHVControlImplementerDialogGtk();


	// From SHVControlImplementer
	virtual int GetSubType(SHVControl* owner);

	virtual SHVBool IsCreated();
	virtual SHVBool Create(SHVControl* owner, SHVControlImplementer* parent, int flags);
	virtual SHVBool Reparent(SHVControl* owner, SHVControlImplementer* parent, int flags);
	virtual SHVBool Destroy(SHVControl* owner);

	virtual SHVRect GetRect(SHVControl* owner);
	virtual void SetRect(SHVControl* owner, const SHVRect& rect);

	virtual SHVBool SetFlag(SHVControl* owner, int flag, bool enable);
	virtual bool GetFlag(SHVControl* owner, int flag);

	virtual SHVFont* GetFont(SHVControl* owner);
	virtual SHVBool SetFont(SHVControl* owner, SHVFont* font, bool resetHeight);

	virtual void* GetNative();
	GtkWidget* GetMainWndHandle();


	// From SHVControlImplementerContainer
	virtual SHVRect GetRegionRect();
	virtual void SetSize(SHVControlContainer* owner, int widthInPixels, int heightInPixels, SHVControlContainer::PosModes mode);

	virtual SHVStringBuffer GetTitle(SHVControlContainer* control);
	virtual void SetTitle(SHVControlContainer* control, const SHVStringC& title);

	virtual SHVColor* GetColor(SHVControlContainer* owner);
	virtual void SetColor(SHVControlContainer* owner, SHVColor* color);

	virtual void SetMinimumSize(SHVControlContainer* owner, int widthInPixels, int heightInPixels);
	virtual SHVPoint GetMinimumSizeInPixels(SHVControlContainer* owner);


protected:

	virtual void SetResizable(bool resizable);

private:
	///\cond INTERNAL
	static void on_size_allocate(GtkWidget * widget, GtkAllocation *allocation, gpointer data);
	static gboolean on_delete_event(GtkWidget *widget, GdkEvent *event, gpointer data);
	
	GtkWidget* MainWindow;
	GtkWidget* Handle;
	int SubType;
	bool Visible;
	
	SHVRect SizedRect;
	
	SHVColorRef Color;
	SHVFontGtkRef Font;
	///\endcond
};

#endif
