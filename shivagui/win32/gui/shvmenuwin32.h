#ifndef __SHIVA_WIN32GUI_MENU_H
#define __SHIVA_WIN32GUI_MENU_H


#include "../../../include/gui/shvmenu.h"
#include "shvguimanagerwin32.h"

#ifndef __SHIVA_WIN32
# error This code only works for win32
#elif defined(__SHIVA_MFC)
# error This code does not work in MFC mode
#endif


//-=========================================================================================================
/// SHVMenuWin32
/**
 */

class SHVMenuWin32 : public SHVMenu
{
public:


	// constructor
	SHVMenuWin32(SHVGUIManagerWin32* manager, SHVMenu::Types type, SHVEventSubscriberBase* subscriber, SHVControl* parent);
	virtual ~SHVMenuWin32();


	// Obtain type
	virtual Types GetType();
	
	// Manipulation items
	virtual void AddStringItem(SHVInt id, const SHVStringC name);
	virtual void AddSeparator();
	virtual SHVMenu* AddSubMenu(const SHVStringC name);

	// Make the menu visible
	virtual void Show(PopupTypes type = PopupDefault, SHVPoint offset = SHVPoint());

	void EmitEvent(SHVInt id);

private:
	///\cond INTERNAL
	void EnsureMenuCreated();

	HMENU hMenuTopLevel;
	HMENU hMenu;
	SHVGUIManagerWin32* Manager;
	SHVMenu::Types Type;
	SHVEventSubscriberBaseRef Subscriber;
	SHVControl* Parent;
	///\endcond
};

#endif
