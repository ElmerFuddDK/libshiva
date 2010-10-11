#include "stdafx.h"
#include "shiva/include/platformspc.h"
#include "shvjsonstreamfactory.h"
#include "shvjsonstreamimpl.h"

//=========================================================================================================
// SHVModuleFactoryJsonStream - factory for module library JsonStream
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVModuleFactoryJsonStream::SHVModuleFactoryJsonStream(SHVModuleList& modules)
  : SHVModuleFactory(), Modules(modules)
{
}

/*************************************
 * CheckVersion
 *************************************/
SHVBool SHVModuleFactoryJsonStream::CheckVersion(int verMajor,int verMinor,int verRelease)
{
  return ( verMajor == __JSONSTREAM_VERSION_MAJOR &&
	   verMinor == __JSONSTREAM_VERSION_MINOR &&
	   verRelease == __JSONSTREAM_VERSION_RELEASE );
}

/*************************************
 * GetMajor
 *************************************/
SHVInt SHVModuleFactoryJsonStream::GetMajor()
{
  return __JSONSTREAM_VERSION_MAJOR;
}

/*************************************
 * GetMinor
 *************************************/
SHVInt SHVModuleFactoryJsonStream::GetMinor()
{
  return __JSONSTREAM_VERSION_MINOR;
}

/*************************************
 * GetRelease
 *************************************/
SHVInt SHVModuleFactoryJsonStream::GetRelease()
{
  return __JSONSTREAM_VERSION_RELEASE;
}

/*************************************
 * GetBuild
 *************************************/
SHVInt SHVModuleFactoryJsonStream::GetBuild()
{
  return __JSONSTREAM_VERSION_RELEASE;
}

/*************************************
 * GetDescription
 *************************************/
SHVStringBuffer SHVModuleFactoryJsonStream::GetDescription()
{
SHVString retVal(_S("Module library for json streaming"));
  return retVal.ReleaseBuffer();
}

/*************************************
 * ResolveModules
 *************************************/
SHVBool SHVModuleFactoryJsonStream::ResolveModules(const SHVString8C symbol)
{
  if (symbol == __MODULESYMBOL_DEFAULTS)
  {
	Modules.AddModule(new SHVJsonStreamImpl(Modules));
    return SHVBool::True;
  }

  return SHVBool::False;
}

/*************************************
 * OnEvent
 *************************************/
void SHVModuleFactoryJsonStream::OnEvent(SHVEvent* event)
{
  SHVUNUSED_PARAM(event);
}

SHVStringBuffer SHVModuleFactoryJsonStream::GetName()
{
  return SHVString(_S("JsonStream")).ReleaseBuffer();
}
