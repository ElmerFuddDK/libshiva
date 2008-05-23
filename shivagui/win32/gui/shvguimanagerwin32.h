#ifndef __SHIVA_WIN32GUI_MANAGER_H
#define __SHIVA_WIN32GUI_MANAGER_H

#include "../../gui/shvguimanagerimpl.h"

// forward declares
class SHVDrawWin32;
class SHVDrawPaintWin32;
class SHVDrawGetDCWin32;
class SHVDrawWindowWin32;
class SHVMenuWin32;


//-=========================================================================================================
/// SHVGUIManagerWin32 - Module for registering control types, and instantiating them
/**
 * This class is a SHIVA module that can be used to keeping default control factories, as well as
 * allowing custom control factories to be registered.
 */

class SHVGUIManagerWin32 : public SHVGUIManagerImpl
{
public:


	// Constructor
	SHVGUIManagerWin32(SHVModuleList& modules);


	// From SHVModule
	virtual SHVBool Register();
	virtual void Unregister();

	// CreateFont
	virtual SHVFont* CreateFont(const SHVStringC name, int height, int styles = SHVFont::StyleNormal);

	// Factories
	virtual SHVColor* CreateColor(SHVColor::ColorVal r, SHVColor::ColorVal g, SHVColor::ColorVal b);
	virtual SHVPen* CreatePen(SHVColor* color, int style = SHVPen::StyleDefault, int width = 1);
	virtual SHVBrush* CreateBrush(SHVColor* color, int style = SHVBrush::StyleDefault);
	virtual SHVRegion* CreateRegion(SHVControlContainer* container);
	virtual SHVMenu* CreatePopupMenu(SHVEventSubscriberBase* subscriber, SHVControl* parent);

	// Factories for template inherited classes
	virtual SHVFormImplementer* ContructFormImplementer(SHVFormBase* owner, SHVGUIManager* manager, SHVControlContainer* controlContainer, SHVString8C entityName);

	// Create draw context
	virtual SHVDrawWin32* CreateDraw(HDC dc);
	virtual SHVDrawPaintWin32* CreateDrawPaint(HWND window);

	// Create win32 color
	virtual SHVColor* CreateColor(COLORREF clr);

	// PreTranslateMessage
	virtual bool PreTranslateMessage(MSG* message);


protected:
friend class SHVMenuWin32;
	///\cond INTERNAL
	int CreateCommandID(HANDLE handle, SHVInt value);
	HANDLE CommandIDToHandle(int cmdID);
	SHVInt CommandIDToValue(int cmdID);
	void RemoveCommandIDs(HANDLE handle);

	struct CommandID
	{
		HANDLE Handle;
		SHVInt Value;

		inline CommandID(HANDLE handle, SHVInt value) : Handle(handle), Value(value) {}
	};
	SHVVector<CommandID> CommandIDMap;
	SHVHashTable<HMENU,SHVMenuRef,HMENU,SHVMenu*> MenuMap;
	///\endcond
};

#endif
