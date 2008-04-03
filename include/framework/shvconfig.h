#ifndef __SHIVA_FRAMEWORK_CONFIG_H
#define __SHIVA_FRAMEWORK_CONFIG_H

#include "../shvtypes.h"
#include "../utils/shvstring.h"
#include "../utils/shvrefobject.h"


//-=========================================================================================================
/// SHVConfigNode class - data container in SHVConfig
/**
 * Contains a data item of type SHVInt, SHVDouble, SHVString or void*
 */

class SHVAPI SHVConfigNode
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



//-=========================================================================================================
/// SHVConfig class - replacement for static vars
/**
 * This class is a container for static vars in a module list.
 */

class SHVAPI SHVConfig
{
public:


	// string config values
	// --------------------

	// getting data
	virtual SHVConfigNode& Find(const SHVStringC& name, const SHVStringC& defVal = NULL) = 0;
	virtual SHVConfigNode& FindInt(const SHVStringC& name, SHVInt defVal = SHVInt()) = 0;
	virtual SHVConfigNode& FindPtr(const SHVStringC& name, void* defVal = NULL) = 0;
	virtual SHVConfigNode& FindRef(const SHVStringC& name, SHVRefObject* defVal = NULL) = 0;


	// setting data
	virtual SHVConfigNode& Set(const SHVStringC& name, const SHVStringC& val) = 0;
	virtual SHVConfigNode& Set(const SHVStringC& name, SHVInt val) = 0;
	virtual SHVConfigNode& SetPtr(const SHVStringC& name, void* val) = 0;
	virtual SHVConfigNode& SetRef(const SHVStringC& name, SHVRefObject* val) = 0;
	virtual void Remove(const SHVStringC& name) = 0;


	// Store and load string config values
	virtual SHVBool Load(const SHVStringC fileName) = 0;
	virtual SHVBool Save(const SHVStringC newFileName = NULL) = 0;


	// enumerator config values
	// ------------------------

	// getting data
	virtual SHVConfigNode& Find(const SHVInt& enumerator, const SHVStringC& defVal = NULL) = 0;
	virtual SHVConfigNode& FindInt(const SHVInt& enumerator, SHVInt defVal = SHVInt()) = 0;
	virtual SHVConfigNode& FindPtr(const SHVInt& enumerator, void* defVal = NULL) = 0;
	virtual SHVConfigNode& FindRef(const SHVInt& enumerator, SHVRefObject* defVal = NULL) = 0;


	// setting data
	virtual SHVConfigNode& Set(const SHVInt& enumerator, const SHVStringC& val) = 0;
	virtual SHVConfigNode& Set(const SHVInt& enumerator, SHVInt val) = 0;
	virtual SHVConfigNode& SetPtr(const SHVInt& enumerator, void* val) = 0;
	virtual SHVConfigNode& SetRef(const SHVInt& enumerator, SHVRefObject* val) = 0;
	virtual void Remove(const SHVInt& enumerator) = 0;


protected:
	virtual ~SHVConfig() {}
};


// ============================================= implementation ============================================= //

#endif
