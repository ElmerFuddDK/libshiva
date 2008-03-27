#include "stdafx.h"
#include "../../../include/platformspc.h"
#include "../include/shvdataengineimpl.h"
#include "shvdataenginefactory.h"


//=========================================================================================================
// SHVModuleFactoryDataEngine - factory for module library DataEngine
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVModuleFactoryDataEngine::SHVModuleFactoryDataEngine(SHVModuleList& modules)
	: SHVModuleFactory(), Modules(modules)
{
}

/*************************************
 * CheckVersion
 *************************************/
SHVBool SHVModuleFactoryDataEngine::CheckVersion(int verMajor,int verMinor,int verRelease)
{
	return ( verMajor == __DATAENGINE_VERSION_MAJOR &&
			 verMinor == __DATAENGINE_VERSION_MINOR &&
			 verRelease == __DATAENGINE_VERSION_RELEASE );
}

/*************************************
 * GetMajor
 *************************************/
SHVInt SHVModuleFactoryDataEngine::GetMajor()
{
	return __DATAENGINE_VERSION_MAJOR;
}

/*************************************
 * GetMinor
 *************************************/
SHVInt SHVModuleFactoryDataEngine::GetMinor()
{
	return __DATAENGINE_VERSION_MINOR;
}

/*************************************
 * GetRelease
 *************************************/
SHVInt SHVModuleFactoryDataEngine::GetRelease()
{
	return __DATAENGINE_VERSION_RELEASE;
}

/*************************************
 * GetBuild
 *************************************/
SHVInt SHVModuleFactoryDataEngine::GetBuild()
{
	return __DATAENGINE_VERSION_RELEASE;
}

/*************************************
 * GetDescription
 *************************************/
SHVStringBuffer SHVModuleFactoryDataEngine::GetDescription()
{
SHVString retVal(_T("Module library for DataEngine"));
	return retVal.ReleaseBuffer();
}

/*************************************
 * ResolveModules
 *************************************/
SHVBool SHVModuleFactoryDataEngine::ResolveModules(const SHVString8C symbol)
{
	if (symbol == __MODULESYMBOL_DEFAULTS)
	{
		Modules.AddModule(new SHVDataEngineImpl(Modules));
		return SHVBool::True;
	}

	return SHVBool::False;
}

/*************************************
 * OnEvent
 *************************************/
void SHVModuleFactoryDataEngine::OnEvent(SHVEvent* event)
{
}

SHVStringBuffer SHVModuleFactoryDataEngine::GetName()
{
	return SHVString(_T("DataEngine")).ReleaseBuffer();
}
