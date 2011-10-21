#ifndef __SHIVA_UTILS_VECTORREF_H
#define __SHIVA_UTILS_VECTORREF_H

#include "shvvectorbase.h"
#include "shvrefobject.h"

//-=========================================================================================================
/// SHVVectorRef template class
/**
 * Manages a simple array of refence counted objects of a given type
 */

template< class T, int GrowSize = 10 >
class SHVVectorRef : SHVVectorBase
{
public:


	// constructor
	SHVVectorRef();
	~SHVVectorRef();


	// properties
	inline size_t CalculateCount() const;
	inline bool IsEmpty();
	inline bool Eof(size_t index);


	// array management
	inline void Clear();

	inline T* operator[](size_t index);
	inline const T* operator[](size_t index) const;
	inline size_t Add(T* item);
	inline void Remove(size_t index);
	inline void Replace(size_t index, T* item);
	inline void Pop();

	inline void Truncate();
	inline void Compress();

private:
	///\cond INTERNAL
	static void DestroyData(void* data);
	///\endcond
};




// ============================================= implementation ============================================= //
/// \class SHVVectorRef shvvectorref.h "shiva/include/utils/shvvectorref.h"


/*************************************
 * Constructor
 *************************************/
/// Constructs the dynamic array
/**
 * The template value GrowSize indicates the amount of items the vector will
 * grow and shrink by.\n
 * The vector will contain pointers to objects of type T.
 */
template<class T, int GrowSize>
SHVVectorRef<T,GrowSize>::SHVVectorRef()
{}

/*************************************
 * Destructor
 *************************************/
template<class T, int GrowSize>
SHVVectorRef<T,GrowSize>::~SHVVectorRef()
{
	SHVVectorBase::Clear(DestroyData);
}


// properties
/*************************************
 * CalculateCount
 *************************************/
template<class T, int GrowSize>
size_t SHVVectorRef<T,GrowSize>::CalculateCount() const
{
	return SHVVectorBase::CalculateCount();
}

/*************************************
 * IsEmpty
 *************************************/
template<class T, int GrowSize>
bool SHVVectorRef<T,GrowSize>::IsEmpty()
{
	return SHVVectorBase::IsEmpty();
}

/*************************************
 * Eof
 *************************************/
template<class T, int GrowSize>
bool SHVVectorRef<T,GrowSize>::Eof(size_t index)
{
	return SHVVectorBase::Eof(index);
}


// array management
/*************************************
 * Clear
 *************************************/
template<class T, int GrowSize>
void SHVVectorRef<T,GrowSize>::Clear()
{
	SHVVectorBase::Clear(DestroyData);
}

/*************************************
 * index operator
 *************************************/
template<class T, int GrowSize>
T* SHVVectorRef<T,GrowSize>::operator[](size_t index)
{
	return (T*)(*(SHVVectorBase*)this)[index];
}
/*************************************
 * const index operator
 *************************************/
template<class T, int GrowSize>
const T* SHVVectorRef<T,GrowSize>::operator[](size_t index) const
{
	return (const T*)(*(SHVVectorBase*)this)[index];
}

/*************************************
 * Add
 *************************************/
template<class T, int GrowSize>
size_t SHVVectorRef<T,GrowSize>::Add(T* item)
{
	return SHVVectorBase::Add(item ? item->CreateRef() : NULL, GrowSize);
}

/*************************************
 * Remove
 *************************************/
template<class T, int GrowSize>
void SHVVectorRef<T,GrowSize>::Remove(size_t index)
{
	if ((*this)[index])
		((T*)SHVVectorBase::Remove(index))->DestroyRef();
}

/*************************************
 * Replace
 *************************************/
template<class T, int GrowSize>
void SHVVectorRef<T,GrowSize>::Replace(size_t index, T* item)
{
T* oldItem = (T*)SHVVectorBase::Replace(index,item ? item->CreateRef() : item);
	if (oldItem)
		oldItem->DestroyRef();
}

/*************************************
 * Pop
 *************************************/
template<class T, int GrowSize>
void SHVVectorRef<T,GrowSize>::Pop()
{
T* oldItem = (T*)SHVVectorBase::Pop(GrowSize);
	if (oldItem)
		oldItem->DestroyRef();
}

/*************************************
 * Truncate
 *************************************/
template<class T, int GrowSize>
void SHVVectorRef<T,GrowSize>::Truncate()
{
	SHVVectorBase::Truncate(GrowSize);
}

/*************************************
 * Compress
 *************************************/
template<class T, int GrowSize>
void SHVVectorRef<T,GrowSize>::Compress()
{
	SHVVectorBase::Compress(GrowSize);
}

///\cond INTERNAL
/*************************************
 * DestroyData
 *************************************/
template<class T, int GrowSize>
void SHVVectorRef<T,GrowSize>::DestroyData(void* data)
{
	((T*) data)->DestroyRef();
}
///\endcond

#endif
