#ifndef __SHIVA_LUAENGINE_LUAENGINE_H
#define __SHIVA_LUAENGINE_LUAENGINE_H

#include "../../framework/shvmodule.h"
#include "shvluascript.h"
#include "shvluaclass.h"
#include "shvluareftype.h"

#define __LUAENGINE_VERSION_MAJOR   0
#define __LUAENGINE_VERSION_MINOR   1
#define __LUAENGINE_VERSION_RELEASE 0


//-=========================================================================================================
/// SHVLuaEngine class
//-=========================================================================================================
/**
 * Module factory for Lua scripts
 */

class SHVLuaEngine : public SHVModule
{
public:


	enum Events
	{
		EventScriptError
	};


	// Script functions
	virtual SHVLuaScript* CreateScript(SHVEventQueue* scriptThread = NULL, int initFuncs = SHVLuaScript::FuncsDefault) = 0;


protected:
	///\cond INTERNAL
	inline SHVLuaEngine(SHVModuleList& modules) : SHVModule(modules,"LuaEngine") {}
	///\endcond
};


#endif
