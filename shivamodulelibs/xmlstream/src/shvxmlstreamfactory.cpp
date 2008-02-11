#include "stdafx.h"
#include "../../../include/platformspc.h"


//=========================================================================================================
// SHVModuleFactoryxmlstream - factory for module library xmlstream
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVModuleFactoryxmlstream::SHVModuleFactoryxmlstream(SHVModuleList& modules)
	: SHVModuleFactory(), Modules(modules)
{
}

/*************************************
 * CheckVersion
 *************************************/
SHVBool SHVModuleFactoryxmlstream::CheckVersion(int verMajor,int verMinor,int verRelease)
{
	return ( verMajor == __XMLSTREAM_VERSION_MAJOR &&
			 verMinor == __XMLSTREAM_VERSION_MINOR &&
			 verRelease == __XMLSTREAM_VERSION_RELEASE );
}

/*************************************
 * GetMajor
 *************************************/
SHVInt SHVModuleFactoryxmlstream::GetMajor()
{
	return __XMLSTREAM_VERSION_MAJOR;
}

/*************************************
 * GetMinor
 *************************************/
SHVInt SHVModuleFactoryxmlstream::GetMinor()
{
	return __XMLSTREAM_VERSION_MINOR;
}

/*************************************
 * GetRelease
 *************************************/
SHVInt SHVModuleFactoryxmlstream::GetRelease()
{
	return __XMLSTREAM_VERSION_RELEASE;
}

/*************************************
 * GetBuild
 *************************************/
SHVInt SHVModuleFactoryxmlstream::GetBuild()
{
	return __XMLSTREAM_VERSION_BUILD;
}

/*************************************
 * GetDescription
 *************************************/
SHVStringBuffer SHVModuleFactoryxmlstream::GetDescription()
{
SHVString retVal(_T("Module library for xmlstream"));
	return retVal.ReleaseBuffer();
}

/*************************************
 * ResolveModules
 *************************************/
SHVBool SHVModuleFactoryxmlstream::ResolveModules(const SHVString8C symbol)
{
	if (symbol == __MODULESYMBOL_DEFAULTS)
	{
		modules.AddModule(new SHVxmlstream(Modules));
		return SHVBool::True;
	}

	return SHVBool::False;
}

/*************************************
 * OnEvent
 *************************************/
void SHVModuleFactoryxmlstream::OnEvent(SHVEvent* event)
{
}
