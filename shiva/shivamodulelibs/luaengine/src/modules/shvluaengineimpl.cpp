#include "stdafx.h"
#include "shiva/include/platformspc.h"

#include "shvluaengineimpl.h"
#include "shvluascriptimpl.h"


//=========================================================================================================
// SHVLuaEngineImpl class
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVLuaEngineImpl::SHVLuaEngineImpl(SHVModuleList& modules) : SHVLuaEngine(modules)
{
}

/*************************************
 * Destructor
 *************************************/
SHVLuaEngineImpl::~SHVLuaEngineImpl()
{
}

/*************************************
 * Register
 *************************************/
SHVBool SHVLuaEngineImpl::Register()
{
	return SHVLuaEngine::Register();
}

/*************************************
 * PostRegister
 *************************************/
void SHVLuaEngineImpl::PostRegister()
{
	SHVLuaEngine::PostRegister();
}

/*************************************
 * Unregister
 *************************************/
void SHVLuaEngineImpl::Unregister()
{
SHVLuaScript* scr;
	SHVLuaEngine::Unregister();
	
	while (ActiveScripts.GetCount())
	{
		scr = ActiveScripts.PopHead();
		scr->StopScript();
	}
}

/*************************************
 * CreateScript
 *************************************/
SHVLuaScript* SHVLuaEngineImpl::CreateScript(SHVEventQueue* scriptThread, int initFuncs)
{
	if (!scriptThread)
		scriptThread = &Modules;
	return new SHVLuaScriptImpl(*this, *scriptThread, initFuncs);
}
