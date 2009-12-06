#ifndef __SHIVA_GTKGUI_SHVMENUGTKSUB_H
#define __SHIVA_GTKGUI_SHVMENUGTKSUB_H


#include "../../../include/gui/shvmenu.h"
#include "../../../include/utils/shvvectorref.h"
#include "shvguimanagergtk.h"

#ifndef __SHIVA_GTK
# error This code only works for gtk
#endif
class SHVMenuGtkRoot;


//-=========================================================================================================
/// SHVMenuGtkSub
/**
 */

class SHVMenuGtkSub : public SHVMenu
{
public:


	// constructor
	SHVMenuGtkSub(SHVGUIManagerGtk* manager, SHVMenuGtkRoot* rootMenu);
	virtual ~SHVMenuGtkSub();


	// Obtain type
	virtual Types GetType();
	

	// Container mode stuff
	virtual ContainerModes GetContainerMode();
	virtual SHVInt GetCompactButtonCount();


	// Manipulation items
	virtual void AddStringItem(SHVInt id, const SHVStringC name, int flags = FlagDefault);
	virtual void AddSeparator();
	virtual SHVMenu* AddSubMenu(const SHVStringC name);

	// Make the menu visible
	virtual void Show(PopupTypes type = PopupDefault, SHVPoint offset = SHVPoint());

	void EmitEvent(SHVInt id);
	
	///\cond INTERNAL
	GtkWidget* EnsureMenuCreated();
	///\endcond



protected:
	///\cond INTERNAL
	static void on_activate(GtkMenuItem *menuitem, gpointer data);
	
	GtkWidget* Menu;
	SHVVectorRef<SHVMenuGtkSub> SubMenus;

	SHVGUIManagerGtk* Manager;
	SHVMenuGtkRoot* RootMenu;
	///\endcond
};
typedef SHVRefObjectContainer<SHVMenuGtkSub> SHVMenuGtkSubRef;

#endif
