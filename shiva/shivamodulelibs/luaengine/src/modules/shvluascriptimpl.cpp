#include "stdafx.h"
#include "shiva/include/platformspc.h"

#include "shvluascriptimpl.h"
#include "shiva/include/threadutils/shvmutexlocker.h"
#include "shiva/include/framework/shveventdata.h"
#include "shvluafuncargsimpl.h"
#include "shiva/include/utils/shvmath.h"
#include "shiva/include/utils/shvstringstream.h"

#include "shvluavalueimpl.h"
#include "shvluarefobjecttype.h"

extern "C"{
#include "../lua/lua.h"
#include "../lua/lauxlib.h"
#include "../lua/lualib.h"
}


//=========================================================================================================
// SHVLuaScriptImpl class
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVLuaScriptImpl::SHVLuaScriptImpl(SHVLuaEngineImpl &engine, SHVEventQueue &queue, int initFuncs)
    : Engine(engine), Queue(queue)
{
	Subscriber = new SHVEventSubscriberFunc<SHVLuaScriptImpl>(this,&SHVLuaScriptImpl::OnInternalEvent,&Queue,true);
	
	HandlingErrors = false;
	Executing = 0;
	
	if (AddToActiveList())
	{
		LuaState = luaL_newstate();
		SHVLuaRefObjectType::RegisterType(LuaState);
		SHVASSERT(lua_gettop((lua_State*)LuaState) == 0);
		if ((initFuncs&FuncsBase))		luaopen_base((lua_State*)LuaState);
		if ((initFuncs&FuncsCoRoutine))	luaopen_coroutine((lua_State*)LuaState);
		if ((initFuncs&FuncsTable))		luaopen_table((lua_State*)LuaState);
		if ((initFuncs&FuncsIo))		luaopen_io((lua_State*)LuaState);
		if ((initFuncs&FuncsOs))		luaopen_os((lua_State*)LuaState);
		if ((initFuncs&FuncsString))	luaopen_string((lua_State*)LuaState);
		if ((initFuncs&FuncsBit32))		luaopen_bit32((lua_State*)LuaState);
		if ((initFuncs&FuncsMath))		luaopen_math((lua_State*)LuaState);
		if ((initFuncs&FuncsDebug))		luaopen_debug((lua_State*)LuaState);
		if ((initFuncs&FuncsPackage))	luaopen_package((lua_State*)LuaState);
		if ((initFuncs&FuncsShiva))		LoadShivaFunctions();
		Started = true;
	}
	else
	{
		SHVASSERT(false); // Always create after register and before preunregister
		Started = false;
		LuaState = NULL;
	}
}

/*************************************
 * Destructor
 *************************************/
SHVLuaScriptImpl::~SHVLuaScriptImpl()
{
	RemoveFromActiveList();
	SHVASSERT(LuaState == NULL);
	if (LuaState)
		lua_close((lua_State*)LuaState);
	Funcs.Clear();
	Classes.Clear();
}

/*************************************
 * GetEventQueue
 *************************************/
SHVEventQueue& SHVLuaScriptImpl::GetEventQueue()
{
	return Queue;
}

/*************************************
 * GetModuleList
 *************************************/
SHVModuleList& SHVLuaScriptImpl::GetModuleList()
{
	return Engine.GetModuleList();
}

/*************************************
 * IsActive
 *************************************/
bool SHVLuaScriptImpl::IsActive() const
{
	return Started;
}

/*************************************
 * IsHandlingErrors
 *************************************/
bool SHVLuaScriptImpl::IsHandlingErrors() const
{
	return HandlingErrors;
}

/*************************************
 * NewArgs
 *************************************/
SHVLuaValues* SHVLuaScriptImpl::NewArgs() const
{
	return new SHVLuaValuesImpl();
}

/*************************************
 * SetHandlingErrors
 *************************************/
void SHVLuaScriptImpl::SetHandlingErrors(bool handle, SHVEventSubscriberBase* errSubscriber)
{
SHVMutexLocker l(Engine.Lock);
	HandlingErrors = handle;
	if (HandlingErrors)
	{
		ErrorSubscriber = errSubscriber;
	}
	else
	{
		if (errSubscriber)
			errSubscriber->ValidateRefCount();
		ErrorSubscriber = NULL;
	}
}

/*************************************
 * ClearErrors
 *************************************/
void SHVLuaScriptImpl::ClearErrors()
{
SHVMutexLocker l(Engine.Lock);
	Errors.RemoveAll();
}

/*************************************
 * HasErrors
 *************************************/
bool SHVLuaScriptImpl::HasErrors()
{
	return Errors.GetCount() ? true : false;
}

/*************************************
 * GetAndClearErrors
 *************************************/
SHVStringBuffer SHVLuaScriptImpl::GetAndClearErrors()
{
SHVMutexLocker (Engine.Lock);
SHVStringStream retVal;

	while (Errors.GetCount())
	{
		retVal.WriteString(Errors.GetFirst());
		Errors.RemoveHead();
	}
	retVal.Finalize();
	
	return SHVString(retVal).ReleaseBuffer();
}

/*************************************
 * Load
 *************************************/
SHVBool SHVLuaScriptImpl::Execute(const SHVStringC script)
{
	if (IsActive())
	{
		if (SHVThreadBase::GetCurrentThreadID() == Queue.GetThreadID() && LuaState)
		{
			Executing++;
			if (luaL_loadstring((lua_State*)LuaState,script.ToStr8().GetBufferConst()) != LUA_OK)
			{
				SHVASSERT(false);
				Executing--;
				return HandleError(ErrParsing, SHVString8C(lua_tostring((lua_State*)LuaState, -1)).ToStrT());
			}
			else
			{
				if (lua_pcall((lua_State*)LuaState, 0, 0, 0) != LUA_OK)
				{
					SHVASSERT(false);
					Executing--;
					return HandleError(ErrExecuting, SHVString8C(lua_tostring((lua_State*)LuaState, -1)).ToStrT());
				}
				lua_settop((lua_State*)LuaState, 0); // Resets the stack
			}
			Executing--;
		}
		else
		{
			Subscriber->EmitNow(Engine.GetModuleList(),new SHVEventData<SHVString>(script,NULL,EventInternalExecute));
			return ErrAsync;
		}
	}
	else
	{
		SHVASSERT(false);
		return HandleError(ErrGeneric, SHVString(_S("Script stopped")).ReleaseBuffer(),false);
	}
	return ErrNone;
}

/*************************************
 * LoadFromFile
 *************************************/
SHVBool SHVLuaScriptImpl::ExecuteFromFile(const SHVStringC fileName)
{
	if (IsActive())
	{
		if (SHVThreadBase::GetCurrentThreadID() == Queue.GetThreadID() && LuaState)
		{
			Executing++;
			if (luaL_loadfile((lua_State*)LuaState,fileName.ToStr8().GetBufferConst()) != LUA_OK)
			{
				SHVASSERT(false);
				Executing--;
				return HandleError(ErrParsing, SHVString8C(lua_tostring((lua_State*)LuaState, -1)).ToStrT());
			}
			else
			{
				if (lua_pcall((lua_State*)LuaState, 0, 0, 0) != LUA_OK)
				{
					SHVASSERT(false);
					Executing--;
					return HandleError(ErrExecuting, SHVString8C(lua_tostring((lua_State*)LuaState, -1)).ToStrT());
				}
				lua_settop((lua_State*)LuaState, 0); // Resets the stack
			}
			Executing--;
		}
		else
		{
			Subscriber->EmitNow(Engine.GetModuleList(),new SHVEventData<SHVString>(fileName,NULL,EventInternalExecuteFromFile));
			return ErrAsync;
		}
	}
	else
	{
		SHVASSERT(false);
		return HandleError(ErrGeneric, SHVString(_S("Script stopped")).ReleaseBuffer(),false);
	}
	return ErrNone;
}

/*************************************
 * ExecuteFunction
 *************************************/
SHVLuaValuesRef SHVLuaScriptImpl::ExecuteFunction(const char* name, SHVLuaValues* args)
{
SHVLuaValuesImplRef retVal = new SHVLuaValuesImpl();
SHVLuaValuesRef a(args);

	if (IsActive())
	{
		if (SHVThreadBase::GetCurrentThreadID() == Queue.GetThreadID() && LuaState)
		{
		int stackCount = lua_gettop((lua_State*)LuaState);
			SHVASSERT(Executing || stackCount == 0); // Assume an empty stack
			Executing++;
			lua_getglobal((lua_State*)LuaState, name);
			if (lua_type((lua_State*)LuaState,stackCount+1) != LUA_TFUNCTION)
			{
				SHVASSERT(false);
				HandleError(ErrExecuting, SHVString(_S("Unknown function")).ReleaseBuffer());
				Executing--;
				return retVal.ReleaseReference();
			}
			if (args)
			{
			SHVLuaValue* val;
				for (int i=0; i<a->GetCount(); i++)
				{
					val = a->Get(i);
					switch (val->GetType())
					{
					case SHVLuaValue::TypeNull:
						lua_pushnil((lua_State*)LuaState);
						break;
					case SHVLuaValue::TypeInt:
						lua_pushinteger((lua_State*)LuaState,val->AsInt());
						break;
					case SHVLuaValue::TypeDouble:
						lua_pushnumber((lua_State*)LuaState,val->AsDouble());
						break;
					case SHVLuaValue::TypeString:
						lua_pushstring((lua_State*)LuaState,val->AsString().ToStr8().GetSafeBuffer());
						break;
					case SHVLuaValue::TypeBool:
						lua_pushboolean((lua_State*)LuaState,val->AsBool() ? 1 : 0);
						break;
					case SHVLuaValue::TypeRefOject:
						{
						const char* typeID = val->GetRefType();
							SHVLuaRefObjectType::PushRef(LuaState,val->AsRef(typeID),typeID);
						}
						break;
					}
				}
			}
			if (lua_pcall((lua_State*)LuaState, (args ? args->GetCount() : 0), LUA_MULTRET, 0) != LUA_OK)
			{
				SHVASSERT(false);
				HandleError(ErrExecuting, SHVString8C(lua_tostring((lua_State*)LuaState, -1)).ToStrT());
				Executing--;
				return retVal.ReleaseReference();
			}
			for (int i = stackCount; i < lua_gettop((lua_State*)LuaState); i++)
			{
				retVal->AddValue(SHVLuaFuncArgsImpl::ToValue(LuaState,i));
			}
			lua_settop((lua_State*)LuaState, stackCount); // Resets the stack
			Executing--;
		}
		else
		{
			Subscriber->EmitNow(Engine.GetModuleList(),new SHVEventData<ExecFuncData>(ExecFuncData(name,args),NULL,EventInternalExecuteFunction));
		}
	}
	else
	{
		SHVASSERT(false);
		HandleError(ErrGeneric, SHVString(_S("Script stopped")).ReleaseBuffer(),false);
	}
	return retVal.ReleaseReference();
}

/*************************************
 * RegisterFunc
 *************************************/
void SHVLuaScriptImpl::RegisterFunc(const char* name, SHVLuaFuncBase* func)
{
	if (SHVThreadBase::GetCurrentThreadID() == Queue.GetThreadID() || !Engine.GetModuleList().IsRegistered())
	{
		if (LuaState)
		{
			lua_pushlightuserdata((lua_State*)LuaState,this);
			lua_pushinteger((lua_State*)LuaState,(int)Funcs.Add(func));
			lua_pushcclosure((lua_State*)LuaState,(lua_CFunction)&LuaFunc,2);
			lua_setglobal((lua_State*)LuaState,name);
		}
		else
		{
			delete func;
		}
	}
	else
	{
		Subscriber->EmitNow(Engine.GetModuleList(),new SHVEventData<RegisterFuncData>(RegisterFuncData(name,func),NULL,EventInternalRegisterFunc));
	}
}

/*************************************
 * RegisterClass
 *************************************/
void SHVLuaScriptImpl::RegisterClass(const char* name, SHVLuaMetaClassBase* meta)
{
	if (SHVThreadBase::GetCurrentThreadID() == Queue.GetThreadID() || !Engine.GetModuleList().IsRegistered())
	{
		if (LuaState)
		{
		SHVLuaClassImpl* luaClass = new SHVLuaClassImpl(meta);
		
			if (luaClass->Initialize(name,this))
				Classes.Add(luaClass);
			else
				delete luaClass;
		}
		else
		{
			delete meta;
		}
	}
	else
	{
		Subscriber->EmitNow(Engine.GetModuleList(),new SHVEventData<RegisterClassData>(RegisterClassData(name,meta),NULL,EventInternalRegisterClass));
	}
}

/*************************************
 * GarbageCollect
 *************************************/
void SHVLuaScriptImpl::GarbageCollect()
{
	if (SHVThreadBase::GetCurrentThreadID() == Queue.GetThreadID() || !Engine.GetModuleList().IsRegistered())
	{
		if (LuaState)
		{
			lua_gc((lua_State*)LuaState,LUA_GCCOLLECT,0);
		}
	}
	else
	{
		Subscriber->EmitNow(Engine.GetModuleList(),new SHVEvent(NULL,EventInternalGarbageCollect));
	}
}

/*************************************
 * StopScript
 *************************************/
void SHVLuaScriptImpl::StopScript()
{
	if (SHVThreadBase::GetCurrentThreadID() == Queue.GetThreadID() || !Engine.GetModuleList().IsRegistered())
	{
		Started = false;
		RemoveFromActiveList();
		if (LuaState)
		{
			lua_close((lua_State*)LuaState);
			LuaState = NULL;
			Funcs.Clear();
			Classes.Clear();
		}
	}
	else if (LuaState)
	{
		Started = false;
		Subscriber->EmitNow(Engine.GetModuleList(),new SHVEvent(NULL,EventInternalStopScript));
	}
}

/*************************************
 * AddToActiveList
 *************************************/
SHVBool SHVLuaScriptImpl::AddToActiveList()
{
SHVBool retVal(SHVBool::True);

	// Make sure the event system is active when we add the script
	if (Engine.GetModuleList().LockEvent())
	{
	SHVMutexLocker l(Engine.Lock);
	SHVListPos pos = Engine.ActiveScripts.Find(this);
	
		if (pos)
		{
			retVal = SHVBool::False;
		}
		else
		{
			Started = true;
			Engine.ActiveScripts.AddTail(this);
		}
		
		Engine.GetModuleList().UnlockEvent();
	}
	else if (!Engine.IsRegistered())
	{
		retVal = SHVBool::False;
	}
	
	return retVal;
}

/*************************************
 * RemoveFromActiveList
 *************************************/
void SHVLuaScriptImpl::RemoveFromActiveList()
{
SHVMutexLocker l(Engine.Lock);
SHVListPos pos = Engine.ActiveScripts.Find(this);
	if (pos)
		Engine.ActiveScripts.RemoveAt(pos);
}

/*************************************
 * HandleError
 *************************************/
SHVBool SHVLuaScriptImpl::HandleError(SHVBool error, SHVStringBuffer errString, bool pop)
{
	if (HandlingErrors)
	{
		Engine.Lock.Lock();
		Errors.AddTail(errString);
		Engine.Lock.Unlock();
		
		if (!ErrorSubscriber.IsNull())
			ErrorSubscriber->EmitNow(Engine.GetModuleList(),new SHVEvent(&Engine,SHVLuaEngine::EventScriptError,error.GetError()));
	}
	if (pop)
		lua_settop((lua_State*)LuaState, 0); // Resets the stack
	
	return error;
}

/*************************************
 * LoadShivaFunctions
 *************************************/
void SHVLuaScriptImpl::LoadShivaFunctions()
{
	RegisterFunc("shiva_eval",new SHVLuaFunc<SHVLuaScriptImpl>(this,&SHVLuaScriptImpl::LuaEval));
}

/*************************************
 * LuaEval
 *************************************/
void SHVLuaScriptImpl::LuaEval(SHVLuaScript* script, SHVLuaFuncArgs& args)
{
	SHVUNUSED_PARAM(script);
	for(int i=0; i<args.ArgCount(); i++)
	{
		args.PushDouble(SHVMath::Eval(args.ArgAsString(i)));
	}
}

/*************************************
 * OnInternalEvent
 *************************************/
void SHVLuaScriptImpl::OnInternalEvent(SHVEvent *event)
{
	switch (event->GetID())
	{
	case EventInternalExecute:
		Execute(SHVEventData<SHVString>::Get(event).GetBufferConst());
		break;
	case EventInternalExecuteFromFile:
		ExecuteFromFile(SHVEventData<SHVString>::Get(event).GetBufferConst());
		break;
	case EventInternalExecuteFunction:
		ExecuteFunction(SHVEventData<ExecFuncData>::Get(event).name,SHVEventData<ExecFuncData>::Get(event).args);
		break;
	case EventInternalRegisterFunc:
		RegisterFunc(SHVEventData<RegisterFuncData>::Get(event).name,SHVEventData<RegisterFuncData>::Get(event).func);
		break;
	case EventInternalRegisterClass:
		RegisterClass(SHVEventData<RegisterClassData>::Get(event).name,SHVEventData<RegisterClassData>::Get(event).meta);
		break;
	case EventInternalGarbageCollect:
		GarbageCollect();
		break;
	case EventInternalStopScript:
		StopScript();
		break;
	}
}

/*************************************
 * LuaFunc
 *************************************/
int SHVLuaScriptImpl::LuaFunc(void *state)
{
SHVLuaScriptImpl* self = (SHVLuaScriptImpl*)lua_touserdata((lua_State*)state,lua_upvalueindex(1));
int funcIdx = (int)lua_tointeger((lua_State*)state,lua_upvalueindex(2));
SHVLuaFuncArgsImpl args(state);
	
	self->Funcs[funcIdx]->Callback(self,args);
	
	return args.ReturnVals;
}
