#ifndef __SHIVA_UTILS_LIST_H
#define __SHIVA_UTILS_LIST_H

#include "shvlistbase.h"
#include "shvptrcontainer.h"


//-=========================================================================================================
/// SHVList class template
/**
 * A double connected list class. It is thread safe for iterations, but not for insertion.
 */

template<class T, class S = T>
class SHVList : public SHVListBase
{
public:
	SHVList();

	void AddHead(S item);
	void AddTail(S item);
	void InsertBefore(SHVListPos current, S item);
	void InsertAfter(SHVListPos current, S item);


	T PopHead();
	T PopTail();

	T& GetFirst(SHVListPos& current) const;
	T& GetFirst() const;
	T& GetPrev(SHVListPos& current) const;
	T& GetNext(SHVListPos& current) const;
	T& GetAt(const SHVListPos current) const;
	T& GetLast(SHVListPos& current) const;
	T& GetLast() const;

	SHVListPos Find(S item) const;
	SHVListPos FindIndex(int index) const;

};


//-========================================================================================================
/// SHVListIterator class template - iterator for SHVList
//-========================================================================================================
/**
 * This class can be used for iteration through SHVList
\code
SHVList<int> list;
SHVListIterator<int> i(list);

    // insert items
    list.AddTail(1);
    list.AddTail(2);

    // iteration
    while (i.MoveNext())
        printf("value: %d\n" i.Get());

    // delete items - advanced usage
    for(i.MoveNext(); i;)
    {
        if (i.Get() == 1)
            list.RemoveAt(i.Pos()); // will move iterator to next position
        else
            i.MoveNext();
    }
\endcode
 */

template<class T, class S = T>
class SHVListIterator
{
	const SHVList<T,S>& List;
	SHVListPos pos;
public:
	inline SHVListIterator(const SHVList<T,S>& list);
	inline bool MoveNext();
	inline operator bool();
	inline T& Get();
	inline SHVListPos& Pos();
};




// ============================================ implementation ============================================ //
/// \class SHVList shvlist.h "shiva/include/utils/shvlist.h"
/// \class SHVListIterator shvlist.h "shiva/include/utils/shvlist.h"

///\cond INTERNAL
template<class T, class S = T>
class SHVListNodeTemp : public SHVListNode
{
friend class SHVList<T,S>;
friend class SHVListIterator<T,S>;
	T Data;
	SHVListNodeTemp(S data) : Data(data) {}
	inline operator T*() { return &Data; }
};
///\endcond



/*************************************
 * Constructor
 *************************************/
template<class T, class S>
SHVList<T,S>::SHVList() {}

/*************************************
 * AddHead
 *************************************/
template<class T, class S>
void SHVList<T,S>::AddHead(S item) { SHVListBase::AddHead( new SHVListNodeTemp<T,S>(item) ); }

/*************************************
 * AddTail
 *************************************/
template<class T, class S>
void SHVList<T,S>::AddTail(S item) { SHVListBase::AddTail( new SHVListNodeTemp<T,S>(item) ); }

/*************************************
 * InsertBefore
 *************************************/
template<class T, class S>
void SHVList<T,S>::InsertBefore(SHVListPos current, S item) { SHVListBase::InsertBefore( current, new SHVListNodeTemp<T,S>(item) ); }

/*************************************
 * InsertAfter
 *************************************/
template<class T, class S>
void SHVList<T,S>::InsertAfter(SHVListPos current, S item)  { SHVListBase::InsertAfter ( current, new SHVListNodeTemp<T,S>(item) ); }


/*************************************
 * PopHead
 *************************************/
template<class T, class S>
T SHVList<T,S>::PopHead()
{
SHVPtrContainer< SHVListNodeTemp<T,S> > data = (SHVListNodeTemp<T,S>*)SHVListBase::PopHead();
	return data->Data;
}

/*************************************
 * PopTail
 *************************************/
template<class T, class S>
T SHVList<T,S>::PopTail()
{
SHVPtrContainer< SHVListNodeTemp<T,S> > data = (SHVListNodeTemp<T,S>*)SHVListBase::PopTail();
	return data->Data;
}

/*************************************
 * Get*
 *************************************/
template<class T, class S>
T& SHVList<T,S>::GetFirst(SHVListPos& current) const { return ((SHVListNodeTemp<T,S>*)SHVListBase::GetFirst(current))->Data; }
template<class T, class S>
T& SHVList<T,S>::GetFirst() const { return ((SHVListNodeTemp<T,S>*)SHVListBase::GetFirst())->Data; }
template<class T, class S>
T& SHVList<T,S>::GetPrev(SHVListPos& current) const  { return ((SHVListNodeTemp<T,S>*)SHVListBase::GetPrev(current))->Data;  }
template<class T, class S>
T& SHVList<T,S>::GetNext(SHVListPos& current) const  { return ((SHVListNodeTemp<T,S>*)SHVListBase::GetNext(current))->Data;  }
template<class T, class S>
T& SHVList<T,S>::GetAt(const SHVListPos current) const { return ((SHVListNodeTemp<T,S>*)SHVListBase::GetAt(current))->Data;  }
template<class T, class S>
T& SHVList<T,S>::GetLast(SHVListPos& current) const  { return ((SHVListNodeTemp<T,S>*)SHVListBase::GetLast(current))->Data;  }
template<class T, class S>
T& SHVList<T,S>::GetLast() const { return ((SHVListNodeTemp<T,S>*)SHVListBase::GetLast())->Data;  }

/*************************************
 * Find
 *************************************/
/// Search the list for an item
/**
 \param item Item to search for
 \return Position of item, or NULL
 */
template<class T, class S>
SHVListPos SHVList<T,S>::Find(S item) const
{
SHVListIterator<T,S> retVal(*this);

	while (retVal.MoveNext())
	{
		if (retVal.Get() == item) break;
	}

	return retVal.Pos();
}

/*************************************
 * FindIndex
 *************************************/
/// Resolve position of a given index
template<class T, class S>
SHVListPos SHVList<T,S>::FindIndex(int index) const
{
SHVListIterator<T,S> retVal(*this);


	for(retVal.MoveNext();index>0 && retVal;index--) retVal.MoveNext();

	return retVal.Pos();
}

/*************************************
 * Iterator::MoveNext
 *************************************/
template<class T, class S>
SHVListIterator<T,S>::SHVListIterator(const SHVList<T,S>& list) : List(list) { pos = NULL; }

/*************************************
 * Iterator::MoveNext
 *************************************/
/// Moves the iterator to the next position
/**
 \return True if there was a next
 */
template<class T, class S>
bool SHVListIterator<T,S>::MoveNext() { return List.MoveNext(pos); }

/*************************************
 * Iterator::Bool
 *************************************/
/// State of iterator - true if position is valid
/**
 \return Valid state of iterator
 */
template<class T, class S>
SHVListIterator<T,S>::operator bool() { return pos != NULL; }

/*************************************
 * Iterator::Get
 *************************************/
/// Returns current item pointed to
/**
 \return Current item
 */
template<class T, class S>
T& SHVListIterator<T,S>::Get() { return List.GetAt(pos); }

/*************************************
 * Iterator::Pos
 *************************************/
/// Reference to internal iterator position
/**
 \return Iterator position
 */
template<class T, class S>
SHVListPos& SHVListIterator<T,S>::Pos() { return pos; }

#endif
