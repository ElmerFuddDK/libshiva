#ifndef __SHIVA_LUAENGINEIMPL_REFOBJECTTYPE_H
#define __SHIVA_LUAENGINEIMPL_REFOBJECTTYPE_H

#include "shiva/include/utils/shvrefobject.h"

#define SHVLUA_UDATANAME "SHVRefObj"


//-=========================================================================================================
/// SHVLuaRefObjectType
//-=========================================================================================================
/**
 * Implements functions for handling refobjects as lua variables
 */

class SHVLuaRefObjectType
{
public:
	static void RegisterType(void* state);
	static bool IsRef(void* state, int idx, const char** typeID);
	static bool IsRefByType(void* state, int idx, const char* typeID);
	static SHVRefObject* ToRef(void* state, int idx, const char** typeID);
	static SHVRefObject* ToRefByType(void* state, int idx, const char* typeID);
	static void PushRef(void* state, SHVRefObject* val, const char* typeID);

private:
	static int LuaGc(void* state);
	struct LuaRefObject { SHVRefObject* Obj; const char* Type; };
};

#endif
