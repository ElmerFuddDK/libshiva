#ifndef __SHIVA_GTKGUI_SHVMENUGTKCONTAINER_H
#define __SHIVA_GTKGUI_SHVMENUGTKCONTAINER_H


#include "shvmenugtkroot.h"
#include "shvguimanagergtk.h"

#ifndef __SHIVA_GTK
# error This code only works for gtk
#endif


//-=========================================================================================================
/// SHVMenuGtkContainer
/**
 */

class SHVMenuGtkContainer : public SHVMenuGtkRoot
{
public:


	// constructor
	SHVMenuGtkContainer(SHVGUIManagerGtk* manager, SHVEventSubscriberBase* subscriber, SHVControlContainer* parent);
	virtual ~SHVMenuGtkContainer();

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
	int GetHeight();
	void Move(SHVRect& rect);
	///\endcond



protected:
	///\cond INTERNAL
	static void on_activate(GtkMenuItem *menuitem, gpointer data);
	
	GtkWidget* MenuBar;
	bool Shown;
	
	SHVGUIManagerGtk* Manager;
	SHVEventSubscriberBaseRef Subscriber;
	SHVControlContainer* Parent;
	///\endcond
};
typedef SHVRefObjectContainer<SHVMenuGtkContainer> SHVMenuGtkContainerRef;

#endif
