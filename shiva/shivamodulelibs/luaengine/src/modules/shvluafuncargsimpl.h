#ifndef __SHIVA_LUAENGINEIMPL_LUAFUNCARGS_H
#define __SHIVA_LUAENGINEIMPL_LUAFUNCARGS_H

#include "shiva/include/modules/luaengine/shvluafunc.h"
class SHVLuaScriptImpl;
class SHVLuaClassImpl;


//-=========================================================================================================
/// SHVLuaFuncArgsImpl class
//-=========================================================================================================
/**
 * Implements the arguments for a shiva lua func call
 */

class SHVLuaFuncArgsImpl : public SHVLuaFuncArgs
{
public:

	SHVLuaFuncArgsImpl(void* state, int argOffset = 1);

	// Arguments
	virtual int ArgCount();

	virtual SHVInt ArgAsInt(int idx);
	virtual SHVStringBuffer ArgAsString(int idx);
	virtual SHVDouble ArgAsDouble(int idx);
	virtual bool ArgAsBool(int idx);
	virtual SHVRefObject* ArgAsRef(int idx, const char* typeID);
	virtual SHVLuaValue* ArgToValue(int idx);


	// Return values
	virtual void PushInt(SHVInt val);
	virtual void PushString(const SHVStringC val);
	virtual void PushDouble(SHVDouble val);
	virtual void PushBool(bool val);
	virtual void PushRef(SHVRefObject* val, const char* typeID);
	virtual void PushRef(SHVLuaValues::RefStruct refObj);


	static SHVLuaValue* ToValue(void* state, int idx, int argOffset = 1);

protected:
friend class SHVLuaScriptImpl;
friend class SHVLuaClassImpl;

	void* State;
	int Args, ArgOffset, ReturnVals;

};

#endif // SHVLUAFUNCARGSIMPL_H
