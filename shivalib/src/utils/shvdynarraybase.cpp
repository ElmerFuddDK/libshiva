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
#include "../../../include/utils/shvdynarraybase.h"

#include <stdlib.h>
#include <string.h>

//=========================================================================================================
// SHVDynArrayBase class - A basic dynamic array containing void pointers
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
/// Constructs the dynamic array
/**
 \param growSize The number of items to expand the array with - must be positive
 \param initSize The initial size of the array (optional)
 \param zeroed Determins whether the array will initially have all items set to NULL (optional)
 */
SHVDynArrayBase::SHVDynArrayBase(int growSize, int initSize, bool zeroed)
{
	Items = ArrayLen = 0;
	Array = NULL;
	GrowSize = (growSize > 0 ? growSize : 10);
	Zeroed = zeroed;

	ClearAndInit(initSize);
}

/*************************************
 * Destructor
 *************************************/
SHVDynArrayBase::~SHVDynArrayBase()
{
	ClearAndInit(0);
}

/*************************************
 * ClearAndInit
 *************************************/
/// Deletes the array and all the items, and creates a new one
/**
 \param initSize Size of the new array
 \param func The function to be used to destroy any items in the array (optional)
 *
 \note If func is not provided then the items will not be deleted
 */
void SHVDynArrayBase::ClearAndInit(int initSize,DestroyFunc func)
{
	if (func)
	{
		for(size_t i=0; i<Items; i++)
		{
			if (Array[i])
				func(Array[i]);
		}
	}

	if (Array)
	{
		free(Array);
		Array = NULL;
	}

	Items = ArrayLen = 0;
	
	Resize(initSize);
}

/*************************************
 * Resize
 *************************************/
/// Resizes the array by a specified amount
/**
 \param count The amount of items to expand the array with - can be negative
 \return Success
 *
 * The array will never be resized to an amount smaller than the last item in the
 * array. If you want to resize the array to the smallest possible amount you can
 * do the following:
 \code
SHVDynArray array(5);
char* test = "test";

	array.Add(NULL);
	array.Add(test);

	array.Compress(); // will move index 1 to index 0, and set length to 1
	array.Resize(-array.GetArrayLen()); // will always resize to the smallest amount
 \endcode
 */
bool SHVDynArrayBase::Resize(int count)
{
bool retVal = ( count>0 || ( (size_t)(-count) < ArrayLen ) );

	if (retVal)
	{
	size_t newLen = ArrayLen+count;

		if (newLen < Items)
			newLen = Items;

		if (newLen != ArrayLen)
		{
		void** oldArray = Array;

			Array = (void**)::malloc(sizeof(void*)*newLen);
			
			if (oldArray)
			{
				if (Items)
					::memcpy(Array,oldArray,sizeof(void*)*Items);
				
				::free(oldArray);
			}

			if (Zeroed && newLen > ArrayLen)
			{
				::memset(Array+Items, 0, (newLen-Items)*sizeof(void*));
			}

			ArrayLen = newLen;
		}
	}

	return retVal;
}

/*************************************
 * index operator
 *************************************/
/// Index operator
/**
 \note This operator cannot be used to insert or replace items in the array
 */
void* SHVDynArrayBase::operator[](size_t index)
{
	if (Zeroed && index >= Items && index < ArrayLen)
		Items = index+1;
	return ( index < Items ? Array[index] : NULL );
}

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
size_t SHVDynArrayBase::Add(void* item)
{
	if (Zeroed)
	{
	size_t retVal = SIZE_T_MAX;
		for (size_t i=0; i < ArrayLen && retVal > ArrayLen; i++)
		{
			if (!Array[i])
			{
				Array[i] = item;
				retVal = i;
			}
		}
		
		if (retVal > ArrayLen) // no more free items, lets expand
		{
			Resize(GrowSize);
			retVal = Add(item);
		}
		if (retVal >= Items)
			Items = retVal+1;

		return retVal;
	}
	else
	{
		if (Items >= ArrayLen)
		Resize(GrowSize);
	
		Array[Items]=item;
		Items++;
	}
	return Items-1;
}

/*************************************
 * InsertAt
 *************************************/
/// Insert the given item at the given position
/**
 \param index Position to insert into
 \param item Item to be inserted
 *
 \note This function only works on zeroed arrays, and if the position is NULL
 */
bool SHVDynArrayBase::InsertAt(size_t index, void* item)
{
bool retVal = (Zeroed && index < ArrayLen && !Array[index]);

	if (retVal)
	{
		Array[index] = item;
	}

	return retVal;
}

/*************************************
 * Remove
 *************************************/
/// Removes an item from the array, and returns it
/**
 \param index Index of the item to remove
 \return Returns the item
 *
 * If the index is out of bounds the function will return NULL.\n
 * The function will set the given index in the array to NULL.
 */
void* SHVDynArrayBase::Remove(size_t index)
{
void* retVal;

	if (index < Items)
	{
		retVal = Array[index];

		if (Zeroed)
		{
			Array[index] = NULL;
		}
		else
		{
			for (size_t i=index; i < Items - 1; i++)
			{
				Array[i] = Array[i+1];
			}
			Items--;
		}
	}
	else retVal = NULL;

	return retVal;
}

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
void* SHVDynArrayBase::Replace(size_t index, void* item)
{
void* retVal;

	if (index < Items)
	{
		retVal = Array[index];
		Array[index] = item;
	}
	else
	{
		retVal = NULL;
		Add(item);
	}

	return retVal;
}

/*************************************
 * Truncate
 *************************************/
/// Truncates the item count
void SHVDynArrayBase::Truncate()
{
	while (Items && !Array[Items-1]) Items--;
}

/*************************************
 * Compress
 *************************************/
/// Compresses the list and truncates
void SHVDynArrayBase::Compress()
{
	Truncate();

	for (size_t i=0; i<Items; i++)
	{
		if (!Array[i])
		{
			Array[i] = Array[Items-1];
			Array[Items-1] = NULL;
			Truncate();
		}
	}
}

/*************************************
 * Sort
 *************************************/
/// QSorts the array accorting to the supplied sorting function
/**
 \param func Sorting function
 */
void SHVDynArrayBase::Sort(SHVDynArrayBase::SortFunc func)
{
	if (Array && Items)
		::qsort(Array,Items,sizeof(void*),(int (*)(const void*,const void*))func);
}

/*************************************
 * BubbleSort
 *************************************/
/// Bubble sorts a given array index
/**
 \param index Index to sort
 \param func Sorting function
 */
void SHVDynArrayBase::BubbleSort(size_t index, SHVDynArrayBase::SortFunc func)
{
	if (index < Items)
	{
		if (index>0 && func(&Array[index-1],&Array[index])>0)
		{
			do
			{
				Swap(index-1,index);
				index--;
			}
			while (index>0 && func(&Array[index-1],&Array[index])>0);
		}
		else if (index<Items-1 && func(&Array[index],&Array[index+1])>0)
		{
			do
			{
				Swap(index,index+1);
				index++;
			}
			while (index<Items-1 && func(&Array[index],&Array[index+1])>0);
		}
	}
}

/*************************************
 * Swap
 *************************************/
/// Swaps the contents of 2 indexes
/**
 \param index1 First index
 \param index2 Second index
 \return Success
 */
bool SHVDynArrayBase::Swap(size_t index1, size_t index2)
{
void* temp;
	if (index1 >= Items || index2 >= Items)
		return false;

	temp = Array[index1];
	Array[index1] = Array[index2];
	Array[index2] = temp;

	return true;
}

/*************************************
 * FindMatchIndex
 *************************************/
/// Get the index of a given value
/**
 \param val Value to search for
 \param func Match function
 \return Index of the item found, or InvalidIndex
 */
size_t SHVDynArrayBase::FindMatchIndex(void* val, SHVDynArrayBase::MatchFunc func)
{
size_t index = Items/2;
size_t jump  = index;
int comp;
size_t retVal = InvalidIndex;
bool exitFlag = false;

	if (!Items)
		return retVal;

	while (retVal == InvalidIndex)
	{
		jump = (jump/2) + (jump%2);

		comp = func(&val,&Array[index]);
		if (comp<0)
			index -= ( index > jump ? jump : index );
		else if (comp>0)
			index += jump;
		else
			retVal = index;

		if (jump==0)
			break;
		else if (exitFlag)
			jump = 0;
		else if(jump == 1)
			exitFlag = true;

		if (index >= Items)
			index = Items-1;
	}

	return retVal;
}

/*************************************
 * FindMatch
 *************************************/
/// Get the item of a given value
/**
 \param val Value to search for
 \param func Match function
 \return The item found, or NULL
 */
void* SHVDynArrayBase::FindMatch(void* val, SHVDynArrayBase::MatchFunc func)
{
size_t index = FindMatchIndex(val,func);
	return (index<Items ? Array[index] : NULL);
}

/*************************************
 * FindFirstMatchIndex
 *************************************/
/// Get the first index of a given value
/**
 \param val Value do search for
 \param func Match function
 \return Index of the item found, or InvalidIndex
 */
size_t SHVDynArrayBase::FindFirstMatchIndex(void* val, SHVDynArrayBase::MatchFunc func)
{
size_t retVal = FindMatchIndex(val, func);

	if (retVal<Items)
	{
		while (retVal>0 && !func(&val,&Array[retVal-1]))
			retVal--;
	}

	return retVal;
}

/*************************************
 * FindFirstMatch
 *************************************/
/// Get the first item of a given value
/**
 \param val Value to search for
 \param func Match function
 \return The item found, or NULL
 */
void* SHVDynArrayBase::FindFirstMatch(void* val, SHVDynArrayBase::MatchFunc func)
{
size_t index = FindFirstMatchIndex(val,func);
	return (index<Items ? Array[index] : NULL);
}
