#include "stdafx.h"
#include "../../../../../include/platformspc.h"

#include "shvcontroltester.h"


//=========================================================================================================
// SHVControlTester - Module for testing controls
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVControlTester::SHVControlTester(SHVModuleList& modules) : SHVModule(modules,"ControlTester")
{
}

/*************************************
 * Register
 *************************************/
SHVBool SHVControlTester::Register()
{
	if (!SHVModuleResolver<SHVGUIManager>(Modules,GUIManager,"GUIManager"))
		return false;

	return SHVModule::Register();
}

/*************************************
 * PostRegister
 *************************************/
void SHVControlTester::PostRegister()
{
	///\todo: add your controls here

	SHVModule::PostRegister();
}

/*************************************
 * Unregister
 *************************************/
void SHVControlTester::Unregister()
{
	Label = NULL;
	EditBox = NULL;
	Button = NULL;

	SHVModule::Unregister();
}
