#include "stdafx.h"
#include "../../../include/platformspc.h"
#include "../include/shvsocketserverimpl.h"
#include "shvsocketserverfactory.h"


//=========================================================================================================
// SHVModuleFactorySocketServer - factory for module library SocketServer
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVModuleFactorySocketServer::SHVModuleFactorySocketServer(SHVModuleList& modules)
	: SHVModuleFactory(), Modules(modules)
{
}

/*************************************
 * CheckVersion
 *************************************/
SHVBool SHVModuleFactorySocketServer::CheckVersion(int verMajor,int verMinor,int verRelease)
{
	return ( verMajor == __SOCKETSERVER_VERSION_MAJOR &&
			 verMinor == __SOCKETSERVER_VERSION_MINOR &&
			 verRelease == __SOCKETSERVER_VERSION_RELEASE );
}

/*************************************
 * GetMajor
 *************************************/
SHVInt SHVModuleFactorySocketServer::GetMajor()
{
	return __SOCKETSERVER_VERSION_MAJOR;
}

/*************************************
 * GetMinor
 *************************************/
SHVInt SHVModuleFactorySocketServer::GetMinor()
{
	return __SOCKETSERVER_VERSION_MINOR;
}

/*************************************
 * GetRelease
 *************************************/
SHVInt SHVModuleFactorySocketServer::GetRelease()
{
	return __SOCKETSERVER_VERSION_RELEASE;
}

/*************************************
 * GetBuild
 *************************************/
SHVInt SHVModuleFactorySocketServer::GetBuild()
{
	return __SOCKETSERVER_VERSION_RELEASE;
}

/*************************************
 * GetDescription
 *************************************/
SHVStringBuffer SHVModuleFactorySocketServer::GetDescription()
{
SHVString retVal(_T("Module library for SocketServer"));
	return retVal.ReleaseBuffer();
}

/*************************************
 * ResolveModules
 *************************************/
SHVBool SHVModuleFactorySocketServer::ResolveModules(const SHVString8C symbol)
{
	if (symbol == __MODULESYMBOL_DEFAULTS)
	{
		Modules.AddModule(new SHVSocketServerImpl(Modules));
		return SHVBool::True;
	}

	return SHVBool::False;
}

/*************************************
 * OnEvent
 *************************************/
void SHVModuleFactorySocketServer::OnEvent(SHVEvent* event)
{
	SHVUNUSED_PARAM(event);
}

SHVStringBuffer SHVModuleFactorySocketServer::GetName()
{
	return SHVString(_T("SocketServer")).ReleaseBuffer();
}
