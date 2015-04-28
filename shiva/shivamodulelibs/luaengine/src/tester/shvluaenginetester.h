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

//-=========================================================================================================
/// SHVLuaTesterClass class
/**
 * Implements a Lua class called "TesterClass"  that has 2 methods:
 * SetValue and GetValue. Both relay into Modules.GetConfig().
 * You can use it this way from Lua:
\code
local object = TesterClass.new()
object:SetValue("configname",5)
\endcode
 */

class SHVLuaTesterClass : public SHVLuaClass<SHVLuaTesterClass>
{
public:
	
	struct MetaData
	{
		int ObjCount;
		SHVLuaEngineTester* EngineTester;
		inline MetaData(SHVLuaEngineTester* engineTester) : EngineTester(engineTester) { ObjCount=0; }
	};
	
	SHVLuaTesterClass(SHVLuaMetaClassBase* meta, SHVLuaScript* script, SHVLuaFuncArgs& args);
	virtual ~SHVLuaTesterClass();

	void SetValue(SHVLuaMetaClassBase* meta, SHVLuaScript* script, SHVLuaFuncArgs& args);
	void GetValue(SHVLuaMetaClassBase* meta, SHVLuaScript* script, SHVLuaFuncArgs& args);
	
	static void RegisterMethods(SHVLuaClassMethods& methods);
	
private:
	
	SHVModuleList& Modules;
};

#endif
