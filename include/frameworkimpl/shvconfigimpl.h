#ifndef __SHIVA_FRAMEWORK_CONFIGIMPL_H
#define __SHIVA_FRAMEWORK_CONFIGIMPL_H

#include "../framework/shvconfig.h"

#include "../utils/shvhashtable.h"
#include "../utils/shvptrcontainer.h"


class SHVConfigNodeImpl;
typedef SHVPtrContainer<SHVConfigNodeImpl> SHVConfigNodeImplPtr;

//-=========================================================================================================
/// SHVConfigImpl class - implementation of config
/**
 * Contains the implementation of the config interface
 */

class SHVAPI SHVConfigImpl : public SHVConfig
{
public:


	// constructor
	SHVConfigImpl();


	// string config values
	// --------------------

	// getting data
	virtual SHVConfigNode& Find(const SHVStringC& name, const SHVStringC& defVal = NULL);
	virtual SHVConfigNode& FindInt(const SHVStringC& name, SHVInt defVal = SHVInt());
	virtual SHVConfigNode& FindPtr(const SHVStringC& name, void* defVal = NULL);
	virtual SHVConfigNode& FindRef(const SHVStringC& name, SHVRefObject* defVal = NULL);


	// setting data
	virtual SHVConfigNode& Set(const SHVStringC& name, const SHVStringC& val);
	virtual SHVConfigNode& Set(const SHVStringC& name, SHVInt val);
	virtual SHVConfigNode& SetPtr(const SHVStringC& name, void* val);
	virtual SHVConfigNode& SetRef(const SHVStringC& name, SHVRefObject* val);
	virtual void Remove(const SHVStringC& name);


	// enumerator config values
	// ------------------------

	// getting data
	virtual SHVConfigNode& Find(const SHVInt& enumerator, const SHVStringC& defVal = NULL);
	virtual SHVConfigNode& FindInt(const SHVInt& enumerator, SHVInt defVal = SHVInt());
	virtual SHVConfigNode& FindPtr(const SHVInt& enumerator, void* defVal = NULL);
	virtual SHVConfigNode& FindRef(const SHVInt& enumerator, SHVRefObject* defVal = NULL);


	// setting data
	virtual SHVConfigNode& Set(const SHVInt& enumerator, const SHVStringC& val);
	virtual SHVConfigNode& Set(const SHVInt& enumerator, SHVInt val);
	virtual SHVConfigNode& SetPtr(const SHVInt& enumerator, void* val);
	virtual SHVConfigNode& SetRef(const SHVInt& enumerator, SHVRefObject* val);
	virtual void Remove(const SHVInt& enumerator);


	void Clear();

private:
	///\cond INTERNAL
	SHVHashTableString<SHVConfigNodeImplPtr> StringEntries;
	SHVHashTable<SHVInt,SHVConfigNodeImplPtr> EnumEntries;
	///\endcond
};


// ============================================= implementation ============================================= //

///\cond INTERNAL

class SHVConfigNodeImpl : public SHVConfigNode
{
public:


	SHVConfigNodeImpl();
	virtual ~SHVConfigNodeImpl() {}


	// data access
	virtual SHVInt ToInt() = 0;
	virtual SHVDouble ToDouble() = 0;
	virtual SHVStringBuffer ToString() = 0;
	virtual void* ToPtr() = 0;
	virtual SHVRefObject* ToRef() = 0;


	// status
	virtual SHVBool IsNull() = 0;


	// storage
	virtual SHVStringBuffer GetStorageString(const SHVStringC& name) = 0;

};


class SHVConfigNodeImplInt : public SHVConfigNodeImpl
{
public:


	SHVConfigNodeImplInt(SHVInt val);


	// data access
	virtual SHVInt ToInt();
	virtual SHVDouble ToDouble();
	virtual SHVStringBuffer ToString();
	virtual void* ToPtr();
	virtual SHVRefObject* ToRef();


	// status
	virtual SHVBool IsNull();


	// storage
	virtual SHVStringBuffer GetStorageString(const SHVStringC& name);


private:
	SHVInt Value;
};


class SHVConfigNodeImplString : public SHVConfigNodeImpl
{
public:


	SHVConfigNodeImplString(const SHVStringC& val);


	// data access
	virtual SHVInt ToInt();
	virtual SHVDouble ToDouble();
	virtual SHVStringBuffer ToString();
	virtual void* ToPtr();
	virtual SHVRefObject* ToRef();


	// status
	virtual SHVBool IsNull();


	// storage
	virtual SHVStringBuffer GetStorageString(const SHVStringC& name);


private:
	SHVString Value;
};


class SHVConfigNodeImplPointer : public SHVConfigNodeImpl
{
public:


	SHVConfigNodeImplPointer(void* val);


	// data access
	virtual SHVInt ToInt();
	virtual SHVDouble ToDouble();
	virtual SHVStringBuffer ToString();
	virtual void* ToPtr();
	virtual SHVRefObject* ToRef();


	// status
	virtual SHVBool IsNull();


	// storage
	virtual SHVStringBuffer GetStorageString(const SHVStringC& name);


private:
	void* Value;
};

class SHVConfigNodeImplRef : public SHVConfigNodeImpl
{
public:


	SHVConfigNodeImplRef(SHVRefObject* val);


	// data access
	virtual SHVInt ToInt();
	virtual SHVDouble ToDouble();
	virtual SHVStringBuffer ToString();
	virtual void* ToPtr();
	virtual SHVRefObject* ToRef();


	// status
	virtual SHVBool IsNull();


	// storage
	virtual SHVStringBuffer GetStorageString(const SHVStringC& name);


private:
	SHVRefObjectContainer<SHVRefObject> Value;
};

///\endcond
#endif
