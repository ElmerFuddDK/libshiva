#ifndef __SHIVA_WIN32GUI_MENU_H
#define __SHIVA_WIN32GUI_MENU_H


#include "../../../include/gui/shvmenu.h"
#include "shvguimanagerwin32.h"

#ifndef __SHIVA_WIN32
# error This code only works for win32
#elif defined(__SHIVA_MFC)
# error This code does not work in MFC mode
#endif

#ifdef __SHIVA_POCKETPC
///\cond INTERNAL
class SHVMenuCommandBarPocketPC;
class SHVMenuContainerPocketPC;
///\endcond
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



private:
	///\cond INTERNAL
friend class SHVMenuContainerPocketPC;
	void EnsureMenuCreated();

	HMENU hMenuTopLevel;
	HMENU hMenu;
	SHVGUIManagerWin32* Manager;
	SHVMenu::Types Type;
	SHVEventSubscriberBaseRef Subscriber;
	SHVControl* Parent;
	///\endcond
};
typedef SHVRefObjectContainer<SHVMenuWin32> SHVMenuWin32Ref;



#ifdef __SHIVA_POCKETPC
///\cond INTERNAL

//-=========================================================================================================
/// SHVMenuContainerPocketPC
/**
 */

class SHVMenuContainerPocketPC : public SHVMenuWin32
{
public:


	SHVMenuContainerPocketPC(SHVGUIManagerWin32* manager, SHVEventSubscriberBase* subscriber, SHVControl* parent);
	virtual ~SHVMenuContainerPocketPC();

	// Manipulation items
	virtual void AddStringItem(SHVInt id, const SHVStringC name, int flags = FlagDefault);
	virtual void AddSeparator();
	virtual SHVMenu* AddSubMenu(const SHVStringC name);

	// Make the menu "visible"
	virtual void Show(PopupTypes type = PopupDefault, SHVPoint offset = SHVPoint());

	void PerformButton(int num, SHVPoint offset);

private:
friend class SHVMenuCommandBarPocketPC;

	SHVMenuCommandBarPocketPC* CmdBar;

	SHVMenuWin32Ref Menu1, Menu2;
	//bool TopLevelNeedSeparator;

	SHVString Button1MenuName, Button2MenuName;
	SHVInt Button1ID, Button2ID;
	bool Button1Enabled, Button2Enabled;
};
typedef SHVRefObjectContainer<SHVMenuContainerPocketPC> SHVMenuContainerPocketPCRef;



//-=========================================================================================================
/// SHVMenuCommandBarPocketPC
/**
 * Internal class for handling the command bar in PocketPC
 */

class SHVMenuCommandBarPocketPC
{
public:

	SHVMenuCommandBarPocketPC(HWND parent, HINSTANCE hInstance, SHVGUIManagerWin32* manager);
	~SHVMenuCommandBarPocketPC();


	void InitializeMenu(SHVMenuContainerPocketPC* menu);
	void SetMenu(HWND parent, SHVMenuContainerPocketPC* menu); // returns true if the menu needs to be shown
	bool OnCommandMsg(HWND hWnd, WPARAM wParam, LPARAM lParam); // returns true if it thinks this message is for this command bar


	HWND hCmdWnd;

protected:
	SHVMenuContainerPocketPCRef Menu;
	HINSTANCE hInstance;
	SHVGUIManagerWin32* Manager;
};
typedef SHVPtrContainer<SHVMenuCommandBarPocketPC> SHVMenuCommandBarPocketPCPtr;
///\endcond
#endif

#endif
