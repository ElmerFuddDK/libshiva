#ifndef __SHIVA_UTILS_LISTBASE_H
#define __SHIVA_UTILS_LISTBASE_H

// forward declare
class SHVListNode;
typedef SHVListNode* SHVListPos;


//-=========================================================================================================
/// SHVListBase class (abstract)
/**
 * This class is intended as a base class for other double connected list classes. It implements
 * all the logic involved in managing the list.\n
 * This class is not meant to be used on its own. Use one of the derived classes, like SHVList.
 \note There's no validation on the position variables actually being for the current list.
 */

class SHVAPI SHVListBase
{
protected:


	SHVListBase();
	~SHVListBase();


	// List management
	void AddHead(SHVListNode* item);
	void AddTail(SHVListNode* item);
	void InsertBefore(SHVListPos current, SHVListNode* item);
	void InsertAfter(SHVListPos current, SHVListNode* item);

	SHVListNode* PopHead();
	SHVListNode* PopTail();


	// List iteration
	SHVListNode* GetFirst(SHVListPos& current) const;
	SHVListNode* GetFirst() const;
	SHVListNode* GetPrev(SHVListPos& current) const;
	SHVListNode* GetNext(SHVListPos& current) const;
	SHVListNode* GetAt(SHVListPos& current) const;
	SHVListNode* GetLast(SHVListPos& current) const;
	SHVListNode* GetLast() const;

	SHVListNode* indexer(int i) const;

public:


	// Public list iteration
	bool MoveFirst(SHVListPos& current) const;
	bool MovePrev(SHVListPos& current) const;
	bool MoveNext(SHVListPos& current) const;
	bool MoveLast(SHVListPos& current) const;

	inline SHVListPos GetHeadPosition();
	inline SHVListPos GetTailPosition();


	// Public list management
	void RemoveHead();
	void RemoveAt(SHVListPos& pos);
	void RemoveTail();
	void RemoveAll();

	long GetCount() const;


private:
	SHVListNode* First;
	SHVListNode* Last;
	long Nodes;
};


//-========================================================================================================
/// SHVListNode class
/**
 * Derive from this class when you create your own list class from SHVListBase. This class is used as
 * nodes in the internal list of SHVListBase.
 */

class SHVAPI SHVListNode
{
friend class SHVListBase;
protected:
	virtual ~SHVListNode();
public:
	inline SHVListNode();
private:
	///\cond INTERNAL
	SHVListNode* Prev;
	SHVListNode* Next;
	///\endcond
};


// ============================================ implementation ============================================ //

/*************************************
 * GetHeadPosition
 *************************************/
/// Returns position of first item in the list
/**
 \return Position of first item in the list
 */
SHVListPos SHVListBase::GetHeadPosition()
{ return First; }

/*************************************
 * GetTailPosition
 *************************************/
/// Returns position of last item in the list
/**
 \return Position of last item in the list
 */
SHVListPos SHVListBase::GetTailPosition()
{ return Last;  }

SHVListNode::SHVListNode()
{ Prev = Next = NULL; }

#endif