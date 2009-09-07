#ifndef __SHIVA_FRAMEWORK_CONFIG_H
#define __SHIVA_FRAMEWORK_CONFIG_H

#include "../shvtypes.h"
#include "../utils/shvstring.h"
#include "../utils/shvrefobject.h"
#include "../utils/shvmathtokenmap.h"


//-=========================================================================================================
/// SHVConfigNode class - data container in SHVConfig
/**
 * Contains a data item of type SHVInt, SHVDouble, SHVString or void*
 */

class SHVAPI SHVConfigNode : public SHVRefObject
{
public:


	// data access
	virtual SHVInt ToInt() = 0;
	virtual SHVDouble ToDouble() = 0;
	virtual SHVStringBuffer ToString() = 0;
	virtual void* ToPtr() = 0;
	virtual SHVRefObject* ToRef() = 0;

	// status
	virtual SHVBool IsNull() = 0;


protected:
	virtual ~SHVConfigNode() {}
};
typedef SHVRefObjectContainer<SHVConfigNode> SHVConfigNodeRef;



//-=========================================================================================================
/// SHVConfig class - replacement for static vars
/**
 * This class is a container for static vars in a module list.
 */

class SHVAPI SHVConfig : public SHVMathTokenMap
{
public:


	// string config values
	// --------------------

	// getting data
	virtual SHVConfigNodeRef Find(const SHVStringC name, const SHVStringC defVal = NULL) = 0;
	virtual SHVConfigNodeRef FindInt(const SHVStringC name, SHVInt defVal = SHVInt()) = 0;
	virtual SHVConfigNodeRef FindPtr(const SHVStringC name, void* defVal = NULL) = 0;
	virtual SHVConfigNodeRef FindRef(const SHVStringC name, SHVRefObject* defVal = NULL) = 0;


	// query data
	virtual SHVBool Contains(const SHVStringC name) = 0;
	virtual SHVBool Get(const SHVStringC name, SHVString& value) = 0;
	virtual SHVBool GetInt(const SHVStringC name, SHVInt& value) = 0;
	virtual SHVBool GetPtr(const SHVStringC name, void*& value) = 0;
	virtual SHVBool GetRef(const SHVStringC name, SHVRefObject*& value) = 0;


	// setting data
	virtual SHVConfigNodeRef Set(const SHVStringC name, const SHVStringC val) = 0;
	virtual SHVConfigNodeRef Set(const SHVStringC name, SHVInt val) = 0;
	virtual SHVConfigNodeRef SetPtr(const SHVStringC name, void* val) = 0;
	virtual SHVConfigNodeRef SetRef(const SHVStringC name, SHVRefObject* val) = 0;
	virtual void Remove(const SHVStringC name) = 0;


	// Store and load string config values
	virtual SHVBool Load(const SHVStringC fileName) = 0;
	virtual SHVBool Save(const SHVStringC newFileName = NULL) = 0;


	// enumerator config values
	// ------------------------

	// getting data
	virtual SHVConfigNodeRef Find(const SHVInt& enumerator, const SHVStringC defVal = NULL) = 0;
	virtual SHVConfigNodeRef FindInt(const SHVInt& enumerator, SHVInt defVal = SHVInt()) = 0;
	virtual SHVConfigNodeRef FindPtr(const SHVInt& enumerator, void* defVal = NULL) = 0;
	virtual SHVConfigNodeRef FindRef(const SHVInt& enumerator, SHVRefObject* defVal = NULL) = 0;


	// query data
	virtual SHVBool Contains(const SHVInt& enumerator) = 0;
	virtual SHVBool Get(const SHVInt& enumerator, SHVString& value) = 0;
	virtual SHVBool GetInt(const SHVInt& enumerator, SHVInt& value) = 0;
	virtual SHVBool GetPtr(const SHVInt& enumerator, void*& value) = 0;
	virtual SHVBool GetRef(const SHVInt& enumerator, SHVRefObject*& value) = 0;


	// setting data
	virtual SHVConfigNodeRef Set(const SHVInt& enumerator, const SHVStringC val) = 0;
	virtual SHVConfigNodeRef Set(const SHVInt& enumerator, SHVInt val) = 0;
	virtual SHVConfigNodeRef SetPtr(const SHVInt& enumerator, void* val) = 0;
	virtual SHVConfigNodeRef SetRef(const SHVInt& enumerator, SHVRefObject* val) = 0;
	virtual void Remove(const SHVInt& enumerator) = 0;


	// From SHVMathTokenMap
	virtual bool LookupValue(const SHVStringC token, double& result) = 0;


protected:
	virtual ~SHVConfig() {}
};


// ============================================= implementation ============================================= //

#endif
