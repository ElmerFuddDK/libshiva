#include "stdafx.h"
#include "../../../../include/platformspc.h"

#include "shvdataenginefactory.h"

#define __DATAENGINE_VERSION_MAJOR 0
#define __DATAENGINE_VERSION_MINOR 0
#define __DATAENGINE_VERSION_RELEASE 1

#define __DATAENGINE_VERSION_BUILD 1


//=========================================================================================================
// SHVModuleFactorydataengine - factory for module library dataengine
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVModuleFactorydataengine::SHVModuleFactorydataengine(SHVModuleList& modules)
	: SHVModuleFactory(), Modules(modules)
{
}

/*************************************
 * CheckVersion
 *************************************/
SHVBool SHVModuleFactorydataengine::CheckVersion(int verMajor,int verMinor,int verRelease)
{
	return ( verMajor == __DATAENGINE_VERSION_MAJOR &&
			 verMinor == __DATAENGINE_VERSION_MINOR &&
			 verRelease == __DATAENGINE_VERSION_RELEASE );
}

/*************************************
 * GetMajor
 *************************************/
SHVInt SHVModuleFactorydataengine::GetMajor()
{
	return __DATAENGINE_VERSION_MAJOR;
}

/*************************************
 * GetMinor
 *************************************/
SHVInt SHVModuleFactorydataengine::GetMinor()
{
	return __DATAENGINE_VERSION_MINOR;
}

/*************************************
 * GetRelease
 *************************************/
SHVInt SHVModuleFactorydataengine::GetRelease()
{
	return __DATAENGINE_VERSION_RELEASE;
}

/*************************************
 * GetBuild
 *************************************/
SHVInt SHVModuleFactorydataengine::GetBuild()
{
	return __DATAENGINE_VERSION_BUILD;
}

/*************************************
 * GetName
 *************************************/
SHVStringBuffer SHVModuleFactorydataengine::GetName()
{
	return SHVString(_S("dataengine")).ReleaseBuffer();
}

/*************************************
 * GetDescription
 *************************************/
SHVStringBuffer SHVModuleFactorydataengine::GetDescription()
{
	SHVString retVal(_S("Module library for dataengine"));
	return retVal.ReleaseBuffer();
}

/*************************************
 * ResolveModules
 *************************************/
SHVBool SHVModuleFactorydataengine::ResolveModules(const SHVString8C symbol)
{
	if (symbol == __MODULESYMBOL_DEFAULTS)
	{
		// Modules.AddModule(new SHVdataengine(Modules));
		// return SHVBool::True;
	}

	return SHVBool::False;
}

/*************************************
 * OnEvent
 *************************************/
void SHVModuleFactorydataengine::OnEvent(SHVEvent* event)
{
	SHVUNUSED_PARAM(event);
}
