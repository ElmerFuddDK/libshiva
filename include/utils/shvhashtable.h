#ifndef __SHIVA_UTILS_HASHTABLE_H
#define __SHIVA_UTILS_HASHTABLE_H

#include "shvhashtablebase.h"
#include "shvstring.h"


// forward declares
template<class K, class D, class CopyK = const K, class CopyD = const D>
class SHVHashTableIterator;
template<class D, class CopyD = const D&>
class SHVHashTableStringIterator;



//-=========================================================================================================
/// SHVHashTable class template
/**
 * Fast hashtable designed with low memory footprint in mind. Unfortunately this means it has no logic for
 * auto-rehashing. You must provide this logic yourself, by monitoring the amount of items compared to buckets
 * and then calling one of the rehashing functions.\n
 * It uses the SHVHashValue operator on the key data type for creating hash values.
 */

template<class K, class D, class CopyK = const K, class CopyD = const D>
class SHVHashTable : protected SHVHashTableBase
{
friend class SHVHashTableIterator<K,D,CopyK,CopyD>;
public:


	// constructor
	SHVHashTable(size_t estimateCount=1000);


	// properties
	inline bool IsEmpty() const;
	inline size_t GetCount() const;
	inline size_t GetSize() const;


	// hashtable management
	inline void Clear();
	inline void Resize(size_t estimateCount);
	inline void SetBucketCount(size_t buckets);

	void Insert(CopyK& key, CopyD& data);
	D* Find(CopyK& key);
	void Remove(CopyK& key);


	// operators
	D& operator[](CopyK& key);


protected:
///\cond INTERNAL
	inline SHVHashIteratorBase CreateIterator();
///\endcond

	static SHVHashValue CreateHash(SHVHashDataBase* data);
	static bool Match(SHVHashDataBase** data1,SHVHashDataBase** data2);
	static void Destroy(SHVHashDataBase* data);
};



//-=========================================================================================================
/// SHVHashTableString convenience class template
/**
 * Use this class if you want to use strings as keys. It is optimized for string key, in regards to
 * comparison when matching 2 keys, and it uses the constant string class as copier, reducing the
 * amount of string copying to a minimum.
 */

template<class D, class CopyD = const D>
class SHVHashTableString : public SHVHashTable<SHVString,D,const SHVStringC,CopyD>
{
friend class SHVHashTableStringIterator<D,CopyD>;
public:
	SHVHashTableString(size_t estimateCount=1000);
private:
///\cond INTERNAL
	inline SHVHashIteratorBase CreateIterator();
///\endcond
	static bool MatchString(SHVHashDataBase** data1,SHVHashDataBase** data2);
};



//-=========================================================================================================
/// SHVHashTableIterator class template
/**
 * Iterator class for SHVHashTable
 */

template<class K, class D, class CopyK, class CopyD>
class SHVHashTableIterator
{
	SHVHashIteratorBase Iterator;
public:
	inline SHVHashTableIterator(SHVHashTable<K,D,CopyK,CopyD>& table);
	inline bool MoveNext();
	inline operator bool();
	K& GetKey();
	D& GetData();

	void Reset();
};



//-=========================================================================================================
/// SHVHashTableStringIterator class template
//-=========================================================================================================
/**
 * Iterator class for SHVHashTableString
 */

template<class D, class CopyD>
class SHVHashTableStringIterator
{
	SHVHashIteratorBase Iterator;
public:
	inline SHVHashTableStringIterator(SHVHashTableString<D,CopyD>& table);
	inline bool MoveNext();
	inline operator bool();
	SHVString& GetKey();
	D& GetData();

	void Reset();
};




// ============================================= implementation ============================================= //

///\cond INTERNAL
template<class K, class D, class CopyK = const K, class CopyD = const D>
class SHVHashKey : public SHVHashDataBase
{
	CopyK& KeyC;
public:
	SHVHashKey(CopyK& key) : KeyC(key) {}
	CopyK& GetKeyC() { return KeyC; }
};

template<class K, class D, class CopyK = const K, class CopyD = const D>
class SHVHashPair : public SHVHashKey<K,D,CopyK,CopyD>
{
	K Key;
	D Data;
public:
	SHVHashPair(CopyK& key, CopyD& data) : SHVHashKey<K,D,CopyK,CopyD>(Key), Key(key), Data(data) {}
	K& GetKey()  { return Key;  }
	D& GetData() { return Data; }
};
///\endcond


/*************************************
 * Constructor
 *************************************/
template<class K, class D, class CopyK, class CopyD>
SHVHashTable<K,D,CopyK,CopyD>::SHVHashTable(size_t estimateCount) : SHVHashTableBase(Match,CreateHash,Destroy,estimateCount) {}
template<class D, class CopyD>
SHVHashTableString<D,CopyD>::SHVHashTableString(size_t estimateCount) : SHVHashTable<SHVString,D,const SHVStringC,CopyD>(estimateCount)
{
	SHVHashTableBase::OverrideMatchFunc(&MatchString);
} 

/*************************************
 * IsEmpty
 *************************************/
template<class K, class D, class CopyK, class CopyD>
bool SHVHashTable<K,D,CopyK,CopyD>::IsEmpty() const		{ return SHVHashTableBase::IsEmpty();  }

/*************************************
 * GetCount
 *************************************/
template<class K, class D, class CopyK, class CopyD>
size_t SHVHashTable<K,D,CopyK,CopyD>::GetCount() const	{ return SHVHashTableBase::GetCount(); }

/*************************************
 * GetSize
 *************************************/
template<class K, class D, class CopyK, class CopyD>
size_t SHVHashTable<K,D,CopyK,CopyD>::GetSize() const	{ return SHVHashTableBase::GetSize();  }

/*************************************
 * Clear
 *************************************/
template<class K, class D, class CopyK, class CopyD>
void SHVHashTable<K,D,CopyK,CopyD>::Clear()			{ SHVHashTableBase::Clear(); }

/*************************************
 * Resize
 *************************************/
template<class K, class D, class CopyK, class CopyD>
void SHVHashTable<K,D,CopyK,CopyD>::Resize(size_t estimateCount)	{ SHVHashTableBase::Resize(estimateCount); }

/*************************************
 * SetBucketCount
 *************************************/
template<class K, class D, class CopyK, class CopyD>
void SHVHashTable<K,D,CopyK,CopyD>::SetBucketCount(size_t buckets)	{ SHVHashTableBase::SetBucketCount(buckets); }

/*************************************
 * Insert
 *************************************/
/// Inserts an item into the table
/**
 \param key Key of the item to insert
 \param data Associated data of the item to insert
 */
template<class K, class D, class CopyK, class CopyD>
void SHVHashTable<K,D,CopyK,CopyD>::Insert(CopyK& key, CopyD& data) { SHVHashTableBase::Insert(new SHVHashPair<K,D,CopyK,CopyD>(key,data)); }

/*************************************
 * Find
 *************************************/
/// Searches for an item, returning a pointer to it, or NULL
/**
 \param key Key to search for
 *
 * Returns the item found, or NULL in case it doesn't exist.
 */
template<class K, class D, class CopyK, class CopyD>
D* SHVHashTable<K,D,CopyK,CopyD>::Find(CopyK& key)
{
SHVHashKey<K,D,CopyK,CopyD> k(key);
SHVHashPair<K,D,CopyK,CopyD>* pair = (SHVHashPair<K,D,CopyK,CopyD>*)SHVHashTableBase::Find(&k);
	return (pair ? &pair->GetData() : NULL);
}

/*************************************
 * Remove
 *************************************/
/// Removes an item by key
/**
 \param key Key to search for
 *
 * Will remove and delete the found item
 */
template<class K, class D, class CopyK, class CopyD>
void SHVHashTable<K,D,CopyK,CopyD>::Remove(CopyK& key)
{
SHVHashKey<K,D,CopyK,CopyD> k(key);
	SHVHashTableBase::Remove(&k);
}

/*************************************
 * index operator
 *************************************/
/// Index operator
/**
 \param key Key to look up
 \return Reference to the found or created item
 *
 * Will create a new item if none exists with the given key.
 \note Don't use this function for inserting items, since it will result in an extra lookup, compared to using Insert
 */
template<class K, class D, class CopyK, class CopyD>
D& SHVHashTable<K,D,CopyK,CopyD>::operator[](CopyK& key)
{
SHVHashKey<K,D,CopyK,CopyD> k(key);
SHVHashPair<K,D,CopyK,CopyD>* pair = (SHVHashPair<K,D,CopyK,CopyD>*)SHVHashTableBase::Find(&k);

	if (!pair)
		SHVHashTableBase::Insert(pair = new SHVHashPair<K,D,CopyK,CopyD>(key,D()));

	return pair->GetData();
}


///\cond INTERNAL
/*************************************
 * CreateIterator
 *************************************/
template<class K, class D, class CopyK, class CopyD>
SHVHashIteratorBase SHVHashTable<K,D,CopyK,CopyD>::CreateIterator() { return SHVHashTableBase::CreateIterator(); }
template<class D, class CopyD>
SHVHashIteratorBase SHVHashTableString<D,CopyD>::CreateIterator() { return SHVHashTableBase::CreateIterator(); }
///\endcond

/*************************************
 * CreateHash
 *************************************/
/// Hashing function
/**
 * This function is used for creating the hash value for a key, using the size_t operator on the key.
 */
template<class K, class D, class CopyK, class CopyD>
SHVHashValue SHVHashTable<K,D,CopyK,CopyD>::CreateHash(SHVHashDataBase* data)
{
SHVHashKey<K,D,CopyK,CopyD>* key = (SHVHashKey<K,D,CopyK,CopyD>*)data;
	return (SHVHashValue)key->GetKeyC();
}

/*************************************
 * Match
 *************************************/
/// Matching function
/**
 * Is used for comparing keys.
 */
template<class K, class D, class CopyK, class CopyD>
bool SHVHashTable<K,D,CopyK,CopyD>::Match(SHVHashDataBase** data1,SHVHashDataBase** data2)
{
SHVHashKey<K,D,CopyK,CopyD>* pair1 = (SHVHashKey<K,D,CopyK,CopyD>*)*data1;
SHVHashKey<K,D,CopyK,CopyD>* pair2 = (SHVHashKey<K,D,CopyK,CopyD>*)*data2;
	
	if (!data1 && data2)
		return false;
	if (data1 && !data2)
		return false;
	if (!data1 && !data2)
		return true;

	return ( pair1->GetKeyC() == pair2->GetKeyC() );
}
/// Matching function
/**
 * Is used for comparing keys of type string
 */
template<class D, class CopyD>
bool SHVHashTableString<D,CopyD>::MatchString(SHVHashDataBase** data1,SHVHashDataBase** data2)
{
SHVHashKey<SHVString,D,const SHVStringC,CopyD>* pair1 = (SHVHashKey<SHVString,D,const SHVStringC,CopyD>*)*data1;
SHVHashKey<SHVString,D,const SHVStringC,CopyD>* pair2 = (SHVHashKey<SHVString,D,const SHVStringC,CopyD>*)*data2;
	
	if (!data1 && data2)
		return false;
	if (data1 && !data2)
		return false;
	if (!data1 && !data2)
		return true;

	return ( pair1->GetKeyC() == pair2->GetKeyC() );
}

/*************************************
 * Destroy
 *************************************/
/// Destroy function
/**
 * Is used to destroy the internal data items in the SHVHashTableBase class
 */
template<class K, class D, class CopyK, class CopyD>
void SHVHashTable<K,D,CopyK,CopyD>::Destroy(SHVHashDataBase* data)
{
SHVHashPair<K,D,CopyK,CopyD>* pair = (SHVHashPair<K,D,CopyK,CopyD>*)data;

	if (pair)
		delete pair;
}



/*************************************
 * Iterator::Constructor
 *************************************/
template<class K, class D, class CopyK, class CopyD>
SHVHashTableIterator<K,D,CopyK,CopyD>::SHVHashTableIterator(SHVHashTable<K,D,CopyK,CopyD>& table) : Iterator(table.CreateIterator()) {}
template<class D, class CopyD>
SHVHashTableStringIterator<D,CopyD>::SHVHashTableStringIterator(SHVHashTableString<D,CopyD>& table) : Iterator(table.CreateIterator()) {}

/*************************************
 * Iterator::MoveNext
 *************************************/
/// Moves the iterator to the next position
/**
 \return True if there was a next
 */
template<class K, class D, class CopyK, class CopyD>
bool SHVHashTableIterator<K,D,CopyK,CopyD>::MoveNext() { return Iterator.MoveNext(); }
/// Moves the iterator to the next position
/**
 \return True if there was a next
 */
template<class D, class CopyD>
bool SHVHashTableStringIterator<D,CopyD>::MoveNext() { return Iterator.MoveNext(); }

/*************************************
 * Iterator::operator bool
 *************************************/
/// State of iterator - true if position is valid
/**
 \return Valid state of iterator
 */
template<class K, class D, class CopyK, class CopyD>
SHVHashTableIterator<K,D,CopyK,CopyD>::operator bool() { return Iterator; }
/// State of iterator - true if position is valid
/**
 \return Valid state of iterator
 */
template<class D, class CopyD>
SHVHashTableStringIterator<D,CopyD>::operator bool() { return Iterator; }

/*************************************
 * Iterator::GetKey
 *************************************/
/// Returns key of current item pointed to
template<class K, class D, class CopyK, class CopyD>
K& SHVHashTableIterator<K,D,CopyK,CopyD>::GetKey()
{
SHVHashPair<K,D,CopyK,CopyD>* pair = (SHVHashPair<K,D,CopyK,CopyD>*)Iterator.Get();
	return pair->GetKey();
}
/// Returns key of current item pointed to
template<class D, class CopyD>
SHVString& SHVHashTableStringIterator<D,CopyD>::GetKey()
{
SHVHashPair<SHVString,D,const SHVStringC,CopyD>* pair = (SHVHashPair<SHVString,D,const SHVStringC,CopyD>*)Iterator.Get();
	return pair->GetKey();
}

/*************************************
 * Iterator::GetData
 *************************************/
/// Returns data of current item pointed to
template<class K, class D, class CopyK, class CopyD>
D& SHVHashTableIterator<K,D,CopyK,CopyD>::GetData()
{
SHVHashPair<K,D,CopyK,CopyD>* pair = (SHVHashPair<K,D,CopyK,CopyD>*)Iterator.Get();
	return pair->GetData();
}
/// Returns data of current item pointed to
template<class D, class CopyD>
D& SHVHashTableStringIterator<D,CopyD>::GetData()
{
SHVHashPair<SHVString,D,const SHVStringC,CopyD>* pair = (SHVHashPair<SHVString,D,const SHVStringC,CopyD>*)Iterator.Get();
	return pair->GetData();
}

/*************************************
 * Iterator::Reset
 *************************************/
/// Resets the iterator to BOF
template<class K, class D, class CopyK, class CopyD>
void SHVHashTableIterator<K,D,CopyK,CopyD>::Reset()
{
	Iterator.Reset();
}
/// Resets the iterator to BOF
template<class D, class CopyD>
void SHVHashTableStringIterator<D,CopyD>::Reset()
{
	Iterator.Reset();
}

#endif
