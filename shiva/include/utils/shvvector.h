#ifndef __SHIVA_UTILS_VECTOR_H
#define __SHIVA_UTILS_VECTOR_H

#include "shvvectorbase.h"
#include "shvptrcontainer.h"

//-=========================================================================================================
/// SHVVector template class
/**
 * Manages a simple array of pointers of a given type
 */

template< class T, int GrowSize = 10 >
class SHVVector : SHVVectorBase
{
public:


	// constructor
	SHVVector();
	~SHVVector();


	// properties
	inline size_t CalculateCount() const;
	inline bool IsEmpty();
	inline bool Eof(size_t index);


	// array management
	inline void Clear();

	inline T* operator[](size_t index);
	inline const T* operator[](size_t index) const;
	inline size_t Add(T* item);
	inline SHVPtrContainer<T> Remove(size_t index);
	inline SHVPtrContainer<T> Replace(size_t index, T* item);
	inline SHVPtrContainer<T> Pop();
	inline T* Get(size_t index);

	inline void Truncate();
	inline void Compress();

private:
	///\cond INTERNAL
	static void DestroyData(void* data);
	///\endcond
};




// ============================================= implementation ============================================= //
/// \class SHVVector shvvector.h "shiva/include/utils/shvvector.h"


/*************************************
 * Constructor
 *************************************/
/// Constructs the vector
/**
 * The template value GrowSize indicates the amount of items the vector will
 * grow and shrink by.\n
 * The vector will contain pointers to objects of type T.
 */
template<class T, int GrowSize>
SHVVector<T,GrowSize>::SHVVector()
{}

/*************************************
 * Destructor
 *************************************/
template<class T, int GrowSize>
SHVVector<T,GrowSize>::~SHVVector()
{
	SHVVectorBase::Clear(DestroyData);
}


// properties
/*************************************
 * CalculateCount
 *************************************/
template<class T, int GrowSize>
size_t SHVVector<T,GrowSize>::CalculateCount() const
{
	return SHVVectorBase::CalculateCount();
}

/*************************************
 * IsEmpty
 *************************************/
template<class T, int GrowSize>
bool SHVVector<T,GrowSize>::IsEmpty()
{
	return SHVVectorBase::IsEmpty();
}

/*************************************
 * Eof
 *************************************/
template<class T, int GrowSize>
bool SHVVector<T,GrowSize>::Eof(size_t index)
{
	return SHVVectorBase::Eof(index);
}


// array management
/*************************************
 * Clear
 *************************************/
template<class T, int GrowSize>
void SHVVector<T,GrowSize>::Clear()
{
	SHVVectorBase::Clear(DestroyData);
}

/*************************************
 * index operator
 *************************************/
template<class T, int GrowSize>
T* SHVVector<T,GrowSize>::operator[](size_t index)
{
	return (T*)(*(SHVVectorBase*)this)[index];
}

/*************************************
 * const index operator
 *************************************/
template<class T, int GrowSize>
const T* SHVVector<T,GrowSize>::operator[](size_t index) const
{
	return (const T*)(*(SHVVectorBase*)this)[index];
}

/*************************************
 * Add
 *************************************/
template<class T, int GrowSize>
size_t SHVVector<T,GrowSize>::Add(T* item)
{
	return SHVVectorBase::Add(item, GrowSize);
}

/*************************************
 * Remove
 *************************************/
template<class T, int GrowSize>
SHVPtrContainer<T> SHVVector<T,GrowSize>::Remove(size_t index)
{
	return (T*)SHVVectorBase::Remove(index);
}

/*************************************
 * Replace
 *************************************/
template<class T, int GrowSize>
SHVPtrContainer<T> SHVVector<T,GrowSize>::Replace(size_t index, T* item)
{
	return (T*)SHVVectorBase::Replace(index,item);
}

/*************************************
 * Pop
 *************************************/
template<class T, int GrowSize>
SHVPtrContainer<T> SHVVector<T,GrowSize>::Pop()
{
	return (T*)SHVVectorBase::Pop(GrowSize);
}

/*************************************
 * Get
 *************************************/
template<class T, int GrowSize>
T* SHVVector<T,GrowSize>::Get(size_t index)
{
	return (T*)SHVVectorBase::Get(index);
}

/*************************************
 * Truncate
 *************************************/
template<class T, int GrowSize>
void SHVVector<T,GrowSize>::Truncate()
{
	SHVVectorBase::Truncate(GrowSize);
}

/*************************************
 * Compress
 *************************************/
template<class T, int GrowSize>
void SHVVector<T,GrowSize>::Compress()
{
	SHVVectorBase::Compress(GrowSize);
}

///\cond INTERNAL
/*************************************
 * DestroyData
 *************************************/
template<class T, int GrowSize>
void SHVVector<T,GrowSize>::DestroyData(void* data)
{
	delete (T*)data;
}
///\endcond

#endif
