#include "stdafx.h"
#include "shiva/include/platformspc.h"
#include "modules/shvluaengineimpl.h"
#include "shvluaenginefactory.h"


//=========================================================================================================
// SHVModuleFactoryLuaEngine - factory for module library LuaEngine
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVModuleFactoryLuaEngine::SHVModuleFactoryLuaEngine(SHVModuleList& modules)
	: SHVModuleFactory(), Modules(modules)
{
}

/*************************************
 * CheckVersion
 *************************************/
SHVBool SHVModuleFactoryLuaEngine::CheckVersion(int verMajor,int verMinor,int verRelease)
{
	return ( verMajor == __LUAENGINE_VERSION_MAJOR &&
			 verMinor == __LUAENGINE_VERSION_MINOR &&
			 verRelease == __LUAENGINE_VERSION_RELEASE );
}

/*************************************
 * GetMajor
 *************************************/
SHVInt SHVModuleFactoryLuaEngine::GetMajor()
{
	return __LUAENGINE_VERSION_MAJOR;
}

/*************************************
 * GetMinor
 *************************************/
SHVInt SHVModuleFactoryLuaEngine::GetMinor()
{
	return __LUAENGINE_VERSION_MINOR;
}

/*************************************
 * GetRelease
 *************************************/
SHVInt SHVModuleFactoryLuaEngine::GetRelease()
{
	return __LUAENGINE_VERSION_RELEASE;
}

/*************************************
 * GetBuild
 *************************************/
SHVInt SHVModuleFactoryLuaEngine::GetBuild()
{
	return __LUAENGINE_VERSION_RELEASE;
}

/*************************************
 * GetDescription
 *************************************/
SHVStringBuffer SHVModuleFactoryLuaEngine::GetDescription()
{
SHVString retVal(_S("Module library for LuaEngine"));
	return retVal.ReleaseBuffer();
}

/*************************************
 * ResolveModules
 *************************************/
SHVBool SHVModuleFactoryLuaEngine::ResolveModules(const SHVString8C symbol)
{
	if (symbol == __MODULESYMBOL_DEFAULTS)
	{
		Modules.AddModule(new SHVLuaEngineImpl(Modules));
		return SHVBool::True;
	}

	return SHVBool::False;
}

/*************************************
 * OnEvent
 *************************************/
void SHVModuleFactoryLuaEngine::OnEvent(SHVEvent* event)
{
	SHVUNUSED_PARAM(event);
}

SHVStringBuffer SHVModuleFactoryLuaEngine::GetName()
{
	return SHVString(_S("LuaEngine")).ReleaseBuffer();
}
