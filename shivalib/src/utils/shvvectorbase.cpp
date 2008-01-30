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
#include "../../../include/utils/shvvectorbase.h"

#ifdef __SHIVA_EPOC
#include <string.h>
#endif


//=========================================================================================================
// SHVDynArrayBase class - A basic dynamic array containing void pointers
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVVectorBase::SHVVectorBase()
{
	Array = ArrayEnd = NULL;
}

/*************************************
 * Destructor
 *************************************/
SHVVectorBase::~SHVVectorBase()
{
	Clear();
}

// properties
/*************************************
 * CalculateCount
 *************************************/
size_t SHVVectorBase::CalculateCount()
{
	return ArrayEnd-Array;
}

/*************************************
 * IsEmpty
 *************************************/
bool SHVVectorBase::IsEmpty()
{
	return (Array ? false : true);
}


// array management
/*************************************
 * Clear
 *************************************/
void SHVVectorBase::Clear(SHVVectorBase::DestroyFunc func)
{
	if (Array)
	{
		if (func)
		{
		void** ptr;
			for (ptr = Array; ptr < ArrayEnd; ptr++)
			{
				if (*ptr)
					func(*ptr);
			}
		}

		::free(Array);
		Array = ArrayEnd = NULL;
	}
}

/*************************************
 * index operator
 *************************************/
void* SHVVectorBase::operator[](size_t index)
{
	return Array[index];
}

/*************************************
 * Add
 *************************************/
/// Adds an item to the end of the array, expanding it
/**
 \param item The item to add
 \param growSize The amount to expand with
 *
 * This function will always add the item to the end of the array.
 * if you want to replace a NULL item you'll need to run through
 * it yourself, and use the replace option.
 *
 \note You must ALWAYS use the same growsize since this is used
       for detecting if it needs to expand.
 */
size_t SHVVectorBase::Add(void* item, size_t growSize)
{
size_t count = CalculateCount();

	if (!Array)
	{
		Array = ArrayEnd = (void**)::malloc(sizeof(void*)*growSize);
	}
	else if (count%growSize == 0) // we need to expand the array
	{
	void** oldArray = Array;
	size_t newLen = count + growSize;

		// set the item first, to avoid setting the pointer twice after copying

		Array = (void**)::malloc(sizeof(void*)*newLen);

		::memcpy(Array,oldArray,sizeof(void*)*count);
		::free(oldArray);

		ArrayEnd = Array+count;
	}

	*ArrayEnd = item;
	ArrayEnd++;

	return count;
}

/*************************************
 * Remove
 *************************************/
void* SHVVectorBase::Remove(size_t index)
{
void* retVal = Array[index];

	Array[index] = NULL;
	return retVal;
}

/*************************************
 * Replace
 *************************************/
void* SHVVectorBase::Replace(size_t index, void* item)
{
void* retVal = Array[index];

	Array[index] = item;
	return retVal;
}

/*************************************
 * Pop
 *************************************/
/// Pops the last item off the array, and truncates
void* SHVVectorBase::Pop(size_t growSize)
{
void** ptr = ArrayEnd-1;
void* retVal = *ptr;

	*ptr = NULL;
	Truncate(growSize);

	return retVal;
}

/*************************************
 * Truncate
 *************************************/
void SHVVectorBase::Truncate(size_t growSize)
{
	if (Array)
	{
	size_t count = CalculateCount();
	size_t freeIndexes = growSize - (count%growSize);
	
		for (;ArrayEnd>Array && *(ArrayEnd-1) == NULL; ArrayEnd--) freeIndexes++;
	
		if (Array == ArrayEnd) // no items
		{
			::free(Array);
			Array = ArrayEnd = NULL;
		}
		else if (freeIndexes >= growSize)
		{
		void** oldArray = Array;
		size_t newLen;
	
			count = CalculateCount();
	
			newLen = count + growSize - (count%growSize);
	
			Array = ArrayEnd = (void**)::malloc(sizeof(void*)*newLen);
			::memcpy(Array,oldArray,sizeof(void*)*count);
			::free(oldArray);
	
			ArrayEnd = Array+count;
		}
	}
}

/*************************************
 * Compress
 *************************************/
void SHVVectorBase::Compress(size_t growSize)
{
	if (Array)
	{
	size_t count = CalculateCount();
	size_t freeIndexes = growSize - (count%growSize);
	void** ptr;
	
		// first we truncate
		for (;ArrayEnd>Array && *(ArrayEnd-1) == NULL; ArrayEnd--) freeIndexes++;

		// next we search for NULLs and move the end items down
		for (ptr = Array; ptr < ArrayEnd; ptr++)
		{
			if (*ptr == NULL)
			{
				ArrayEnd--;
				*ptr = *ArrayEnd;
				freeIndexes++;
			}
		}
	
		if (Array == ArrayEnd) // no items
		{
			::free(Array);
			Array = ArrayEnd = NULL;
		}
		else if (freeIndexes >= growSize)
		{
		void** oldArray = Array;
		size_t newLen;
	
			count = CalculateCount();
	
			newLen = count%growSize;
			newLen = ( newLen ? count + growSize - newLen : count );
	
			Array = ArrayEnd = (void**)::malloc(sizeof(void*)*newLen);
			::memcpy(Array,oldArray,sizeof(void*)*count);
			::free(oldArray);
	
			ArrayEnd = Array+count;
		}
	}
}
