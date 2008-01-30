/*
 *   Copyright (C) 2008 by Lars Eriksen
 *
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 of the
 *   License, or (at your option) any later version with the following
 *   exeptions:
 *
 *   1) Static linking to the library does not constitute derivative work
 *      and does not require the author to provide source code for the
 *      application.
 *      Compiling applications with the source code directly linked in is
 *      Considered static linking as well.
 *
 *   2) You do not have to provide a copy of the license with programs
 *      that are linked against this code.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "stdafx.h"
#include "../../../include/platformspc.h"
#include "../../../include/utils/shvhashtablebase.h"


#define HASHTABLE_VECTORGROWSIZE 10

//=========================================================================================================
// SHVHashTableBase class - base class for hashing containers
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
/// Default constructor
/**
 \param match Function for matching 2 items
 \param createHash Function for creating a hash value
 \param destroy Function for destroying an item
 \param estimateCount Estimated amount of items to be inserted (optional)
 */
SHVHashTableBase::SHVHashTableBase(MatchFunc match, CreateHashFunc createHash, DestroyFunc destroy, size_t estimateCount) : Buckets(5,(int)CalculateBuckets(estimateCount),true)
{
	Match = match;
	CreateHash = createHash;
	Destroy = destroy;
	Count = 0;
}

/*************************************
 * Destructor
 *************************************/
SHVHashTableBase::~SHVHashTableBase()
{
	Clear();
}

/*************************************
 * IsEmpty
 *************************************/
/// Returns true if the hash table has no items
bool SHVHashTableBase::IsEmpty() const
{
	return (Count == 0);
}

/*************************************
 * GetCount
 *************************************/
/// Get the amount of items in the hash table
/**
 \return Amount of items
 */
size_t SHVHashTableBase::GetCount() const
{
	return Count;
}

/*************************************
 * GetSize
 *************************************/
/// Get the amount of buckets in the hash table
/**
 \return Bucket count
 */
size_t SHVHashTableBase::GetSize() const
{
	return Buckets.GetArrayLen();
}

/*************************************
 * Clear
 *************************************/
/// Remove all items in the table
void SHVHashTableBase::Clear()
{
SHVVectorBase* bucket;

	for (size_t i=0; i<Buckets.GetCount(); i++)
	{
		if (Buckets[i])
		{
			bucket = (SHVVectorBase*)Buckets[i];
			bucket->Clear((SHVDynArrayBase::DestroyFunc)Destroy);
			Buckets.Remove(i);
			delete bucket;
		}
	}

	Count = 0;

	Buckets.Truncate();
}

/*************************************
 * Resize
 *************************************/
/// Resizes the amount of buckets according to the estimated amount of items
/**
 \param estimateCount Estimated amount of items to be inserted
 *
 * Will calculate the amount of buckets to use from the estimated amount if items
 * in a not very scientific way.\n
 * The operation should be fairly fast, even with alot of items present already.
 * Each item has its hash value cached, so it will be easy to re-insert them into
 * the new buckets.
 */
void SHVHashTableBase::Resize(size_t estimateCount)
{
size_t newBucketCount = CalculateBuckets(estimateCount);

	if (!newBucketCount)
		newBucketCount = 1;

	SetBucketCount(newBucketCount);
}

/*************************************
 * SetBucketCount
 *************************************/
/// Resizes the amount of buckets
/**
 \param buckets Number of buckets to use
 *
 * The operation should be fairly fast, even with alot of items present already.
 * Each item has its hash value cached, so it will be easy to re-insert them into
 * the new buckets.
 */
void SHVHashTableBase::SetBucketCount(size_t buckets)
{
	if (buckets != Buckets.GetCount())
	{
	SHVDynArrayBase tmpContainer(10,Count);
	SHVVectorBase* bucket;
	size_t i,j, count;

		for (i=0; i<Buckets.GetCount(); i++)
		{
			if (Buckets[i])
			{
				bucket = (SHVVectorBase*)Buckets[i];
				count=bucket->CalculateCount();
				for (j=0; j<count; j++)
				{
					if ( (*bucket)[j] )
						tmpContainer.Add(bucket->Remove(j));
				}
			}
		}

		Clear();
		Buckets.ClearAndInit((int)buckets);

		for (i=0; i<tmpContainer.GetCount(); i++)
		{
			InsertReal((SHVHashDataBase*)tmpContainer[i]);
		}
	}
}

/*************************************
 * Insert
 *************************************/
/// Inserts an item into the table
/**
 \param data Item to be inserted
 */
void SHVHashTableBase::Insert(SHVHashDataBase* data)
{
	data->HashKey = CreateHash(data);
	
	InsertReal(data);
}

/*************************************
 * Find
 *************************************/
/// Searches for an item, returning a pointer to it, or NULL
/**
 \param key Key to search for
 *
 * Returns the item found, or NULL in case it doesn't exist.
 */
SHVHashDataBase* SHVHashTableBase::Find(SHVHashDataBase* key)
{
size_t hashKey = CreateHash(key);
size_t index = hashKey%GetSize();
SHVHashDataBase* retVal = NULL;

	if (Buckets[index])
	{
	SHVVectorBase* bucket = (SHVVectorBase*)Buckets[index];

		index = FindInBucket(bucket,key,hashKey);

		if (index != SHVDynArrayBase::InvalidIndex)
			retVal = (SHVHashDataBase*)(*bucket)[index];
	}

	return retVal;
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
void SHVHashTableBase::Remove(SHVHashDataBase* key)
{
size_t hashKey = CreateHash(key);
size_t index = hashKey%GetSize();

	if (Buckets[index])
	{
	SHVVectorBase* bucket = (SHVVectorBase*)Buckets[index];

		index = FindInBucket(bucket,key,hashKey);

		if (index != SHVDynArrayBase::InvalidIndex)
		{
		void* last = bucket->Pop(HASHTABLE_VECTORGROWSIZE);

			if (index == bucket->CalculateCount())
				Destroy((SHVHashDataBase*)last);
			else
				Destroy((SHVHashDataBase*)bucket->Replace(index,last));

			Count--;
		}
	}
}

/*************************************
 * CreateIterator
 *************************************/
/// Creates a basic iterator for the table
/**
 \return Iterator
 *
 * Use this iterator in a derived iterator class for your own hash table sub class.
 */
SHVHashIteratorBase SHVHashTableBase::CreateIterator()
{
	return SHVHashIteratorBase(*this);
}


/*************************************
 * HashInt
 *************************************/
/// Hashing function for integers
size_t SHVHashTableBase::HashInt(int val)
{
	return val; // not good enough
}

/*************************************
 * HashString
 *************************************/
/// Hashing function for strings
size_t SHVHashTableBase::HashString(const SHVStringC& str)
{
size_t retVal = 0;
	for(const SHVTChar* ch = str.GetSafeBuffer(); *ch; ch++)
		retVal = *ch + retVal * 5;
	return retVal;
}

///\cond INTERNAL
/*************************************
 * InsertReal
 *************************************/
void SHVHashTableBase::InsertReal(SHVHashDataBase* data)
{
size_t index = data->HashKey%GetSize();
SHVVectorBase* bucket;
bool added = false;

	if (Buckets[index])
	{
		bucket = (SHVVectorBase*)Buckets[index];
		index = FindInBucket(bucket,data,data->HashKey);

		if (index != SHVDynArrayBase::InvalidIndex)
		{
			added = true;
			Destroy((SHVHashDataBase*)bucket->Replace(index,data));
		}
	}
	else
	{
		bucket = new SHVVectorBase();
		Buckets.InsertAt(index, bucket);
	}
	
	if (!added)
	{
		bucket->Add(data,HASHTABLE_VECTORGROWSIZE);
		Count++;
	}

}

/*************************************
 * FindInBucket
 *************************************/
size_t SHVHashTableBase::FindInBucket(SHVVectorBase* bucket, SHVHashDataBase* data, size_t key)
{
size_t retVal = SHVDynArrayBase::InvalidIndex;
	if (bucket)
	{
	bool found = false;
	SHVHashDataBase* pData;
	size_t count = bucket->CalculateCount();
		for (size_t i=0; i<count && !found; i++)
		{
			if ((*bucket)[i])
			{
				pData = (SHVHashDataBase*)(*bucket)[i];
				if (pData->HashKey == key && Match(&data, &pData))
				{
					found = true;
					retVal = i;
				}
			}
		}
	}
	
	return retVal;
}
///\endcond


/*************************************
 * CalculateBuckets
 *************************************/
/// Simple function for calculating bucket count from an estimated amount
size_t SHVHashTableBase::CalculateBuckets(size_t estimate)
{
	return estimate/75;
}


//=========================================================================================================
// SHVHashIteratorBase class
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVHashIteratorBase::SHVHashIteratorBase(SHVHashTableBase& table) : Table(table)
{
	Reset();
}

/*************************************
 * MoveNext
 *************************************/
bool SHVHashIteratorBase::MoveNext()
{
bool running = true;
	if (!Eof)
		Index++;
	else
		Eof = false;
	

	while (Bucket < Table.GetSize() && running)
	{
		if (!Table.Buckets[Bucket] || Index >= ((SHVVectorBase*)Table.Buckets[Bucket])->CalculateCount() )
		{
			Bucket++;
			Index = 0;
		}
		else if ((*((SHVVectorBase*)Table.Buckets[Bucket]))[Index] == NULL )
		{
			Index++;
		}
		else
			running = false;
	}

	if (Bucket >= Table.GetSize())
	{
		Bucket = Index = 0;
		Eof = true;
	}
	
	return !Eof;
}

/*************************************
 * bool operator
 *************************************/
SHVHashIteratorBase::operator bool()
{ return !Eof; }

/*************************************
 * Get
 *************************************/
SHVHashDataBase* SHVHashIteratorBase::Get()
{
	return (SHVHashDataBase*)(*((SHVVectorBase*)Table.Buckets[Bucket]))[Index];
}

/*************************************
 * Reset
 *************************************/
void SHVHashIteratorBase::Reset()
{
	Eof = true;
	Bucket = Index = 0;
}
