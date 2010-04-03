#include "stdafx.h"
#include "shiva/include/platformspc.h"
#include "modules/shvshvtemplateshvimpl.h"
#include "shvshvtemplateshvfactory.h"


//=========================================================================================================
// SHVModuleFactorySHVTemplateSHV - factory for module library SHVTemplateSHV
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVModuleFactorySHVTemplateSHV::SHVModuleFactorySHVTemplateSHV(SHVModuleList& modules)
	: SHVModuleFactory(), Modules(modules)
{
}

/*************************************
 * CheckVersion
 *************************************/
SHVBool SHVModuleFactorySHVTemplateSHV::CheckVersion(int verMajor,int verMinor,int verRelease)
{
	return ( verMajor == __SHVTEMPLATESHV_VERSION_MAJOR &&
			 verMinor == __SHVTEMPLATESHV_VERSION_MINOR &&
			 verRelease == __SHVTEMPLATESHV_VERSION_RELEASE );
}

/*************************************
 * GetMajor
 *************************************/
SHVInt SHVModuleFactorySHVTemplateSHV::GetMajor()
{
	return __SHVTEMPLATESHV_VERSION_MAJOR;
}

/*************************************
 * GetMinor
 *************************************/
SHVInt SHVModuleFactorySHVTemplateSHV::GetMinor()
{
	return __SHVTEMPLATESHV_VERSION_MINOR;
}

/*************************************
 * GetRelease
 *************************************/
SHVInt SHVModuleFactorySHVTemplateSHV::GetRelease()
{
	return __SHVTEMPLATESHV_VERSION_RELEASE;
}

/*************************************
 * GetBuild
 *************************************/
SHVInt SHVModuleFactorySHVTemplateSHV::GetBuild()
{
	return __SHVTEMPLATESHV_VERSION_RELEASE;
}

/*************************************
 * GetDescription
 *************************************/
SHVStringBuffer SHVModuleFactorySHVTemplateSHV::GetDescription()
{
SHVString retVal(_T("Module library for SHVTemplateSHV"));
	return retVal.ReleaseBuffer();
}

/*************************************
 * ResolveModules
 *************************************/
SHVBool SHVModuleFactorySHVTemplateSHV::ResolveModules(const SHVString8C symbol)
{
	if (symbol == __MODULESYMBOL_DEFAULTS)
	{
		Modules.AddModule(new SHVSHVTemplateSHVImpl(Modules));
		return SHVBool::True;
	}

	return SHVBool::False;
}

/*************************************
 * OnEvent
 *************************************/
void SHVModuleFactorySHVTemplateSHV::OnEvent(SHVEvent* event)
{
	SHVUNUSED_PARAM(event);
}

SHVStringBuffer SHVModuleFactorySHVTemplateSHV::GetName()
{
	return SHVString(_T("SHVTemplateSHV")).ReleaseBuffer();
}
