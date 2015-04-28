#ifndef __SHIVA_LUAENGINE_LUACLASSBASE_H
#define __SHIVA_LUAENGINE_LUACLASSBASE_H

#include "shvluafunc.h"
class SHVLuaClassMethods;
class SHVLuaMetaClassBase;


//-=========================================================================================================
/// SHVLuaClassBase class
//-=========================================================================================================
/**
 * Generic interface for implementing a Lua class in SHIVA
 \see SHVLuaClass
 */
class SHVLuaClassBase
{
public:

	// You will need to implement this as a static "RegisterMethods" on your class
	typedef void (*OnRegisterMethods)(SHVLuaClassMethods& methods);

	virtual ~SHVLuaClassBase() {}

protected:
	inline SHVLuaClassBase(SHVLuaMetaClassBase* meta, SHVLuaScript* script, SHVLuaFuncArgs& args)
	{ SHVUNUSED_PARAM(meta); SHVUNUSED_PARAM(script); SHVUNUSED_PARAM(args); }
};


//-=========================================================================================================
/// SHVLuaMetaClassBase class
//-=========================================================================================================
/**
 * Base class for creating handling shiva classes in lua
 */

class SHVLuaMetaClassBase
{
public:

	virtual ~SHVLuaMetaClassBase() {}

	virtual void RegisterMethods(SHVLuaClassMethods& methods) = 0;
	virtual SHVLuaClassBase* New(SHVLuaScript* script, SHVLuaFuncArgs& args) = 0;

};
typedef SHVPtrContainer<SHVLuaMetaClassBase> SHVLuaMetaClassBasePtr;


//-=========================================================================================================
/// SHVLuaMethodBase class
//-=========================================================================================================
/**
 * Base class for defining method functions for a class
 */

class SHVLuaMethodBase
{
public:

	virtual ~SHVLuaMethodBase() {}


	virtual void Callback(SHVLuaClassBase* self, SHVLuaMetaClassBase* meta, SHVLuaScript* script, SHVLuaFuncArgs& args) = 0;

};
typedef SHVPtrContainer<SHVLuaMethodBase> SHVLuaMethodBasePtr;


//-=========================================================================================================
/// SHVLuaMethod class
//-=========================================================================================================
/**
 * Implementation of a lua method function
 */

template <class T>
class SHVLuaMethod : public SHVLuaMethodBase
{
public:

	// defines
	typedef void (T::*OnCallback)(SHVLuaMetaClassBase* meta, SHVLuaScript* script, SHVLuaFuncArgs& args);


	// constructor
	SHVLuaMethod(OnCallback func);


	virtual void Callback(SHVLuaClassBase* self, SHVLuaMetaClassBase* meta, SHVLuaScript* script, SHVLuaFuncArgs& args);


protected:
	OnCallback Func;
};


//-=========================================================================================================
/// SHVLuaClassMethods class
//-=========================================================================================================
/**
 * Interface for defining methods for a class
 */

class SHVLuaClassMethods
{
public:


	// Add a method during register
	virtual void Add(const char* name, SHVLuaMethodBase* method) = 0;

};



// ============================================= implementation ============================================= //

/*************************************
 * Constructor
 *************************************/
template<class T>
SHVLuaMethod<T>::SHVLuaMethod(OnCallback func) : Func(func)
{
}

/*************************************
 * Callback
 *************************************/
template<class T>
void SHVLuaMethod<T>::Callback(SHVLuaClassBase* self, SHVLuaMetaClassBase* meta, SHVLuaScript* script, SHVLuaFuncArgs& args)
{
	(((T*)self)->*Func)(meta, script, args);
}

#endif
