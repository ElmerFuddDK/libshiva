#ifndef __SHIVA_LUAENGINEIMPL_LUAVALUE_H
#define __SHIVA_LUAENGINEIMPL_LUAVALUE_H

#include "shiva/include/modules/luaengine/shvluavalue.h"
#include "shiva/include/utils/shvvector.h"


//-=========================================================================================================
/// SHVLuaValueImpl class
//-=========================================================================================================
/**
 * Defines an object placeholder for a lua value
 */

class SHVLuaValueImpl : public SHVLuaValue
{
public:


	virtual ~SHVLuaValueImpl();


	virtual Types GetType() const;


	virtual SHVInt AsInt() const;
	virtual SHVDouble AsDouble() const;
	virtual SHVStringBuffer AsString() const;
	virtual SHVBool AsBool() const;
	virtual SHVRefObject* AsRef() const;


	static SHVLuaValueImpl* NewNull();
	static SHVLuaValueImpl* NewInt(SHVInt val);
	static SHVLuaValueImpl* NewDouble(SHVDouble val);
	static SHVLuaValueImpl* NewString(const SHVStringC val);
	static SHVLuaValueImpl* NewBool(SHVBool val);
	static SHVLuaValueImpl* NewRef(SHVRefObject* val);


private:

	SHVLuaValueImpl();

	///\cond INTERNAL
	Types Type;
	union DataUnion
	{
		int IntVal;
		double DoubleVal;
		SHVString* StringVal;
		int BoolVal;
		SHVRefObject* ObjVal;
	} Data;
	///\endcond
};


//-=========================================================================================================
/// SHVLuaValuesImpl class
//-=========================================================================================================
/**
 * Contains an a list of values to be used for function calls
 */

class SHVLuaValuesImpl : public SHVLuaValues
{
public:

	SHVLuaValuesImpl();

	virtual int GetCount();
	virtual SHVLuaValue* Get(int idx);

	virtual SHVLuaValues* AddInt(SHVInt val);
	virtual SHVLuaValues* AddDouble(SHVDouble val);
	virtual SHVLuaValues* AddString(const SHVStringC val);
	virtual SHVLuaValues* AddBool(SHVBool val);
	virtual SHVLuaValues* AddRef(SHVRefObject* val);

	// For internal use
	void AddValue(SHVLuaValue* value);

private:
	///\cond INTERNAL
	SHVVector<SHVLuaValue> Values;
	///\endcond
};
typedef SHVRefObjectContainer<SHVLuaValuesImpl> SHVLuaValuesImplRef;

#endif
