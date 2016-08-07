#ifndef __SHIVA_UTILS_DYNARRAY_H
#define __SHIVA_UTILS_DYNARRAY_H

#include "shvdynarraybase.h"
#include "shvptrcontainer.h"
///\cond INTERNAL
template<class T, class S> class SHVDynArrayBuffer;
///\endcond

//-=========================================================================================================
/// SHVDynArray template class
/**
 * Manages an array of object of a given type
 */

template< class T, class S = T >
class SHVDynArray : SHVDynArrayBase
{
public:

	// defines
	typedef int (*SortFunc)(T**,T**);
	typedef int (*MatchFunc)(S**,T**);
	typedef void (*DestroyFunc)(T*);
	enum { InvalidIndex = SIZE_T_MAX };


	SHVDynArray(int growSize = 10, int initSize=0);
	SHVDynArray(const SHVDynArrayBuffer<T,S>& buffer);
	~SHVDynArray();
	
	inline SHVDynArray<T,S>& operator=(const SHVDynArrayBuffer<T,S>& buffer);
	inline SHVDynArray<T,S>& operator+=(const SHVDynArrayBuffer<T,S>& buffer);


	// properties
	inline size_t GetCount() const;
	inline size_t GetArrayLen() const;
	inline size_t GetGrowSize() const;
	inline void   SetGrowSize(size_t s);


	// array management
	inline void ClearAndInit(int initSize=0);
	inline bool Resize(int count);

	inline T* operator[](size_t index);
	inline const T* operator[](size_t index) const;
	inline size_t Add(T* item);
	inline SHVPtrContainer<T> Remove(size_t index);
	inline SHVPtrContainer<T> Replace(size_t index, T* item);

	inline void Truncate();
	inline void Compress();


	// sorting
	inline void Sort(SortFunc func);
	inline void BubbleSort(size_t index, SortFunc func);
	inline bool Swap(size_t index1, size_t index2);


	// searching
	inline size_t FindMatchIndex(S* val, MatchFunc func);
	inline T* FindMatch(S* val, MatchFunc func);
	inline size_t FindFirstMatchIndex(S* val, MatchFunc func);
	inline T* FindFirstMatch(S* val, MatchFunc func);
	
	inline SHVDynArrayBuffer<T,S> ReleaseBuffer();

private:
	///\cond INTERNAL
	static void DefaultDestroy(T* val);
	DestroyFunc Destroy;
	///\endcond
};




// ============================================= implementation ============================================= //
/// \class SHVDynArray shvdynarray.h "shiva/include/utils/shvdynarray.h"

///\cond INTERNAL
template<class T,  class S>
class SHVDynArrayBuffer : public SHVDynArrayBufferBase
{
friend class SHVDynArray<T,S>;
	SHVDynArrayBase::DestroyFunc Destroy;
	SHVDynArrayBuffer(const SHVDynArrayBufferBase& buffer, SHVDynArrayBase::DestroyFunc destroy) : SHVDynArrayBufferBase(buffer), Destroy(destroy) {}
public:
	SHVDynArrayBuffer(const SHVDynArrayBuffer<T,S>& buffer) : SHVDynArrayBufferBase(buffer), Destroy(buffer.Destroy) {}
	virtual ~SHVDynArrayBuffer() { SHVDynArrayBufferBase::Clear(Destroy); }
};
///\endcond

/*************************************
 * Constructor
 *************************************/
/// Constructs the dynamic array
/**
 \param growSize The number of items to expand the array with - must be positive
 \param initSize The initial size of the array (optional)
 */
template<class T, class S>
SHVDynArray<T,S>::SHVDynArray(int growSize, int initSize) : SHVDynArrayBase(growSize,initSize), Destroy(DefaultDestroy)
{}
template<class T, class S>
SHVDynArray<T,S>::SHVDynArray(const SHVDynArrayBuffer<T,S>& buffer) : SHVDynArrayBase(buffer), Destroy((SHVDynArray<T,S>::DestroyFunc)buffer.Destroy)
{}

/*************************************
 * Destructor
 *************************************/
template<class T, class S>
SHVDynArray<T,S>::~SHVDynArray()
{
	ClearAndInit();
}

/*************************************
 * operator=
 *************************************/
template<class T, class S>
SHVDynArray<T,S>& SHVDynArray<T,S>::operator=(const SHVDynArrayBuffer<T,S>& buffer)
{
	ClearAndInit();
	SHVDynArrayBase::operator =(buffer);
	Destroy = (SHVDynArray<T,S>::DestroyFunc)buffer.Destroy;
	return *this;
}

/*************************************
 * operator+=
 *************************************/
template<class T, class S>
SHVDynArray<T,S>& SHVDynArray<T,S>::operator+=(const SHVDynArrayBuffer<T,S>& buffer)
{
	SHVDynArrayBase::operator +=(buffer);
	return *this;
}

/*************************************
 * DefaultDestroy
 *************************************/
///\cond INTERNAL
template<class T, class S>
void SHVDynArray<T,S>::DefaultDestroy(T* val)
{
	delete val;
}
///\endcond

/*************************************
 * GetCount
 *************************************/
template<class T, class S>
size_t SHVDynArray<T,S>::GetCount() const
{ return SHVDynArrayBase::GetCount(); }

/*************************************
 * GetArrayLen
 *************************************/
template<class T, class S>
size_t SHVDynArray<T,S>::GetArrayLen() const
{ return SHVDynArrayBase::GetArrayLen(); }

/*************************************
 * GetGrowSize
 *************************************/
template<class T, class S>
size_t SHVDynArray<T,S>::GetGrowSize() const
{ return SHVDynArrayBase::GetGrowSize(); }
template<class T, class S>

/*************************************
 * SetGrowSize
 *************************************/
void   SHVDynArray<T,S>::SetGrowSize(size_t s)
{ SHVDynArrayBase::SetGrowSize(s); }


/*************************************
 * ClearAndInit
 *************************************/
template<class T, class S>
void SHVDynArray<T,S>::ClearAndInit(int initSize)
{ SHVDynArrayBase::ClearAndInit(initSize,(SHVDynArrayBase::DestroyFunc)Destroy ); }

/*************************************
 * Resize
 *************************************/
template<class T, class S>
bool SHVDynArray<T,S>::Resize(int count)
{ return SHVDynArrayBase::Resize(count); }

/*************************************
 * index operator
 *************************************/
template<class T, class S>
T* SHVDynArray<T,S>::operator[](size_t index)
{ return (T*)(*(SHVDynArrayBase*)this)[index]; }

/*************************************
 * index operator
 *************************************/
template<class T, class S>
const T* SHVDynArray<T,S>::operator[](size_t index) const
{ return (const T*)(*(SHVDynArrayBase*)this)[index]; }

/*************************************
 * Add
 *************************************/
/// Add an item at the end of the array
/**
 \param item The item to be inserted
 \return Index of the inserted item
 *
 * The array will be expanded according to \e growSize if the array is full.
 */
template<class T, class S>
size_t SHVDynArray<T,S>::Add(T* item)
{ return SHVDynArrayBase::Add((void*)item); }

/*************************************
 * Remove
 *************************************/
template<class T, class S>
SHVPtrContainer<T> SHVDynArray<T,S>::Remove(size_t index)
{ return (T*)SHVDynArrayBase::Remove(index); }

/*************************************
 * Replace
 *************************************/
/// Replace an item in the array, and returns the old
/**
 \param index Index of the item to be replaced
 \param item Item to replace with
 \return The replaced item
 *
 * If the index is out of bounds the function will cause a memory
 * leak. You will need to check for bounds before calling this
 * function.
 */
template<class T, class S>
SHVPtrContainer<T> SHVDynArray<T,S>::Replace(size_t index, T* item)
{ return (T*)SHVDynArrayBase::Replace(index, (void*)item); }

/*************************************
 * Truncate
 *************************************/
template<class T, class S>
void SHVDynArray<T,S>::Truncate()
{ SHVDynArrayBase::Truncate(); }

/*************************************
 * Compress
 *************************************/
template<class T, class S>
void SHVDynArray<T,S>::Compress()
{ SHVDynArrayBase::Compress(); }

/*************************************
 * Sort
 *************************************/
template<class T, class S>
void SHVDynArray<T,S>::Sort(SortFunc func)
{ SHVDynArrayBase::Sort((SHVDynArrayBase::SortFunc)func); }

/*************************************
 * BubbleSort
 *************************************/
template<class T, class S>
void SHVDynArray<T,S>::BubbleSort(size_t index, SortFunc func)
{ SHVDynArrayBase::BubbleSort(index,(SHVDynArrayBase::SortFunc)func); }

/*************************************
 * Swap
 *************************************/
template<class T, class S>
bool SHVDynArray<T,S>::Swap(size_t index1, size_t index2)
{ return SHVDynArrayBase::Swap(index1,index2); }

/*************************************
 * FindMatchIndex
 *************************************/
/// Get the index of a given value
/**
 \param val Value to search for
 \param func Match function
 \return Index of the item found, or InvalidIndex
 */
template<class T, class S>
size_t SHVDynArray<T,S>::FindMatchIndex(S* val, MatchFunc func)
{ return SHVDynArrayBase::FindMatchIndex((void*)val, (SHVDynArrayBase::MatchFunc)func); }

/*************************************
 * FindMatch
 *************************************/
/// Get the item of a given value
/**
 \param val Value to search for
 \param func Match function
 \return The item found, or NULL
 */
template<class T, class S>
T* SHVDynArray<T,S>::FindMatch(S* val, MatchFunc func)
{ return (T*)SHVDynArrayBase::FindMatch((void*)val, (SHVDynArrayBase::MatchFunc)func); }

/*************************************
 * FindFirstMatchIndex
 *************************************/
/// Get the first index of a given value
/**
 \param val Value do search for
 \param func Match function
 \return Index of the item found, or InvalidIndex
 */
template<class T, class S>
size_t SHVDynArray<T,S>::FindFirstMatchIndex(S* val, MatchFunc func)
{ return SHVDynArrayBase::FindFirstMatchIndex((void*)val, (SHVDynArrayBase::MatchFunc)func); }

/*************************************
 * FindFirstMatch
 *************************************/
/// Get the first item of a given value
/**
 \param val Value to search for
 \param func Match function
 \return The item found, or NULL
 */
template<class T, class S>
T* SHVDynArray<T,S>::FindFirstMatch(S* val, MatchFunc func)
{ return (T*)SHVDynArrayBase::FindFirstMatch((void*)val, (SHVDynArrayBase::MatchFunc)func); }


/*************************************
 * ReleaseBuffer
 *************************************/
/// Releases contents from the array
/**
 * Returns a buffer object containing the contents of the array,
 * releasing them from the object. Use this to transfer contents
 * to a different array\n
\code
SHVDynArrayBuffer<int> GenerateList()
{
SHVVector<int> array;
	array.Add(new int(1));
	array.Add(new int(2));
	array.Add(new int(3));
	return array.ReleaseBuffer();
}

void main()
{
SHVDynArray<int> result(GenerateList());
	// result now contains the 3 objects added in the function
}
\endcode
 */
template<class T, class S>
SHVDynArrayBuffer<T,S> SHVDynArray<T,S>::ReleaseBuffer()
{
	return SHVDynArrayBuffer<T,S>(SHVDynArrayBase::ReleaseBuffer(),(SHVDynArrayBase::DestroyFunc)Destroy);
}

#endif
