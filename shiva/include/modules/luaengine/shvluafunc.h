#ifndef __SHIVA_LUAENGINE_LUAFUNC_H
#define __SHIVA_LUAENGINE_LUAFUNC_H

#include "../../framework/shveventsubscriber.h"
#include "shvluavalue.h"
class SHVLuaScript;


//-=========================================================================================================
/// SHVLuaFuncArgs class
//-=========================================================================================================
/**
 * Interface for accessing arguments and pushing return values in functions
 */

class SHVLuaFuncArgs
{
public:


	// Arguments
	virtual int ArgCount() = 0;

	virtual SHVInt ArgAsInt(int idx) = 0;
	virtual SHVStringBuffer ArgAsString(int idx) = 0;
	virtual SHVDouble ArgAsDouble(int idx) = 0;
	virtual SHVRefObject* ArgRef(int idx) = 0;
	virtual SHVLuaValue* ArgToValue(int idx) = 0;


	// Return values
	virtual void PushInt(SHVInt val) = 0;
	virtual void PushString(const SHVStringC val) = 0;
	virtual void PushDouble(SHVDouble val) = 0;
	virtual void PushRef(SHVRefObject* val) = 0;

};


//-=========================================================================================================
/// SHVLuaFuncBase class
//-=========================================================================================================
/**
 * Base class for defining functions into a lua script
 */

class SHVLuaFuncBase
{
public:

	virtual ~SHVLuaFuncBase() {}


	virtual void Callback(SHVLuaScript* script, SHVLuaFuncArgs& args) = 0;

};


//-=========================================================================================================
/// SHVLuaFunc class
//-=========================================================================================================
/**
 * Implementation of a lua script function
 */

template <class T>
class SHVLuaFunc : public SHVLuaFuncBase
{
public:

	// defines
	typedef void (T::*OnCallback)(SHVLuaScript* script, SHVLuaFuncArgs& args);


	// constructor
	SHVLuaFunc(T* target, OnCallback func);


	virtual void Callback(SHVLuaScript* script, SHVLuaFuncArgs& args);


protected:
	T* Target;
	OnCallback Func;
};



// ============================================= implementation ============================================= //

/*************************************
 * Constructor
 *************************************/
template<class T>
SHVLuaFunc<T>::SHVLuaFunc(T* target, OnCallback func) : Target(target), Func(func)
{
}

/*************************************
 * Callback
 *************************************/
template<class T>
void SHVLuaFunc<T>::Callback(SHVLuaScript* script, SHVLuaFuncArgs& args)
{
	(Target->*Func)(script, args);
}

#endif
