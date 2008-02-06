#ifndef __SHIVA_GUI_MANAGER_H
#define __SHIVA_GUI_MANAGER_H

#include "../../include/framework/shvmodule.h"
#include "shvcontrolcreatorbase.h"



//-=========================================================================================================
/// SHVGUIManager - Module for registering control types, and instantiating them
/**
 * This class is a SHIVA module that can be used to keeping default control factories, as well as
 * allowing custom control factories to be registered.
 */

class SHVGUIManager : public SHVModule
{
public:


	// enums
	enum {
		CfgInstanceHandle,
		CfgNativeOptions = 100
	};

	enum Events {
		EventPreTranslateMessage
	};


	// Register controls
	virtual SHVBool RegisterFactory(int controlType, int controlSubType, SHVControlCreatorBase* factory) = 0;
	virtual SHVBool RegisterFactory(const SHVString8C& controlName, SHVControlCreatorBase* factory) = 0;


	// Control factory
	virtual SHVControl* CreateControl(int controlType, int controlSubType) = 0;
	virtual SHVControl* CreateControl(const SHVString8C& controlName) = 0;


	virtual SHVModuleList& GetModuleList() = 0;
	virtual SHVConfig& GetConfig() = 0;


protected:
	///\cond INTERNAL
	inline SHVGUIManager(SHVModuleList& modules) : SHVModule(modules,"ControlFactory") {}
	///\endcond
};

#endif
