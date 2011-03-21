#include "stdafx.h"
#include "shiva/include/platformspc.h"

#include "shvshvtemplateshv.h"


//=========================================================================================================
// SHVSHVTemplateSHV class
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVSHVTemplateSHV::SHVSHVTemplateSHV(SHVModuleList& modules) : SHVModule(modules,"SHVTemplateSHV")
{
}

/*************************************
 * Register
 *************************************/
SHVBool SHVSHVTemplateSHV::Register()
{
	// resolve the GUI manager
	if (!SHVModuleResolver<SHVGUIManager>(Modules,GUIManager,"GUIManager"))
		return false;

	return SHVModule::Register();
}

/*************************************
 * PostRegister
 *************************************/
void SHVSHVTemplateSHV::PostRegister()
{
	SHVModule::PostRegister();

	// Create form main and initialize it
	FormMain = new SHVFormMain(GUIManager,GUIManager->GetMainWindow());
	FormMain->InitializeForm();
}

/*************************************
 * Unregister
 *************************************/
void SHVSHVTemplateSHV::Unregister()
{
	SHVModule::Unregister();
	
	// Always cleanup resources from other modules here
	FormMain = NULL;
}
