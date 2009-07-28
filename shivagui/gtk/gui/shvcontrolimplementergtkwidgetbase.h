#ifndef __SHIVA_GTKGUI_CONTROLIMPLEMENTERBASE_H
#define __SHIVA_GTKGUI_CONTROLIMPLEMENTERBASE_H


#include "../../../include/gui/shvcontrolimplementer.h"
#include "../../../include/gui/utils/shvpoint.h"
#include "utils/shvfontgtk.h"

#ifndef __SHIVA_GTK
# error This code only works for gtk
#else
# include <gtk/gtk.h>
#endif

// forward declare
//class SHVControlWrapperGtk;
//class SHVControlContainerWrapperGtk;


//-=========================================================================================================
/// SHVControlImplementerGtkWidgetBase - implementation of base implementer methods
/**
 */

class SHVControlImplementerGtkWidgetBase
{
public:


	SHVControlImplementerGtkWidgetBase();
	virtual ~SHVControlImplementerGtkWidgetBase();


	virtual SHVBool IsCreated();
	virtual SHVBool Reparent(SHVControl* owner, SHVControlImplementer* parent, int flags);
	virtual SHVBool Destroy(SHVControl* owner);

	virtual SHVRect GetRect(SHVControl* owner);
	virtual void SetRect(SHVControl* owner, const SHVRect& rect);

	virtual SHVBool SetFlag(SHVControl* owner, int flag, bool enable = true);
	virtual bool GetFlag(SHVControl* owner, int flag);

	virtual SHVFont* GetFont(SHVControl* owner);
	virtual SHVBool SetFont(SHVControl* owner, SHVFont* font, SHVInt newHeight);


	virtual GtkWidget* GetHandle();

	SHVPoint CalculateMinSize(SHVControl* owner, int widthInChars, int heightInChars);

	void SetResizable(bool resizable);

protected:

	virtual void SetHandle(GtkWidget* handle);


private:
//friend class SHVControlWrapperWin32;
//friend class SHVControlContainerWrapperWin32;

	GtkWidget* Window;
	SHVFontGtkRef Font;

};


#endif
