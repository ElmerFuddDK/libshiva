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

	return SHVModule::Register();
}
