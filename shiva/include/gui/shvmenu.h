#ifndef __SHIVA_GUI_MENU_H
#define __SHIVA_GUI_MENU_H

#include "../../include/framework/shvmodulelist.h"
#include "../../include/utils/shvrefobject.h"
#include "../../include/utils/shvstring.h"
#include "utils/shvfont.h"
#include "utils/shvpoint.h"


//-=========================================================================================================
/// SHVMenu - Interface for menus in SHIVA
/**
 * A menu can either be a menu associated with a top level control container
 * or a popup menu. This is defined on creation time, either by using
 * SHVControlContainer::CreateMenu or SHVControl::CreatePopupMenu\n
 * A different type, Sub, is used for adding sub menus to the above mentioned
 * types.\n
 * Adding items and separators directly to a control container menu will not
 * work on all platforms.\n
 * A menu of type control container can be in 2 modes depending on platform.
 * In compact mode the menu is limited to a series of buttons, and once the
 * buttons are filled up it will fall back to adding the submenus to the first
 * button menu, if possible.\n
 *\note On some platforms a popup menu is modal.
 */

class SHVMenu : public SHVRefObject
{
public:


	// defines and constants
	enum Types {
		TypeSub,
		TypeControlContainer,
		TypePopup
	};

	enum PopupTypes {
		PopupDefault = 0,
		PopupMouse,
		PopupBelowWindow,
		PopupCenterWindow
	};

	enum ContainerModes {
		ContainerInvalid = -1, // not a container type menu
		ContainerNormal,       // normal PC style menus
		ContainerCompact       // compact style PDA/phone menus (typically 2 buttons)
	};

	enum Flags {
		FlagNone = 0,
		FlagDisabled  = 1,
		FlagDefault = FlagNone
	};


	// Obtain type
	virtual Types GetType() = 0;


	// Container mode stuff
	virtual ContainerModes GetContainerMode() = 0;
	virtual SHVInt GetCompactButtonCount() = 0;
	
	
	// Manipulation items
	virtual void AddStringItem(SHVInt id, const SHVStringC name, int flags = FlagDefault) = 0;
	virtual void AddSeparator() = 0;
	virtual SHVMenu* AddSubMenu(const SHVStringC name) = 0;

	// Make the menu visible
	virtual void Show(PopupTypes type = PopupDefault, SHVPoint offset = SHVPoint()) = 0;


protected:
	// Destructor
	virtual ~SHVMenu() {}
};
typedef SHVRefObjectContainer<SHVMenu> SHVMenuRef;

#endif
