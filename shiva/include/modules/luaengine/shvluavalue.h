#ifndef __SHIVA_LUAENGINE_LUAVALUE_H
#define __SHIVA_LUAENGINE_LUAVALUE_H

#include "../../framework/shveventsubscriber.h"


//-=========================================================================================================
/// SHVLuaValue class
//-=========================================================================================================
/**
 * Defines an object placeholder for a lua value
 */

class SHVLuaValue
{
public:


	enum Types
	{
		TypeNull,
		TypeInt,
		TypeDouble,
		TypeString,
		TypeBool,
		TypeRefOject
	};


	virtual ~SHVLuaValue() {}


	virtual Types GetType() const = 0;
	virtual const char* GetRefType() const = 0;


	virtual SHVInt AsInt() const = 0;
	virtual SHVDouble AsDouble() const = 0;
	virtual SHVStringBuffer AsString() const = 0;
	virtual SHVBool AsBool() const = 0;
	virtual SHVRefObject* AsRef(const char* typeID = NULL) const = 0;


};
typedef SHVPtrContainer<SHVLuaValue> SHVLuaValuePtr;



//-=========================================================================================================
/// SHVLuaValues class
//-=========================================================================================================
/**
 * Contains an a list of values to be used for function calls
 */

class SHVLuaValues : public SHVRefObject
{
public:

	// For transportation from SHVLuaRefType
	struct RefStruct { SHVRefObject* Obj; const char* TypeID; };

	virtual int GetCount() = 0;
	virtual SHVLuaValue* Get(int idx) = 0;

	virtual SHVLuaValues* AddInt(SHVInt val) = 0;
	virtual SHVLuaValues* AddDouble(SHVDouble val) = 0;
	virtual SHVLuaValues* AddString(const SHVStringC val) = 0;
	virtual SHVLuaValues* AddBool(SHVBool val) = 0;
	virtual SHVLuaValues* AddRef(SHVRefObject* val, const char* typeID = NULL) = 0;
	virtual SHVLuaValues* AddRef(RefStruct val) = 0;

};
typedef SHVRefObjectContainer<SHVLuaValues> SHVLuaValuesRef;

#endif
