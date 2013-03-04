#include "stdafx.h"
#include "shiva/include/platformspc.h"

#include "shvluafuncargsimpl.h"
#include "shvluarefobjecttype.h"
#include "shvluavalueimpl.h"

extern "C"{
#include "../lua/lua.h"
#include "../lua/lauxlib.h"
#include "../lua/lualib.h"
}


//=========================================================================================================
// SHVLuaFuncArgsImpl class
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVLuaFuncArgsImpl::SHVLuaFuncArgsImpl(void *state)
{
	Args = lua_gettop((lua_State*)state);
	State = state;
	ReturnVals = 0;
}

/*************************************
 * ArgCount
 *************************************/
int SHVLuaFuncArgsImpl::ArgCount()
{
	return Args;
}

/*************************************
 * ArgAsInt
 *************************************/
SHVInt SHVLuaFuncArgsImpl::ArgAsInt(int idx)
{
	if (lua_type((lua_State*)State,idx+1) == LUA_TNIL)
		return SHVInt();
	return lua_tointeger((lua_State*)State,idx+1);
}

/*************************************
 * ArgAsString
 *************************************/
SHVStringBuffer SHVLuaFuncArgsImpl::ArgAsString(int idx)
{
	if (lua_type((lua_State*)State,idx+1) == LUA_TNIL)
		return SHVString().ReleaseBuffer();
	return SHVString8C(lua_tostring((lua_State*)State,idx+1)).ToStrT();
}

/*************************************
 * ArgAsDouble
 *************************************/
SHVDouble SHVLuaFuncArgsImpl::ArgAsDouble(int idx)
{
	if (lua_type((lua_State*)State,idx+1) == LUA_TNIL)
		return SHVDouble();
	return lua_tonumber((lua_State*)State,idx+1);
}

/*************************************
 * ArgAsBool
 *************************************/
bool SHVLuaFuncArgsImpl::ArgAsBool(int idx)
{
	if (lua_type((lua_State*)State,idx+1) == LUA_TNIL)
		return false;
	return lua_toboolean((lua_State*)State,idx+1);
}

/*************************************
 * ArgAsRefObject
 *************************************/
SHVRefObject* SHVLuaFuncArgsImpl::ArgAsRef(int idx)
{
	return SHVLuaRefObjectType::ToRef(State,idx+1);
}

/*************************************
 * ArgToValue
 *************************************/
SHVLuaValue* SHVLuaFuncArgsImpl::ArgToValue(int idx)
{
	return ToValue(State,idx);
}

/*************************************
 * PushInt
 *************************************/
void SHVLuaFuncArgsImpl::PushInt(SHVInt val)
{
	ReturnVals++;
	if (val.IsNull())
		lua_pushnil((lua_State*)State);
	else
		lua_pushinteger((lua_State*)State,val);
}

/*************************************
 * PushString
 *************************************/
void SHVLuaFuncArgsImpl::PushString(const SHVStringC val)
{
	ReturnVals++;
	lua_pushstring((lua_State*)State,val.ToStr8().GetSafeBuffer());
}

/*************************************
 * PushDouble
 *************************************/
void SHVLuaFuncArgsImpl::PushDouble(SHVDouble val)
{
	ReturnVals++;
	if (val.IsNull())
		lua_pushnil((lua_State*)State);
	else
		lua_pushnumber((lua_State*)State,val);
}

/*************************************
 * PushBool
 *************************************/
void SHVLuaFuncArgsImpl::PushBool(bool val)
{
	ReturnVals++;
	lua_pushboolean((lua_State*)State,val ? 1 : 0);
}

/*************************************
 * PushRefObject
 *************************************/
void SHVLuaFuncArgsImpl::PushRef(SHVRefObject* val)
{
	ReturnVals++;
	SHVLuaRefObjectType::PushRef(State,val);
}

/*************************************
 * ToValue
 *************************************/
SHVLuaValue* SHVLuaFuncArgsImpl::ToValue(void *state, int idx)
{
	switch (lua_type((lua_State*)state,idx))
	{
	case LUA_TNUMBER:
		return SHVLuaValueImpl::NewDouble(lua_tonumber((lua_State*)state,idx));
	case LUA_TBOOLEAN:
		return SHVLuaValueImpl::NewBool(lua_toboolean((lua_State*)state,idx) ? true : false);
	case LUA_TSTRING:
		return SHVLuaValueImpl::NewString(SHVString8C(lua_tostring((lua_State*)state,idx)).ToStrT());
	case LUA_TUSERDATA:
		if (SHVLuaRefObjectType::IsRef((lua_State*)state,idx))
		{
			return SHVLuaValueImpl::NewRef(SHVLuaRefObjectType::ToRef((lua_State*)state,idx));
		}
		// continue
	default:
		SHVASSERT(false); // Unknown type
	case LUA_TNIL:
		break;
	}
	return SHVLuaValueImpl::NewNull();
}
