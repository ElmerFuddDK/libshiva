#include "stdafx.h"
#include "shiva/include/platformspc.h"

#include "shvshvtemplateshvimpl.h"


//=========================================================================================================
// SHVSHVTemplateSHVImpl class
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVSHVTemplateSHVImpl::SHVSHVTemplateSHVImpl(SHVModuleList& modules) : SHVSHVTemplateSHV(modules)
{
}

/*************************************
 * Destructor
 *************************************/
SHVSHVTemplateSHVImpl::~SHVSHVTemplateSHVImpl()
{
}

/*************************************
 * Register
 *************************************/
SHVBool SHVSHVTemplateSHVImpl::Register()
{
	return SHVSHVTemplateSHV::Register();
}

/*************************************
 * PostRegister
 *************************************/
void SHVSHVTemplateSHVImpl::PostRegister()
{
	SHVSHVTemplateSHV::PostRegister();
}

/*************************************
 * Unregister
 *************************************/
void SHVSHVTemplateSHVImpl::Unregister()
{
	SHVSHVTemplateSHV::Unregister();
}
