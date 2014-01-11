#include "stdafx.h"
#include "shiva/include/platformspc.h"
#include "modules/shvsubprocessserverimpl.h"
#include "shvsubprocessfactory.h"


//=========================================================================================================
// SHVModuleFactorySubProcess - factory for module library SubProcess
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVModuleFactorySubProcess::SHVModuleFactorySubProcess(SHVModuleList& modules)
	: SHVModuleFactory(), Modules(modules)
{
}

/*************************************
 * CheckVersion
 *************************************/
SHVBool SHVModuleFactorySubProcess::CheckVersion(int verMajor,int verMinor,int verRelease)
{
	return ( verMajor == __SUBPROCESS_VERSION_MAJOR &&
			 verMinor == __SUBPROCESS_VERSION_MINOR &&
			 verRelease == __SUBPROCESS_VERSION_RELEASE );
}

/*************************************
 * GetMajor
 *************************************/
SHVInt SHVModuleFactorySubProcess::GetMajor()
{
	return __SUBPROCESS_VERSION_MAJOR;
}

/*************************************
 * GetMinor
 *************************************/
SHVInt SHVModuleFactorySubProcess::GetMinor()
{
	return __SUBPROCESS_VERSION_MINOR;
}

/*************************************
 * GetRelease
 *************************************/
SHVInt SHVModuleFactorySubProcess::GetRelease()
{
	return __SUBPROCESS_VERSION_RELEASE;
}

/*************************************
 * GetBuild
 *************************************/
SHVInt SHVModuleFactorySubProcess::GetBuild()
{
	return __SUBPROCESS_VERSION_RELEASE;
}

/*************************************
 * GetDescription
 *************************************/
SHVStringBuffer SHVModuleFactorySubProcess::GetDescription()
{
SHVString retVal(_S("Module library for SubProcess"));
	return retVal.ReleaseBuffer();
}

/*************************************
 * ResolveModules
 *************************************/
SHVBool SHVModuleFactorySubProcess::ResolveModules(const SHVString8C symbol)
{
	if (symbol == __MODULESYMBOL_DEFAULTS || symbol == __MODULESYMBOL_SUBPROCESS)
	{
		Modules.AddModule(new SHVSubProcessServerImpl(Modules));
		return SHVBool::True;
	}

	return SHVBool::False;
}

/*************************************
 * OnEvent
 *************************************/
void SHVModuleFactorySubProcess::OnEvent(SHVEvent* event)
{
	SHVUNUSED_PARAM(event);
}

SHVStringBuffer SHVModuleFactorySubProcess::GetName()
{
	return SHVString(_S("SubProcess")).ReleaseBuffer();
}
