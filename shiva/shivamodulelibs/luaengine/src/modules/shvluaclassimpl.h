#ifndef __SHIVA_LUAENGINEIMPL_LUACLASS_H
#define __SHIVA_LUAENGINEIMPL_LUACLASS_H

#include "shiva/include/modules/luaengine/shvluaclass.h"
#include "shiva/include/utils/shvvector.h"
class SHVLuaScriptImpl;


//-=========================================================================================================
/// SHVLuaClassImpl class
//-=========================================================================================================
/**
 * Embeds the SHVLuaClass into a Lua script.
 */
class SHVLuaClassImpl : public SHVLuaClassMethods
{
public:

	SHVLuaClassImpl(SHVLuaMetaClassBase* meta);
	virtual ~SHVLuaClassImpl();
	
	SHVBool Initialize(const char* name, SHVLuaScriptImpl* script);
	
	// From SHVLuaClassMethods
	virtual void Add(const char* name, SHVLuaMethodBase* method);

private:
	
	static int LuaConstructor(void* state);
	static int LuaDestructor(void* state);
	static int LuaMethod(void* state);
	
	SHVLuaScriptImpl* Script;
	SHVLuaMetaClassBasePtr Meta;
	SHVString8 MetaName;
	SHVVector<SHVLuaMethodBase> Methods;
};

#endif
