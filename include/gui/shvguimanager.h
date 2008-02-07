#ifndef __SHIVA_GUI_MANAGER_H
#define __SHIVA_GUI_MANAGER_H

#include "../../include/framework/shvmodule.h"
#include "shvcontrolcreatorbase.h"
#include "shvcontrolcontainer.h"
#include "shvcontroledit.h"
#include "shvcontrollabel.h"
#include "shvcontrolbutton.h"



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
		CfgRegionHorizMargin,
		CfgRegionVertMargin,
		CfgNativeOptions = 100
	};

	enum Events {
		EventPreTranslateMessage
	};


	// Register controls
	virtual SHVBool RegisterFactory(int controlType, int controlSubType, SHVControlCreatorBase* factory) = 0;
	virtual SHVBool RegisterFactory(const SHVString8C& controlName, SHVControlCreatorBase* factory) = 0;


	// Control factory
	virtual SHVControl* NewControl(int controlType, int controlSubType) = 0;
	virtual SHVControl* NewControl(const SHVString8C& controlName) = 0;


	virtual SHVModuleList& GetModuleList() = 0;
	virtual SHVConfig& GetConfig() = 0;

	virtual SHVControlContainer* GetMainWindow() = 0;

	inline SHVControlEdit* NewEdit(int subType = SHVControlEdit::SubTypeDefault);
	inline SHVControlLabel* NewLabel(int subType = SHVControlLabel::SubTypeDefault);
	inline SHVControlButton* NewButton(int subType = SHVControlButton::SubTypeDefault);

protected:
	///\cond INTERNAL
	inline SHVGUIManager(SHVModuleList& modules) : SHVModule(modules,"GUIManager") {}
	///\endcond
};



// ============================================= implementation ============================================= //

/*************************************
 * CreateEdit
 *************************************/
SHVControlEdit* SHVGUIManager::NewEdit(int subType)
{
	return (SHVControlEdit*)NewControl(SHVControl::TypeEdit,subType);
}

/*************************************
 * CreateLabel
 *************************************/
SHVControlLabel* SHVGUIManager::NewLabel(int subType)
{
	return (SHVControlLabel*)NewControl(SHVControl::TypeLabel,subType);
}

/*************************************
 * NewButton
 *************************************/
SHVControlButton* SHVGUIManager::NewButton(int subType)
{
	return (SHVControlButton*)NewControl(SHVControl::TypeButton,subType);
}


#endif
