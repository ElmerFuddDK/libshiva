#ifndef __SHIVA_GTKGUI_SHVMENUGTKPOPUP_H
#define __SHIVA_GTKGUI_SHVMENUGTKPOPUP_H


#include "shvmenugtkroot.h"

#ifndef __SHIVA_GTK
# error This code only works for gtk
#endif


//-=========================================================================================================
/// SHVMenuGtkPopup
/**
 */

class SHVMenuGtkPopup : public SHVMenuGtkRoot
{
public:


	// constructor
	SHVMenuGtkPopup(SHVGUIManagerGtk* manager, SHVEventSubscriberBase* subscriber, SHVControl* parent);
	virtual ~SHVMenuGtkPopup();

	// Destroy the menu
	void Destroy();

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

	virtual void EmitEvent(SHVInt id);
	bool IsShown();
	
	///\cond INTERNAL
	void EnsureMenuCreated();
	///\endcond



protected:
	///\cond INTERNAL
	static void on_activate(GtkMenuItem *menuitem, gpointer data);
	static void on_selection_done(GtkMenuShell* menushell, gpointer data);
	static void menu_position(GtkMenu* menu, gint* x, gint* y, gboolean* push_in, gpointer data);
	
	GtkWidget* Menu;
	bool Shown;
	SHVPoint Offset;
	PopupTypes ShowType;
	
	SHVGUIManagerGtk* Manager;
	SHVEventSubscriberBaseRef Subscriber;
	SHVControl* Parent;
	///\endcond
};
typedef SHVRefObjectContainer<SHVMenuGtkPopup> SHVMenuGtkPopupRef;

#endif
