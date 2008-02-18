#ifndef __SHIVA_UTILS_HASHTABLEBASE_H
#define __SHIVA_UTILS_HASHTABLEBASE_H

#include "shvstring.h"
#include "shvdynarraybase.h"

#include "shvvectorbase.h"


class SHVHashDataBase;
class SHVHashIteratorBase;


//-=========================================================================================================
/// SHVHashTableBase class - base class for hashing containers
/**
 * SHVHashTableBase contains basic functionality for managing a map of hashable data. It does not contain
 * any code for managing the data inside it, or for creating the hash values, but rather relies on the
 * user to supply functions for this.\n
 * It is designed with low memory footprint and portability in mind.\n
 * It is intended to be privately derived, and then exporting whatever needed from the sub class.
 \note This class is not intended for basic usage. Please use one of the derived classes instead.
 \note It does not provide an indexing operator since it does not know how to create the item types it contains
 */

class SHVAPI SHVHashTableBase
{
friend class SHVHashIteratorBase;
public:


	// defines
	typedef size_t (*CreateHashFunc)(SHVHashDataBase* data);
	typedef bool (*MatchFunc)(SHVHashDataBase** data1,SHVHashDataBase** data2);
	typedef void (*DestroyFunc)(SHVHashDataBase* data);


	SHVHashTableBase(MatchFunc match, CreateHashFunc createHash, DestroyFunc destroy, size_t estimateCount=1000);
	~SHVHashTableBase();


	// properties
	bool IsEmpty() const;
	size_t GetCount() const;
	size_t GetSize() const;


	// Hashtable management
	void Clear();
	void Resize(size_t estimateCount);
	void SetBucketCount(size_t buckets);

	void Insert(SHVHashDataBase* data);
	SHVHashDataBase* Find(SHVHashDataBase* key);
	void Remove(SHVHashDataBase* key);

	SHVHashIteratorBase CreateIterator();


	// hashing functions
	static size_t HashInt(int val);
	static size_t HashString(const SHVStringC& str);

	static size_t CalculateBuckets(size_t estimateCount);


	// helper functions for sub classes
protected:
	inline void OverrideMatchFunc(MatchFunc func);
	inline void OverrideCreateHashFunc(CreateHashFunc func);
	inline void OverrideDestroyFunc(DestroyFunc func);


private:
	///\cond INTERNAL
	void InsertReal(SHVHashDataBase* data);
	size_t FindInBucket(SHVVectorBase* bucket, SHVHashDataBase* data, size_t key);

	int Count;
	SHVDynArrayBase Buckets;

	MatchFunc Match;
	CreateHashFunc CreateHash;
	DestroyFunc Destroy;
	///\endcond
};



//-=========================================================================================================
/// SHVHashIteratorBase class
/**
 * Base class for iterating a hash table.
 */

class SHVAPI SHVHashIteratorBase
{
friend class SHVHashTableBase;

	SHVHashTableBase& Table;
	bool Eof;
	size_t Bucket;
	size_t Index;
	
	SHVHashIteratorBase(SHVHashTableBase& table);
public:
	bool MoveNext();
	operator bool();
	SHVHashDataBase* Get();

	void Reset();
};



//-=========================================================================================================
/// SHVHashDataBase class - empty class for holding data
/**
 * Base class for holding data in a hash table.
 */

class SHVHashDataBase
{
friend class SHVHashTableBase;
	size_t HashKey;
public:
	inline SHVHashDataBase() {HashKey=0;}
};



// ============================================ implementation ============================================ //

void SHVHashTableBase::OverrideMatchFunc(MatchFunc func)
{ Match = func; }
void SHVHashTableBase::OverrideCreateHashFunc(CreateHashFunc func)
{ CreateHash = func; }
void SHVHashTableBase::OverrideDestroyFunc(DestroyFunc func)
{ Destroy = func; }
#endif
