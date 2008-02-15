#ifndef __SHIVA_GUIIMPL_MANAGER_H
#define __SHIVA_GUIIMPL_MANAGER_H

#include "../include/shvguimanager.h"
#include "../../include/utils/shvhashtable.h"


//-=========================================================================================================
/// SHVGUIManagerImpl - Module for registering control types, and instantiating them
/**
 * This class is a SHIVA module that can be used to keeping default control factories, as well as
 * allowing custom control factories to be registered.
 */

class SHVGUIManagerImpl : public SHVGUIManager
{
public:

	// Constructor
	SHVGUIManagerImpl(SHVModuleList& modules);


	// From SHVModule
	virtual SHVBool Register();
	virtual void Unregister();


	// Font creation function
	virtual SHVFont* CreateFont(const SHVStringC name, int height, int styles = SHVFont::StyleNormal) = 0;

	// Factories
	virtual SHVColor* CreateColor(SHVColor::ColorVal r, SHVColor::ColorVal g, SHVColor::ColorVal b) = 0;
	virtual SHVPen* CreatePen(SHVColor* color, int style = SHVPen::StyleDefault, int width = 1) = 0;
	virtual SHVBrush* CreateBrush(SHVColor* color, int style = SHVBrush::StyleDefault) = 0;
	virtual SHVRegion* CreateRegion(SHVControlContainer* container) = 0;


	// Register controls
	virtual SHVBool RegisterFactory(int controlType, int controlSubType, SHVControlCreatorBase* factory);
	virtual SHVBool RegisterFactory(const SHVString8C& controlName, SHVControlCreatorBase* factory);


	// Control factory
	virtual SHVControl* NewControl(int controlType, int controlSubType);
	virtual SHVControl* NewControl(const SHVString8C& controlName);


	virtual SHVModuleList& GetModuleList();
	virtual SHVConfig& GetConfig();

	virtual SHVControlContainer* GetMainWindow();
	inline void SetMainWindow(SHVControlContainer* mainWindow);


protected:
	///\cond INTERNAL
	typedef struct ControlPair {
		int Type;
		int SubType;
		
		inline ControlPair(int type, int subType) : Type(type), SubType(subType)  {}
		inline operator size_t() const { return Type+(SubType<<4); }
		inline bool operator==(const ControlPair& p) const { return p.Type == Type && p.SubType == SubType; }
	};
	
	SHVHashTable<ControlPair,SHVControlCreatorBaseRef> ControlsByType;
	SHVHashTable<SHVString8,SHVControlCreatorBaseRef,const SHVString8C> ControlsByName;
	SHVControlContainerRef MainWindow;
	///\endcond
};


// ============================================ implementation ============================================ //

/*************************************
 * SetMainWindow
 *************************************/
void SHVGUIManagerImpl::SetMainWindow(SHVControlContainer* mainWindow)
{
	MainWindow = mainWindow;
}

#endif
