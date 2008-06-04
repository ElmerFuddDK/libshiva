#ifndef __SHIVA_GUI_REGISTERBITMAP_H
#define __SHIVA_GUI_REGISTERBITMAP_H

#include "shvguimanager.h"
#include "../framework/shveventdata.h"
#include "../utils/shvdll.h"


//-=========================================================================================================
/// SHVRegisterBitmap - wrapper for registering bitmaps into the gui manager


class SHVRegisterBitmap
{
public:
#ifdef __SHIVA_WIN32
	struct Registration {
		int ResourceID;
		int BitmapID;
	};


	inline SHVRegisterBitmap(SHVModuleList& modules, Registration* regs);
	inline SHVRegisterBitmap(SHVModuleList& modules, Registration* regs, SHVDll& context);
	inline SHVRegisterBitmap(SHVModuleList& modules, Registration* regs, HINSTANCE hInstance);

	inline static void RegisterBitmaps(SHVModuleList& modules, Registration* regs, HINSTANCE hInstance);
#endif	
};

#ifdef __SHIVA_WIN32
class SHVBitmapResourceMapWin32 : public SHVRefObject
{
public:
	HINSTANCE hInstance;
	int ResourceID;
	inline SHVBitmapResourceMapWin32(HINSTANCE hInst, int resID) : hInstance(hInst), ResourceID(resID) {}
};
#endif


// ============================================= implementation ============================================= //

/*************************************
 * Constructors
 *************************************/
#ifdef __SHIVA_WIN32
SHVRegisterBitmap::SHVRegisterBitmap(SHVModuleList& modules, Registration* regs)
{
	RegisterBitmaps(modules,regs,(HINSTANCE)modules.GetConfig(SHVModuleList::CfgGUI).FindPtr(SHVGUIManager::CfgInstanceHandle).ToPtr());
}
SHVRegisterBitmap::SHVRegisterBitmap(SHVModuleList& modules, Registration* regs, SHVDll& context)
{
	RegisterBitmaps(modules,regs,context.Base().GetNativeHandle());
}
SHVRegisterBitmap::SHVRegisterBitmap(SHVModuleList& modules, Registration* regs, HINSTANCE hInstance)
{
	RegisterBitmaps(modules,regs,hInstance);
}

/*************************************
 * RegisterBitmaps
 *************************************/
void SHVRegisterBitmap::RegisterBitmaps(SHVModuleList& modules, Registration* regs, HINSTANCE hInstance)
{
	for (;regs && regs->ResourceID;regs++)
	{
		modules.GetConfig(SHVModuleList::CfgBitmapResourceMap).SetRef(regs->BitmapID,new SHVBitmapResourceMapWin32(hInstance,regs->ResourceID));
	}
}
#endif

#endif
