#ifndef __SHIVA_LUAENGINE_LUAREFTYPE_H
#define __SHIVA_LUAENGINE_LUAREFTYPE_H

#include "shvluascript.h"



//-=========================================================================================================
/// SHVLuaRefType template
//-=========================================================================================================
/**
 * Eases type checking with ref objects in lua. Example:
\code
// Example ref object
typedef SHVRefObjectTemplate<int> SHVRefInt;

// Defines the lua type class for SHVRefInt as SHVLuaRefInt
SHVLUAREFTYPE(SHVLuaRefInt,SHVRefInt);

// adding to an argument list:
SHVLuaValues* SHVLuaTest::NewArguments(SHVLuaScript* luaScript)
{
	return luaScript->NewArgs()->AddRef(SHVLuaRefInt().Obj( new SHVRefInt(1234) ));
}

// Pushing as return value
void SHVLuaTest::LuaTestFunc(SHVLuaScript* script, SHVLuaFuncArgs& args)
{
	args.PushRef(SHVLuaRefInt().Obj( new SHVRefInt(512) ));
}

// Get value
SHVRefInt* SHVLuaTest::GetRefInt(SHVLuaValue* val)
{
	return SHVLuaRefInt().Get(val);
}
\endcode
 */

template <class T>
class SHVLuaRefType
{
public:

	SHVLuaRefType(const char* typeID);

	const char* Type() const;

	SHVLuaValues::RefStruct Obj(T* obj) const;

	T* Get(SHVLuaFuncArgs* arg, int idx) const;
	T* Get(SHVLuaValues* vals, int idx) const;
	T* Get(SHVLuaValue* val) const;

	bool Is(SHVLuaValues* vals, int idx) const;
	bool Is(SHVLuaValue* val) const;

private:
	const char* TypeID;
};
#define SHVLUAREFTYPE(name,type) class name : public SHVLuaRefType< type > { public: inline name() : SHVLuaRefType<type>( #type ) {} }



// ============================================= implementation ============================================= //

/*************************************
 * Constuctor
 *************************************/
template<class T>
SHVLuaRefType<T>::SHVLuaRefType(const char* typeID) : TypeID(typeID)
{
}

/*************************************
 * Type
 *************************************/
template<class T>
const char* SHVLuaRefType<T>::Type() const
{
	return TypeID;
}

/*************************************
 * Obj
 *************************************/
template<class T>
SHVLuaValues::RefStruct SHVLuaRefType<T>::Obj(T* obj) const
{
SHVLuaValues::RefStruct retVal = { obj, TypeID };
	return retVal;
}

/*************************************
 * Get
 *************************************/
template<class T>
T* SHVLuaRefType<T>::Get(SHVLuaFuncArgs* arg, int idx) const
{
	return (T*)arg->ArgAsRef(idx,TypeID);
}
template<class T>
T* SHVLuaRefType<T>::Get(SHVLuaValues* vals, int idx) const
{
SHVLuaValue* val = vals->Get(idx);
	return (val ? (T*)val->AsRef(TypeID) : NULL);
}
template<class T>
T* SHVLuaRefType<T>::Get(SHVLuaValue* val) const
{
	return (T*)val->AsRef(TypeID);
}

/*************************************
 * Is
 *************************************/
template<class T>
bool SHVLuaRefType<T>::Is(SHVLuaValues* vals, int idx) const
{
SHVLuaValue* val = vals->Get(idx);
	return (val && val->GetType() == SHVLuaValue::TypeRefOject && SHVString8C(val->GetRefType()) == SHVString8C(TypeID));
}
template<class T>
bool SHVLuaRefType<T>::Is(SHVLuaValue* val) const
{
	return (val && val->GetType() == SHVLuaValue::TypeRefOject && SHVString8C(val->GetRefType()) == SHVString8C(TypeID));
}

#endif
