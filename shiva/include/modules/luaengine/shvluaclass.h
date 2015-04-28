#ifndef __SHIVA_LUAENGINE_LUACLASS_H
#define __SHIVA_LUAENGINE_LUACLASS_H

#include "shvluascript.h"
#include "shiva/include/framework/shveventdata.h"


//-=========================================================================================================
/// SHVLuaClass class
//-=========================================================================================================
/**
 * Implement this interface to create a lua class. If you use the generic
 * SHVLuaMetaClass to register it then you will need to implement
 * a constructor that takes a SHVLuaScript* and a SHVLuaFuncArgs from the
 * "<ClassName>:New" function, and a "RegisterMethods" static as well:
\code
class MyLuaClass : public SHVLuaClass<MyLuaClass>
{
	int Counter;
public:

	MyLuaClass(SHVLuaMetaClassBase* meta, SHVLuaScript* script, SHVLuaFuncArgs& args) : SHVLuaClass<MyLuaClass>(meta,script,args), Counter(0)
	{
		if (args.ArgCount())
			Counter = args.ArgAsInt(0);
	}
	
	void DoSomething(SHVLuaMetaClassBase* meta, SHVLuaScript* script, SHVLuaFuncArgs& args)
	{
		SHVConsole::Printf(_S("Do something %d\n"), ++Counter);
		args.PushInt(Counter);
	}
	
	static void RegisterMethods(SHVLuaClassMethods& methods)
	{
		RegisterMethod(methods,"DoSomething",&MyLuaClass::DoSomething);
	}
};

// Example of how to register the class into a script
void RegisterMyLuaClass(SHVLuaScript* script)
{
	script->RegisterClass("MyLuaClass", new SHVLuaMetaClass<MyLuaClass>());
}
\endcode
 * The above example will allow you to do the following in your lua script:
\code
	local myobject = MyLuaClass.new(4) -- Calls constructor of MyLuaClass
	myobject:DoSomething() -- Calls DoSomething of the object created
\endcode
 */
template <class T>
class SHVLuaClass : public SHVLuaClassBase
{
public:

	typedef void (T::*OnCallback)(SHVLuaMetaClassBase* meta, SHVLuaScript* script, SHVLuaFuncArgs& args);
	
	virtual ~SHVLuaClass() {}
	
protected:
	static void RegisterMethod(SHVLuaClassMethods& methods, const char* name, OnCallback func);
	inline SHVLuaClass(SHVLuaMetaClassBase* meta, SHVLuaScript* script, SHVLuaFuncArgs& args) : SHVLuaClassBase(meta,script,args) {}
};


//-=========================================================================================================
/// SHVLuaMetaClass class
//-=========================================================================================================
/**
 * Implementation of a lua script meta class
 */

template <class T>
class SHVLuaMetaClass : public SHVLuaMetaClassBase
{
public:

	// constructor
	SHVLuaMetaClass();

	virtual void RegisterMethods(SHVLuaClassMethods& methods);
	virtual SHVLuaClassBase* New(SHVLuaScript* script, SHVLuaFuncArgs& args);

};


//-=========================================================================================================
/// SHVLuaMetaClassData class
//-=========================================================================================================
/**
 * Implementation of a lua script meta class with extra data
 */

template <class T, class D, class C = const D&>
class SHVLuaMetaClassData : public SHVLuaMetaClassBase
{
public:

	// constructor
	SHVLuaMetaClassData(C d);

	virtual void RegisterMethods(SHVLuaClassMethods& methods);
	virtual SHVLuaClassBase* New(SHVLuaScript* script, SHVLuaFuncArgs& args);

	static D& Get(SHVLuaMetaClassBase* meta);

private:

	D Data;
};


// ============================================= implementation ============================================= //

/*************************************
 * RegisterMethod
 *************************************/
template<class T>
void SHVLuaClass<T>::RegisterMethod(SHVLuaClassMethods& methods, const char* name, OnCallback func)
{
	methods.Add(name,new SHVLuaMethod<T>(func));
}

/*************************************
 * Constructor
 *************************************/
template<class T>
SHVLuaMetaClass<T>::SHVLuaMetaClass()
{
}
template<class T,class D, class C>
SHVLuaMetaClassData<T,D,C>::SHVLuaMetaClassData(C d) : Data(d)
{
}

/*************************************
 * RegisterMethods
 *************************************/
template<class T>
void SHVLuaMetaClass<T>::RegisterMethods(SHVLuaClassMethods& methods)
{
	T::RegisterMethods(methods);
}
template<class T,class D, class C>
void SHVLuaMetaClassData<T,D,C>::RegisterMethods(SHVLuaClassMethods& methods)
{
	T::RegisterMethods(methods);
}

/*************************************
 * New
 *************************************/
template<class T>
SHVLuaClassBase* SHVLuaMetaClass<T>::New(SHVLuaScript* script, SHVLuaFuncArgs& args)
{
	return new T(this, script, args);
}
template<class T,class D, class C>
SHVLuaClassBase* SHVLuaMetaClassData<T,D,C>::New(SHVLuaScript* script, SHVLuaFuncArgs& args)
{
	return new T(this, script, args);
}

/*************************************
 * Get
 *************************************/
template<class T,class D, class C>
D& SHVLuaMetaClassData<T,D,C>::Get(SHVLuaMetaClassBase* meta)
{
	return ((SHVLuaMetaClassData<T,D,C>*) meta)->Data;
}

#endif
