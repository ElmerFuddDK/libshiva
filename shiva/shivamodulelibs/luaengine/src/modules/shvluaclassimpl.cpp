#include "stdafx.h"
#include "shiva/include/platformspc.h"

#include "shvluascriptimpl.h"
#include "shvluafuncargsimpl.h"

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
SHVLuaClassImpl::SHVLuaClassImpl(SHVLuaMetaClassBase* meta) : Meta(meta)
{
}

/*************************************
 * Destructor
 *************************************/
SHVLuaClassImpl::~SHVLuaClassImpl()
{
}

/*************************************
 * Initialize
 *************************************/
SHVBool SHVLuaClassImpl::Initialize(const char* name, SHVLuaScriptImpl* script)
{
	if (SHVThreadBase::GetCurrentThreadID() == script->Queue.GetThreadID() && script->LuaState && name && MetaName.IsEmpty())
	{
	lua_State* state = (lua_State*)script->LuaState;
	
		Script = script;
		MetaName.Format("luaL_%s", name);
		
		luaL_newmetatable(state, MetaName.GetBufferConst());

		lua_pushlightuserdata(state,this);
		lua_pushcclosure(state,(lua_CFunction)&SHVLuaClassImpl::LuaConstructor,1);
		lua_setfield(state, -2, "new");
		
		lua_pushlightuserdata(state,this);
		lua_pushcclosure(state,(lua_CFunction)&SHVLuaClassImpl::LuaDestructor,1);
		lua_setfield(state, -2, "__gc");
		
		Meta->RegisterMethods(*this);
		
		lua_pushvalue(state, -1);
		lua_setfield(state, -1, "__index");
		lua_pushvalue(state, -1);
		lua_setfield(state, -1, "__metatable");
		
		lua_setglobal(state, name);

		return SHVBool::True;
	}
	return SHVBool::False;
}

/*************************************
 * Add
 *************************************/
void SHVLuaClassImpl::Add(const char* name, SHVLuaMethodBase* method)
{
	if (Script)
	{
	lua_State* state = (lua_State*)Script->LuaState;
		lua_pushlightuserdata(state,this);
		lua_pushinteger(state,(int)Methods.Add(method));
		lua_pushcclosure(state,(lua_CFunction)&LuaMethod,2);
		lua_setfield(state, -2, name);
	}
	else
	{
		delete method;
	}
}

/*************************************
 * LuaConstructor
 *************************************/
int SHVLuaClassImpl::LuaConstructor(void* state)
{
SHVLuaClassImpl* self = (SHVLuaClassImpl*)lua_touserdata((lua_State*)state,lua_upvalueindex(1));
SHVLuaFuncArgsImpl args(state);
SHVLuaClassBase** objectPtr;
	
	objectPtr = (SHVLuaClassBase**)lua_newuserdata((lua_State*)state, sizeof(SHVLuaClassBase*));
	*objectPtr = self->Meta->New(self->Script,args);
	SHVASSERT(args.ReturnVals == 0); // Not allowed to return anything on a constructor
	
	luaL_getmetatable((lua_State*)state, self->MetaName.GetBufferConst());
	lua_setmetatable((lua_State*)state, -2);
	
	return 1;
}

/*************************************
 * LuaDestructor
 *************************************/
int SHVLuaClassImpl::LuaDestructor(void* state)
{
SHVLuaClassImpl* self = (SHVLuaClassImpl*)lua_touserdata((lua_State*)state,lua_upvalueindex(1));
SHVLuaClassBase* object = *(SHVLuaClassBase**)luaL_checkudata((lua_State*)state, 1, self->MetaName.GetBufferConst());
	
	delete object;
	return 0;
}

/*************************************
 * LuaMethod
 *************************************/
int SHVLuaClassImpl::LuaMethod(void* state)
{
SHVLuaClassImpl* self = (SHVLuaClassImpl*)lua_touserdata((lua_State*)state,lua_upvalueindex(1));
int methodIdx = (int)lua_tointeger((lua_State*)state,lua_upvalueindex(2));
SHVLuaClassBase* object = *(SHVLuaClassBase**)luaL_checkudata((lua_State*)state, 1, self->MetaName.GetBufferConst());
SHVLuaFuncArgsImpl args(state,2);
	
	self->Methods[methodIdx]->Callback(object,self->Meta,self->Script,args);
	
	return args.ReturnVals;
}
