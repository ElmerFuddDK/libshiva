#ifndef __SHIVA_LUAENGINE_LUASCRIPT_H
#define __SHIVA_LUAENGINE_LUASCRIPT_H

#include "../../utils/shvrefobject.h"
#include "../../framework/shvevent.h"
#include "shvluafunc.h"
#include "shvluaclassbase.h"
#include "shvluavalue.h"


//-=========================================================================================================
/// SHVLuaScript class
//-=========================================================================================================
/**
 * Encapsulates a lua script
 */

class SHVLuaScript : public SHVRefObject
{
public:

	enum Funcs {
		FuncsBase = 0x0001,
		FuncsCoRoutine = 0x0002,
		FuncsTable = 0x0004,
		FuncsIo = 0x0008,
		FuncsOs = 0x0010,
		FuncsString = 0x0020,
		FuncsBit32 = 0x0040,
		FuncsMath = 0x0080,
		FuncsDebug = 0x0100,
		FuncsPackage = 0x0200,
		FuncsShiva = 0x0400,
		FuncsDefault = FuncsBase|FuncsCoRoutine|FuncsTable|FuncsIo|FuncsOs|FuncsString|FuncsBit32|FuncsMath|FuncsPackage|FuncsShiva,
		FuncsAll = 0xFFFF
	};
	
	enum Errors {
		ErrNone = SHVBool::True,
		ErrGeneric = SHVBool::False,
		ErrAsync = 1, // call will be performed asyncronous
		ErrParsing = 2,
		ErrExecuting = 3
	};

	// properties
	virtual SHVEventQueue& GetEventQueue() = 0;
	virtual SHVModuleList& GetModuleList() = 0;
	virtual bool IsActive() const = 0;
	virtual bool IsHandlingErrors() const = 0;
	
	virtual SHVLuaValues* NewArgs() const = 0;
	
	virtual void SetHandlingErrors(bool handle = true, SHVEventSubscriberBase* errSubscriber = NULL) = 0;
	virtual void ClearErrors() = 0;
	virtual bool HasErrors() = 0;
	virtual SHVStringBuffer GetAndClearErrors() = 0;
	
	virtual SHVBool Execute(const SHVStringC script) = 0;
	virtual SHVBool ExecuteFromFile(const SHVStringC fileName) = 0;
	virtual SHVLuaValuesRef ExecuteFunction(const char* name, SHVLuaValues* args = NULL) = 0;

	virtual void RegisterFunc(const char* name, SHVLuaFuncBase* func) = 0;
	virtual void RegisterClass(const char* name, SHVLuaMetaClassBase* constructor) = 0;

	virtual void GarbageCollect() = 0;

	virtual void StopScript() = 0;

};
typedef SHVRefObjectContainer<SHVLuaScript> SHVLuaScriptRef;

#endif
