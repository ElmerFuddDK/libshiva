#ifndef __SHIVA_LUAENGINEIMPL_LUASCRIPT_H
#define __SHIVA_LUAENGINEIMPL_LUASCRIPT_H

#include "shvluaengineimpl.h"
#include "shiva/include/utils/shvvector.h"


//-=========================================================================================================
/// SHVLuaScriptImpl class
//-=========================================================================================================
/**
 * Implements the lua script
 \todo Add a ExecuteFromStream function that takes a SHVStreamIn*
 \todo Add an event that makes it possible to register other shiva functions on initialize eg. from dataengine
 \todo Add a way to register classes (<class>.<function>)
 \todo Make it compile on windows ce
 */

class SHVLuaScriptImpl : public SHVLuaScript
{
public:
	
	SHVLuaScriptImpl(SHVLuaEngineImpl& engine, SHVEventQueue& queue, int initFuncs);
	virtual ~SHVLuaScriptImpl();

	// From SHVLuaScript
	virtual SHVEventQueue& GetEventQueue();
	virtual bool IsActive() const;
	virtual bool IsHandlingErrors() const;
	
	virtual SHVLuaValues* NewArgs() const;
	
	virtual void SetHandlingErrors(bool handle = true, SHVEventSubscriberBase* errSubscriber = NULL);
	virtual void ClearErrors();
	virtual bool HasErrors();
	virtual SHVStringBuffer GetAndClearErrors();
	
	virtual SHVBool Execute(const SHVStringC script);
	virtual SHVBool ExecuteFromFile(const SHVStringC fileName);
	virtual SHVLuaValuesRef ExecuteFunction(const char* name, SHVLuaValues* args = NULL);

	virtual void RegisterFunc(const char* name, SHVLuaFuncBase* func);

	virtual void StopScript();


private:
	///\cond INTERNAL
	enum InternalEvents {
		EventInternalExecute,
		EventInternalExecuteFromFile,
		EventInternalExecuteFunction,
		EventInternalStopScript,
		EventInternalRegisterFunc
	};
	
	SHVBool AddToActiveList();
	void RemoveFromActiveList();
	
	SHVBool HandleError(SHVBool error, SHVStringBuffer errString, bool pop = true);
	
	void LoadShivaFunctions();
	void LuaEval(SHVLuaScript* script, SHVLuaFuncArgs& args);
	
	void OnInternalEvent(SHVEvent* event);
	static int LuaFunc(void* state);

	struct RegisterFuncData {
	
		const char* name;
		SHVLuaFuncBase* func;
		
		inline RegisterFuncData(const char* n, SHVLuaFuncBase* f) : name(n), func(f) {}
	};

	struct ExecFuncData {
	
		const char* name;
		SHVLuaValuesRef args;
		
		inline ExecFuncData(const char* n, SHVLuaValues* a) : name(n), args(a) {}
	};

	SHVLuaEngineImpl& Engine;
	SHVEventQueue& Queue;
	SHVEventSubscriberBaseRef Subscriber, ErrorSubscriber;
	
	bool Started, HandlingErrors;
	void* LuaState;
	void* LuaErrorFunc;
	SHVVector<SHVLuaFuncBase> Funcs;
	SHVList<SHVString,SHVStringBuffer> Errors;
	///\endcond
};

#endif
