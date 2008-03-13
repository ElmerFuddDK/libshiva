#include "stdafx.h"
#include "../../../include/platformspc.h"
#include "shvxmlstreamfactory.h"
#include "../include/xmlstreamimpl/shvxmlstream_impl.h"

//=========================================================================================================
// SHVModuleFactoryXmlStream - factory for module library XmlStream
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVModuleFactoryXmlStream::SHVModuleFactoryXmlStream(SHVModuleList& modules)
	: SHVModuleFactory(), Modules(modules)
{
}

/*************************************
 * CheckVersion
 *************************************/
SHVBool SHVModuleFactoryXmlStream::CheckVersion(int verMajor,int verMinor,int verRelease)
{
	return ( verMajor == __XMLSTREAM_VERSION_MAJOR &&
			 verMinor == __XMLSTREAM_VERSION_MINOR &&
			 verRelease == __XMLSTREAM_VERSION_RELEASE );
}

/*************************************
 * GetMajor
 *************************************/
SHVInt SHVModuleFactoryXmlStream::GetMajor()
{
	return __XMLSTREAM_VERSION_MAJOR;
}

/*************************************
 * GetMinor
 *************************************/
SHVInt SHVModuleFactoryXmlStream::GetMinor()
{
	return __XMLSTREAM_VERSION_MINOR;
}

/*************************************
 * GetRelease
 *************************************/
SHVInt SHVModuleFactoryXmlStream::GetRelease()
{
	return __XMLSTREAM_VERSION_RELEASE;
}

/*************************************
 * GetBuild
 *************************************/
SHVInt SHVModuleFactoryXmlStream::GetBuild()
{
	return __XMLSTREAM_VERSION_BUILD;
}

/*************************************
 * GetDescription
 *************************************/
SHVStringBuffer SHVModuleFactoryXmlStream::GetDescription()
{
SHVString retVal(_T("Module library for XmlStream"));
	return retVal.ReleaseBuffer();
}

/*************************************
 * GetName
 *************************************/
SHVStringBuffer SHVModuleFactoryXmlStream::GetName()
{
SHVString retVal(_T("XmlStream"));
	return retVal.ReleaseBuffer();
}

/*************************************
 * ResolveModules
 *************************************/
SHVBool SHVModuleFactoryXmlStream::ResolveModules(const SHVString8C symbol)
{
	if (symbol == __MODULESYMBOL_DEFAULTS)
	{
		Modules.AddModule(new SHVXmlStream_impl(Modules));
		return SHVBool::True;
	}

	return SHVBool::False;
}

/*************************************
 * OnEvent
 *************************************/
void SHVModuleFactoryXmlStream::OnEvent(SHVEvent* event)
{
}
