#ifndef __SHIVA_UTILS_DYNARRAYREF_H
#define __SHIVA_UTILS_DYNARRAYREF_H

#include "shvrefobject.h"
#include "shvdynarraybase.h"


// ========================================================================================================
///  SHVDynArrayRef class - containing reference counting pointers
/**
 * Use this class when you want a dynamic array that contains objects based on SHVRefObject.
 */

template< class T >
class SHVDynArrayRef : protected SHVDynArrayBase
{
public:


	SHVDynArrayRef(int growSize = 50, int initSize=0, bool zeroed=false);
	~SHVDynArrayRef();


	// properties
	size_t GetCount();
	size_t GetArrayLen();
	size_t GetGrowSize();
	void   SetGrowSize(size_t s);


	// array management
	void ClearAndInit(int initSize=0);
	bool Resize(int count);

	T* operator[](size_t index);
	size_t Add(T* item);
	bool   InsertAt(size_t index, T* item); ///< only works if zeroed and if the index is NULL  .. for now
	SHVRefObjectContainer<T> Remove(size_t index);
	SHVRefObjectContainer<T> Replace(size_t index, T* item);

	void Truncate();
	void Compress();

private:
	static void Destroy(void* val);
};


// ============================================= implementation ============================================= //


/*************************************
 * Constructor
 *************************************/
template< class T >
SHVDynArrayRef<T>::SHVDynArrayRef(int growSize, int initSize, bool zeroed) : SHVDynArrayBase(growSize,initSize,zeroed)
{}

/*************************************
 * Destructor
 *************************************/
template< class T >
SHVDynArrayRef<T>::~SHVDynArrayRef()
{
	SHVDynArrayBase::ClearAndInit(0,&SHVDynArrayRef<T>::Destroy);
}

/*************************************
 * GetCount
 *************************************/
template< class T >
size_t SHVDynArrayRef<T>::GetCount()
{
	return SHVDynArrayBase::GetCount();
}

/*************************************
 * GetArrayLen
 *************************************/
template< class T >
size_t SHVDynArrayRef<T>::GetArrayLen()
{
	return SHVDynArrayBase::GetArrayLen();
}

/*************************************
 * GetGrowSize
 *************************************/
template< class T >
size_t SHVDynArrayRef<T>::GetGrowSize()
{
	return SHVDynArrayBase::GetGrowSize();
}

/*************************************
 * SetGrowSize
 *************************************/
template< class T >
void  SHVDynArrayRef<T>::SetGrowSize(size_t s)
{
	SHVDynArrayBase::SetGrowSize(s);
}

/*************************************
 * ClearAndInit
 *************************************/
template< class T >
void SHVDynArrayRef<T>::ClearAndInit(int initSize)
{
	SHVDynArrayBase::ClearAndInit(initSize, Destroy);
}

/*************************************
 * Resize
 *************************************/
template< class T >
bool SHVDynArrayRef<T>::Resize(int count)
{
	return SHVDynArrayBase::Resize(count);
}

/*************************************
 * index operator
 *************************************/
template< class T >
T* SHVDynArrayRef<T>::operator[](size_t index)
{
	return (T*)(*(SHVDynArrayBase*)this)[index];
}

/*************************************
 * Add
 *************************************/
template< class T >
size_t SHVDynArrayRef<T>::Add(T* item)
{
	return SHVDynArrayBase::Add((void*)item->CreateRef());
}

/*************************************
 * InsertAt
 *************************************/
template< class T >
bool SHVDynArrayRef<T>::InsertAt(size_t index, T* item)
{
bool retVal = (T*)SHVDynArrayBase::InsertAt(index,item);

	if (retVal)
		item->CreateRef();

	return retVal;
}

/*************************************
 * Remove
 *************************************/
template< class T >
SHVRefObjectContainer<T> SHVDynArrayRef<T>::Remove(size_t index)
{
T* retVal = SHVDynArrayBase::Remove(index);

	if (retVal)
		retVal->ReleaseRef();

	return retVal;
}

/*************************************
 * Replace
 *************************************/
template< class T >
SHVRefObjectContainer<T> SHVDynArrayRef<T>::Replace(size_t index, T* item)
{
T* retVal = (T*)SHVDynArrayBase::Replace(index,(void*)item->CreateRef());

	if (retVal)
		retVal->ReleaseRef();

	return retVal;
}

/*************************************
 * Truncate
 *************************************/
template< class T >
void SHVDynArrayRef<T>::Truncate()
{
	SHVDynArrayBase::Truncate(); 
}

/*************************************
 * Compress
 *************************************/
template< class T >
void SHVDynArrayRef<T>::Compress()
{
	SHVDynArrayBase::Compress();
}

/*************************************
 * Destroy
 *************************************/
template< class T >
void SHVDynArrayRef<T>::Destroy(void* val)
{
	((T*)val)->DestroyRef();
}

#endif
