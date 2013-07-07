#include "stdafx.h"
#include "shiva/include/platformspc.h"
#include "shiva/include/utils/shvstringc.h"

#include "shvluarefobjecttype.h"

extern "C"{
#include "../lua/lua.h"
#include "../lua/lauxlib.h"
#include "../lua/lualib.h"
}

//=========================================================================================================
// SHVLuaRefObjectType
//=========================================================================================================

/*************************************
 * RegisterType
 *************************************/
void SHVLuaRefObjectType::RegisterType(void* state)
{
static const luaL_Reg shvref_methods[] = {
		{"unref", (lua_CFunction)&SHVLuaRefObjectType::LuaGc},
		{"__gc", (lua_CFunction)&SHVLuaRefObjectType::LuaGc},
		{NULL, NULL}
	};

	// Create the meta table and push it on the stack
	luaL_newmetatable((lua_State*)state, SHVLUA_UDATANAME);
	lua_pushvalue((lua_State*)state, -1);

	// metatable.__index = metatable
	lua_setfield((lua_State*)state, -2, "__index");

	// Add the methods to the meta table and pop it from the stack
	luaL_setfuncs((lua_State*)state, shvref_methods, 0);
	lua_pop((lua_State*)state, 1);
}

/*************************************
 * IsRefObject
 *************************************/
bool SHVLuaRefObjectType::IsRef(void* state, int idx, const char** typeID)
{
LuaRefObject* data = (LuaRefObject*)luaL_testudata((lua_State*)state, idx, SHVLUA_UDATANAME);
	if (data)
	{
		if (typeID)
			*typeID = data->Type;
		return true;
	}
	else if (typeID)
	{
		*typeID = NULL;
	}
	return false;
}
bool SHVLuaRefObjectType::IsRefByType(void* state, int idx, const char* typeID)
{
LuaRefObject* data = (LuaRefObject*)luaL_testudata((lua_State*)state, idx, SHVLUA_UDATANAME);
	return data && SHVString8C(data->Type) == SHVString8C(typeID);
}

/*************************************
 * ToRefObject
 *************************************/
SHVRefObject* SHVLuaRefObjectType::ToRef(void* state, int idx, const char** typeID)
{
LuaRefObject* data = (LuaRefObject*)luaL_testudata((lua_State*)state, idx, SHVLUA_UDATANAME);
	if (data)
	{
		if (typeID)
			*typeID = data->Type;
		return data->Obj;
	}
	else if (typeID)
	{
		*typeID = NULL;
	}
	return NULL;
}
SHVRefObject* SHVLuaRefObjectType::ToRefByType(void* state, int idx, const char* typeID)
{
LuaRefObject* data = (LuaRefObject*)luaL_testudata((lua_State*)state, idx, SHVLUA_UDATANAME);
	return (data && SHVString8C(data->Type) == SHVString8C(typeID) ? data->Obj : NULL);
}

/*************************************
 * PushRefObject
 *************************************/
void SHVLuaRefObjectType::PushRef(void* state, SHVRefObject* val, const char* typeID)
{
LuaRefObject* data = (LuaRefObject*)lua_newuserdata((lua_State*)state, sizeof(LuaRefObject));

	data->Obj = NULL;
	data->Type = typeID;

	// Set the meta table
	luaL_getmetatable((lua_State*)state, SHVLUA_UDATANAME);
	lua_setmetatable((lua_State*)state, -2);

	if (val)
		data->Obj = val->CreateRef();
}

/*************************************
 * LuaGc
 *************************************/
int SHVLuaRefObjectType::LuaGc(void* state)
{
LuaRefObject* data = (LuaRefObject*)luaL_testudata((lua_State*)state, 1, SHVLUA_UDATANAME);

	if (data && data->Obj)
	{
		data->Obj->DestroyRef();
		data->Obj = NULL;
	}
	return 0;
}
