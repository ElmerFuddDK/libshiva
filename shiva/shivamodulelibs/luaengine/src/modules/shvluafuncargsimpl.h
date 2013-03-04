#ifndef __SHIVA_LUAENGINEIMPL_LUAFUNCARGS_H
#define __SHIVA_LUAENGINEIMPL_LUAFUNCARGS_H

#include "shiva/include/modules/luaengine/shvluafunc.h"
class SHVLuaScriptImpl;


//-=========================================================================================================
/// SHVLuaFuncArgsImpl class
//-=========================================================================================================
/**
 * Implements the arguments for a shiva lua func call
 */

class SHVLuaFuncArgsImpl : public SHVLuaFuncArgs
{
public:

	SHVLuaFuncArgsImpl(void* state);

	// Arguments
	virtual int ArgCount();

	virtual SHVInt ArgAsInt(int idx);
	virtual SHVStringBuffer ArgAsString(int idx);
	virtual SHVDouble ArgAsDouble(int idx);
	virtual bool ArgAsBool(int idx);
	virtual SHVRefObject* ArgAsRef(int idx);
	virtual SHVLuaValue* ArgToValue(int idx);


	// Return values
	virtual void PushInt(SHVInt val);
	virtual void PushString(const SHVStringC val);
	virtual void PushDouble(SHVDouble val);
	virtual void PushBool(bool val);
	virtual void PushRef(SHVRefObject* val);


	static SHVLuaValue* ToValue(void* state, int idx);

protected:
friend class SHVLuaScriptImpl;

	void* State;
	int Args, ReturnVals;

};

#endif // SHVLUAFUNCARGSIMPL_H
