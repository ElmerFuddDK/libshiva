#ifndef __SHIVA_GTKGUI_H
#define __SHIVA_GTKGUI_H

#include "shvguimanagergtk.h"
#include "../../../include/gui/shvcontrol.h"
#include "../../../include/gui/shvcontrolimplementer.h"

#ifndef __SHIVA_GTK
# error This code only works for gtk
#endif


class Gtk
{
public:

	inline static GtkWidget* GetHandle(SHVControl* control);
	inline static GtkWidget* GetHandle(SHVControlImplementer* implementer);
	static GtkWidget* GetMainWndHandle(SHVControl* control);

	inline static SHVConfig& GetGUIConfig(SHVControl* control);

//	inline static SHVDrawGtk* CreateDraw(SHVControl* control);
};



// ============================================= implementation ============================================= //

/*************************************
 * GetHandle
 *************************************/
GtkWidget* Gtk::GetHandle(SHVControl* control)
{
	return (GtkWidget*)control->GetImplementor()->GetNative();
}

/*************************************
 * GetHandle
 *************************************/
GtkWidget* Gtk::GetHandle(SHVControlImplementer* implementer)
{
	return (GtkWidget*)implementer->GetNative();
}

/*************************************
 * GetGUIConfig
 *************************************/
SHVConfig& Gtk::GetGUIConfig(SHVControl* control)
{
	return control->GetModuleList().GetConfig(SHVModuleList::CfgGUI);
}

/*************************************
 * CreateDraw
 *************************************/
//SHVDrawGtk* Gtk::CreateDraw(SHVControl* control)
//{
//	return ((SHVGUIManagerGtk*)control->GetManager())->CreateDraw(GetHandle(control));
//}
#endif
