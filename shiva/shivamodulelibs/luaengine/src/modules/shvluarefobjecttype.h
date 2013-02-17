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
	static bool IsRef(void* state, int idx);
	static SHVRefObject* ToRef(void* state, int idx);
	static void PushRef(void* state, SHVRefObject* val);

private:
	static int LuaGc(void* state);
};

#endif
