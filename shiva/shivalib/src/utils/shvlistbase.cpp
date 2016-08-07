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
#include "../../../include/utils/shvlistbase.h"


// ========================================================================================================
//  SHVListBase class
// ========================================================================================================
/// \class SHVListBase shvlistbase.h "shiva/include/utils/shvlistbase.h"

/*************************************
 * Constructor
 *************************************/
SHVListBase::SHVListBase()
{
	Nodes = 0;
	First = Last = NULL;
}
SHVListBase::SHVListBase(const SHVListBufferBase& buffer)
{
SHVListBufferBase* unConst = (SHVListBufferBase*)&buffer;
	First = unConst->First;
	Last = unConst->Last;
	Nodes = unConst->Nodes;
	unConst->First = unConst->Last = NULL;
	unConst->Nodes = 0;
}

/*************************************
 * Destructor
 *************************************/
SHVListBase::~SHVListBase()
{
	RemoveAll();
}

/*************************************
 * Operator=
 *************************************/
SHVListBase& SHVListBase::operator=(const SHVListBufferBase& buffer)
{
SHVListBufferBase* unConst = (SHVListBufferBase*)&buffer;

	RemoveAll();
	
	First = unConst->First;
	Last = unConst->Last;
	Nodes = unConst->Nodes;
	unConst->First = unConst->Last = NULL;
	unConst->Nodes = 0;
	
	return *this;
}

/*************************************
 * Operator+=
 *************************************/
SHVListBase& SHVListBase::operator+=(const SHVListBufferBase& buffer)
{

	if (buffer.Nodes)
	{
	SHVListBufferBase* unConst = (SHVListBufferBase*)&buffer;
	
		if (Nodes)
		{
			Last->Next = unConst->First;
			unConst->First->Prev = Last;
			Last = unConst->Last;
			
			Nodes += unConst->Nodes;
		}
		else
		{
			First = unConst->First;
			Last = unConst->Last;
			Nodes = unConst->Nodes;
		}
		
		unConst->First = unConst->Last = NULL;
		unConst->Nodes = 0;
	}
	
	return *this;
}

/*************************************
 * AddHead
 *************************************/
/// Adds an item at the start of the list
/**
 \param newNode Node to be inserted
 */
void SHVListBase::AddHead(SHVListNode* newNode)
{
	newNode->Next = First;
	newNode->Prev = NULL;
	
	if (First != NULL)
		First->Prev   = newNode;
	First = newNode;
	
	if (Last == NULL) Last = newNode;
	
	Nodes++;
}

/*************************************
 * AddTail
 *************************************/
/// Adds an item at the end of the list
/**
 \param newNode Node to be inserted
 */
void SHVListBase::AddTail(SHVListNode* newNode)
{
	newNode->Next = NULL;
	newNode->Prev = Last;
	
	if (Last != NULL)
		Last->Next = newNode;
	Last = newNode;

	if (First == NULL) First = newNode;

	Nodes++;
}

/*************************************
 * InsertBefore
 *************************************/
/// Inserts before current position
/**
 \param current Position to be inserted before
 \param newNode Node to be inserted
 */
void SHVListBase::InsertBefore(SHVListPos current, SHVListNode* newNode)
{
	if (!current)
		AddHead(newNode);
	else
	{
		newNode->Next = current;
		newNode->Prev = current->Prev;

		if (current->Prev)
			current->Prev->Next = newNode;
		current->Prev = newNode;

		if (First == current)
			First = newNode;

		Nodes++;
	}
}

/*************************************
 * InsertAfter
 *************************************/
/// Inserts after current position
/**
 \param current Position to be inserted after
 \param newNode Node to be inserted
 */
void SHVListBase::InsertAfter(SHVListPos current, SHVListNode* newNode)
{
	if (!current)
		AddTail(newNode);
	else
	{
		newNode->Next = current->Next;
		newNode->Prev = current;

		if (current->Next)
			current->Next->Prev = newNode;
		current->Next = newNode;

		if (Last == current)
			Last = newNode;

		Nodes++;
	}
}


/*************************************
 * PopHead
 *************************************/
/// Removes the first item and returns it
/**
 \return The first item
 */
SHVListNode* SHVListBase::PopHead()
{
	if (First == NULL) return NULL;
SHVListNode* retVal = First;
SHVListNode* tmpNode = First->Next;
	First = tmpNode;
	if (First == NULL) Last = NULL;
	else First->Prev = NULL;

	Nodes--;
	return retVal;
}

/*************************************
 * PopTail
 *************************************/
/// Removes the last item and returns it
/**
 \return The last item
 */
SHVListNode* SHVListBase::PopTail()
{
	if (Last == NULL) return NULL;
SHVListNode* retVal = Last;
SHVListNode* tmpNode = Last->Prev;
	Last = tmpNode;
	if (Last == NULL) First = NULL;
	else Last->Next = NULL;

	Nodes--;
	return retVal;
}


/*************************************
 * GetFirst
 *************************************/
/// Returns the first item, and sets position to it
/**
 \param current Will be set to the first item
 \return The first item
 *
 * This function will return NULL if there are no items in the
 * list, and will set current to NULL in that case.
 */
SHVListNode* SHVListBase::GetFirst(SHVListPos& current) const
{
	current = First;
	return First;
}
/// Returns the first item
/**
 \return The first item
 *
 * This function will return NULL if there are no items in the
 * list.
 */
SHVListNode* SHVListBase::GetFirst() const
{
	return First;
}

/*************************************
 * GetPrev
 *************************************/
/// Returns previous item in list, and sets position to it
/**
 \param current Position from where to move backward
 \return The previous item
 *
 * Will return NULL if the current item is the first, in which
 * case current will be set to NULL as well.\n
 * If current is NULL it will behave as GetLast.
 */
SHVListNode* SHVListBase::GetPrev(SHVListPos& current) const
{
	if (current != NULL) current = current->Prev;
	else current = Last;
	return current;
}

/*************************************
 * GetNext
 *************************************/
/// Returns next item in list, and sets position to it
/**
 \param current Position from where to move forward
 \return The next item
 *
 * Will return NULL if the current item is the last, in which
 * case current will be set to NULL as well.
 * If current is NULL it will behave as GetFirst.
 */
SHVListNode* SHVListBase::GetNext(SHVListPos& current) const
{
	if (current != NULL) current = current->Next;
	else current = First;
	return current;
}

/*************************************
 * GetAt
 *************************************/
/// Returns the item at the given position
/**
 \param current Position from where to return the item
 \return The item
 */
SHVListNode* SHVListBase::GetAt(const SHVListPos current) const
{
	return (SHVListNode*)current;
}

/*************************************
 * GetLast
 *************************************/
/// Returns the last item, and sets position to it
/**
 \param current Will be set to the last item
 \return The last item
 *
 * This function will return NULL if there are no items in the
 * list, and will set current to NULL in that case.
 */
SHVListNode* SHVListBase::GetLast(SHVListPos& current) const
{
	current = Last;
	return Last;
}
/// Returns the last item
/**
 \return The last item
 *
 * This function will return NULL if there are no items in the
 * list.
 */
SHVListNode* SHVListBase::GetLast() const
{
	return Last;
}

/*************************************
 * indexer
 *************************************/
/// Indexing function intended for [] operators
/**
 \param i Index
 \return The item at index i, or NULL
 */
SHVListNode* SHVListBase::indexer(int i) const
{
SHVListPos retVal;

	for (GetFirst(retVal);i>0 && retVal;i--) GetNext(retVal);

	return retVal;
}

/*************************************
 * ReleaseBuffer
 *************************************/
SHVListBufferBase SHVListBase::ReleaseBuffer()
{
SHVListBufferBase retVal;
	retVal.First = First;
	retVal.Last = Last;
	retVal.Nodes = Nodes;
	
	First = Last = NULL;
	Nodes = 0;
	
	return retVal;
}

// public:

/*************************************
 * MoveFirst
 *************************************/
/// Sets position to the first item
/**
 \param current Position to be set
 \return Success - true if there is a first
 */
bool SHVListBase::MoveFirst(SHVListPos& current) const
{
	current = First;
	return current != NULL;
}

/*************************************
 * MovePrev
 *************************************/
/// Sets position to the previous item
/**
 \param current Position to be set
 \return Success - true if there is a previous
 */
bool SHVListBase::MovePrev(SHVListPos& current) const
{
	if (current != NULL) current = current->Prev;
	else current = Last;
	return current != NULL;
}

/*************************************
 * MoveNext
 *************************************/
/// Sets position to the next item
/**
 \param current Position to be set
 \return Success - true if there is a next
 */
bool SHVListBase::MoveNext(SHVListPos& current) const
{
	if (current != NULL) current = current->Next;
	else current = First;
	return current != NULL;
}

/*************************************
 * MoveLast
 *************************************/
/// Sets position to the last item
/**
 \param current Position to be set
 \return Success - true if there is a last
 */
bool SHVListBase::MoveLast(SHVListPos& current) const
{
	current = Last;
	return current != NULL;
}

/*************************************
 * RemoveHead
 *************************************/
/// Removes the first item in the list and deletes it
void SHVListBase::RemoveHead()
{
	if (First == NULL) return;
SHVListNode* tmpNode = First->Next;
	delete First;
	First = tmpNode;
	if (First == NULL) Last = NULL;
	else First->Prev = NULL;

	Nodes--;
}

/*************************************
 * RemoveAt
 *************************************/
/// Removes the item at a given position in the list and deletes it
/**
 \param pos Position from where to remove
 \note Sets pos to the next one in the list, or NULL if the last
 */
void SHVListBase::RemoveAt(SHVListPos& pos)
{
	if (pos == NULL) return;
SHVListNode* current = pos;
SHVListNode* retVal = pos->Next;
	if (pos == First)
	{
		First = retVal;
		if (First == NULL) Last = NULL;
		else First->Prev = NULL;
	}
	else if (pos == Last)
	{
		Last = pos->Prev;
		if (Last == NULL) First = NULL;
		else Last->Next = NULL;
	}
	else
	{
		pos->Prev->Next = pos->Next;
		pos->Next->Prev = pos->Prev;
	}
	delete current;
	pos = retVal;

	Nodes--;
}

/*************************************
 * RemoveTail
 *************************************/
/// Removes the last item in the list and deletes it
void SHVListBase::RemoveTail()
{
	if (Last == NULL) return;
SHVListNode* tmpNode = Last->Prev;
	delete Last;
	Last = tmpNode;
	if (Last == NULL) First = NULL;
	else Last->Next = NULL;

	Nodes--;
}

/*************************************
 * RemoveAll
 *************************************/
/// Removes the all items in the list and deletes them
void SHVListBase::RemoveAll()
{
SHVListNode* p;
	for (SHVListNode* n = First; n != NULL;)
	{
		p = n; n = n->Next; delete p;
	}
	First = NULL;
	Last  = NULL;
	Nodes = 0;
}

/*************************************
 * GetCount
 *************************************/
/// Get amount of items in the list
/**
 \return Amount of items in the list
 */
long SHVListBase::GetCount() const
{
	return Nodes;
}


// ========================================================================================================
//  SHVListNode class
// ========================================================================================================

/*************************************
 * Destructor
 *************************************/
SHVListNode::~SHVListNode()
{}

///\cond INTERNAL
// ========================================================================================================
//  SHVListBufferBase class
// ========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVListBufferBase::SHVListBufferBase(const SHVListBufferBase& buffer)
{
SHVListBufferBase* unConst = (SHVListBufferBase*)&buffer;
	First = unConst->First;
	Last = unConst->Last;
	Nodes = unConst->Nodes;
	unConst->First = unConst->Last = NULL;
	unConst->Nodes = 0;
}

/*************************************
 * Destructor
 *************************************/
SHVListBufferBase::~SHVListBufferBase()
{
SHVListNode* p;
	for (SHVListNode* n = First; n != NULL;)
	{
		p = n; n = n->Next; delete p;
	}
}
///\endcond
