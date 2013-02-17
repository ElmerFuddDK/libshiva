#ifndef __SHIVA_LUAENGINEIMPL_LUAENGINE_H
#define __SHIVA_LUAENGINEIMPL_LUAENGINE_H

#include "shiva/include/modules/luaengine/shvluaengine.h"
#include "shiva/include/utils/shvlist.h"
#include "shiva/include/threadutils/shvmutex.h"
class SHVLuaScriptImpl;


//-=========================================================================================================
/// SHVLuaEngineImpl class
//-=========================================================================================================
/**
 * Implementation of SHVLuaEngine
 */

class SHVLuaEngineImpl : public SHVLuaEngine
{
public:


	// constructor
	SHVLuaEngineImpl(SHVModuleList& modules);
	~SHVLuaEngineImpl();


	// module functions
	virtual SHVBool Register();
	virtual void PostRegister();
	virtual void Unregister();


	// Script functions
	virtual SHVLuaScript* CreateScript(SHVEventQueue* scriptThread = NULL, int initFuncs = SHVLuaScript::FuncsDefault);

	
private:
friend class SHVLuaScriptImpl;

	///\cond INTERNAL
	SHVMutex Lock;
	SHVList<SHVLuaScriptImpl*> ActiveScripts;
	///\endcond

};

#endif
