#ifndef __SHIVA_MODULES_LUAENGINETESTER_H
#define __SHIVA_MODULES_LUAENGINETESTER_H

#include "shiva/include/framework/shvmodule.h"
#include "shiva/include/modules/luaengine/shvluaengine.h"

//-=========================================================================================================
/// SHVLuaEngineTester class
/**
 */

class SHVLuaEngineTester : public SHVModule
{
public:

	SHVLuaEngineTester(SHVModuleList& modules);

	virtual SHVBool Register();
	virtual void PostRegister();
	virtual void Unregister();

	virtual void OnEvent(SHVEvent* event);


private:

	void LuaTestFunc(SHVLuaScript* script, SHVLuaFuncArgs& args);

	SHVLuaEngine* LuaEngine;
	SHVLuaScriptRef LuaScript;
};

#endif
