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
#include "../../../include/utils/shvstringutf8.h"
#include "../../../include/utils/shvstring.h"
#include "../../../include/utils/shvlist.h"

#include <string.h>
#include <stdio.h>

#ifndef va_list
# include <stdarg.h>
#endif

#if defined __SHVSTRING_INCLUDE_UNICODE
# ifdef __SHIVA_WIN32
# elif defined(__SHIVA_EPOC)
#  include <string.h>
# endif
#endif

#ifdef __SHIVA_EPOC
# include <e32std.h>
# include <e32des8.h>
#endif

#ifdef __SHIVA_WIN32
# define vsnprintf _vsnprintf
#else
# include <stdlib.h>
#endif

// ========================================================================================================
//  convenience functions for easy portability
// ========================================================================================================
long SHVStringUTF8C::StrToL(const SHVChar* str, SHVChar** ptr, int base)
{
	return SHVString8C::StrToL(str,ptr,base);
}
size_t SHVStringUTF8C::StrLen(const SHVChar* str)
{
size_t retVal = 0;

	if (!str)
		return 0;

	for(;*str;str++)
	{
		if ( ((*str)&0xC0) != 0xC0 )
			retVal++;
	}

	return retVal;
}
size_t SHVStringUTF8C::SizeOfChars(const char* str, size_t chars)
{
const char* offset = str;

	for(;*str && chars;offset++)
	{
		if ( ((*offset)&0xC0) != 0xC0 )
			chars--;
	}
	
	return offset-str;
}
size_t SHVStringUTF8C::SizeOfCharsReverse(const char* str, size_t chars)
{
const char* end = str + SHVString8C::StrLen(str);
const char* offset = end;

	for(;offset > str && chars;)
	{
		offset--;
		chars--;

		// move past unicode chars
		for(;offset > str && ((*(offset-1))&0xC0) == 0xC0;offset--) ;
	}
	
	return end-offset;
}
int SHVStringUTF8C::StrCmp(const SHVChar* str1,const SHVChar* str2)
{
	return SHVString8C::StrCmp(str1,str2);
}
int SHVStringUTF8C::StrCaseCmp(const SHVChar* s1,const SHVChar* s2)
{
#if defined(__SHIVA_EPOC)
SHVStringUTF8 str1(s1);
SHVStringUTF8 str2(s2);
	str1.MakeLower();
	str2.MakeLower();
	return str1.Compare(str2);
#elif defined(__SHIVA_WIN32)
	return _stricmp(s1,s2);
#else
	return strcasecmp(s1,s2);
#endif
}
SHVChar* SHVStringUTF8C::StrCat(SHVChar* dest, const SHVChar* source)
{
	return SHVString8C::StrCat(dest,source);
}


// ========================================================================================================
//  SHVStringC class Constant
// ========================================================================================================

/*************************************
 * Comparison operators
 *************************************/
bool SHVStringUTF8C::operator==(const SHVStringUTF8C& str) const
{ return Compare(str) == 0; }
bool SHVStringUTF8C::operator!=(const SHVStringUTF8C& str) const
{ return Compare(str) != 0; }
bool SHVStringUTF8C::operator<(const SHVStringUTF8C& str) const
{ return Compare(str) < 0;  }
bool SHVStringUTF8C::operator<=(const SHVStringUTF8C& str) const
{ return Compare(str) <= 0; }
bool SHVStringUTF8C::operator>(const SHVStringUTF8C& str) const
{ return Compare(str) > 0;  }
bool SHVStringUTF8C::operator>=(const SHVStringUTF8C& str) const
{ return Compare(str) >= 0; }

/*************************************
 * GetSafeBuffer
 *************************************/
const SHVChar* SHVStringUTF8C::GetSafeBuffer() const
{
static const SHVChar nChar[] = { '\0' };
	return (IsNull() ? nChar : Buffer);
}

/*************************************
 * ToLong
 *************************************/
long SHVStringUTF8C::ToLong(SHVChar** endChar) const
{
SHVChar* charBuf = Buffer;
	if (IsNull())
		return 0;
	if (endChar == NULL)
		endChar = &charBuf;
	return StrToL(Buffer,endChar,10);
}

/*************************************
 * LongToString
 *************************************/
SHVStringBufferUTF8 SHVStringUTF8C::LongToString(long val)
{
static const SHVChar nChar[] = { '%', 'd', '\0' };
SHVStringUTF8 str;
	str.Format(nChar, val);
	return str.ReleaseBuffer();
}

/*************************************
 * GetLength
 *************************************/
size_t SHVStringUTF8C::GetLength() const
{
	if (IsNull())
		return 0;
	return StrLen(Buffer);
}

/*************************************
 * hashing function
 *************************************/
SHVStringUTF8C::operator size_t() const
{
size_t retVal = 0;
	for(const SHVChar* ch = GetSafeBuffer(); *ch; ch++)
		retVal = *ch + retVal * 5;
	return retVal;
}

/*************************************
 * Compare*
 *************************************/
short SHVStringUTF8C::Compare(const SHVStringUTF8C& str) const
{
	if (Buffer == NULL && str.IsNull())
		return 0;
	if (Buffer == NULL && !str.IsNull())
		return -1;
	if (Buffer && str.IsNull())
		return 1;
	return (short)StrCmp(Buffer,str.GetBufferConst());
}
short SHVStringUTF8C::CompareNoCase(const SHVStringUTF8C& str) const
{
	if (Buffer == NULL && str.IsNull())
		return 0;
	if (Buffer == NULL && !str.IsNull())
		return -1;
	if (Buffer && str.IsNull())
		return 1;
	return (short)StrCaseCmp(Buffer,str.GetBufferConst());
}

/*************************************
 * Right
 *************************************/
SHVStringBufferUTF8 SHVStringUTF8C::Right(size_t len) const
{
SHVStringUTF8 retVal;

	if (!IsNull())
	{
	size_t strLen = SHVString8C::StrLen(Buffer);

		len = SizeOfCharsReverse(Buffer,len);

		retVal.SetBufferSize(len+1);
		if (len > 0) memcpy(retVal.Buffer,Buffer+(strLen-len),len*sizeof(SHVChar));
		retVal.Buffer[len] = '\0';
	}
	return retVal.ReleaseBuffer();
}

/*************************************
 * Left
 *************************************/
SHVStringBufferUTF8 SHVStringUTF8C::Left(size_t len) const
{
SHVStringUTF8 retVal;

	if (!IsNull())
	{
	size_t strLen = SHVString8C::StrLen(Buffer);

		len = SizeOfChars(Buffer,len);

		retVal.SetBufferSize(len+1);
		if (len>0) memcpy(retVal.Buffer,Buffer,len*sizeof(SHVChar));
		retVal.Buffer[len] = '\0';
	}
	return retVal.ReleaseBuffer();
}

/*************************************
 * Mid
 *************************************/
SHVStringBufferUTF8 SHVStringUTF8C::Mid(size_t first, size_t len) const
{
SHVStringUTF8 retVal;
size_t strLen = SHVString8C::StrLen(Buffer);
	
	first = SizeOfChars(Buffer,first);

	if (first>=strLen)
		; // return null string
	else if (len>=(strLen-first))
		retVal = Buffer+first;
	else
	{
		len = SizeOfChars(Buffer+first,len);

		retVal.SetBufferSize(len+1);
		if (len>0) ::memcpy(retVal.Buffer,Buffer+first,len*sizeof(SHVChar));
		retVal.Buffer[len] = '\0';
	}
	return retVal.ReleaseBuffer();
}

/*************************************
 * Find
 *************************************/
long SHVStringUTF8C::Find(const SHVStringUTF8C& str,long offset) const
{
const SHVChar* self = Buffer;

	if (Buffer)
	{
		while (offset && *self)
		{
			if (((*self)&0xC0) != 0xC0)
			{
				offset--;
			}
			self++;
		}
	}

	return SHVString8C(self).Find(str.GetBufferConst());
}

/*************************************
 * ReverseFind
 *************************************/
long SHVStringUTF8C::ReverseFind(const SHVStringUTF8C& str) const
{
	return SHVString8C(Buffer).ReverseFind(str.Buffer);
}

/*************************************
 * Tokenize
 *************************************/
SHVStringBufferUTF8 SHVStringUTF8C::Tokenize(const SHVString8C& tokens, size_t& pos) const
{
size_t start = pos;
size_t len   = SHVString8C::StrLen(Buffer);
int cpChars = 0;

	while (pos < len && tokens.LocateChar(Buffer[pos]) == -1)
	{
		if (Buffer[pos] > 0)
			cpChars++;
		pos++;
	}

	if (pos<len) // then it was a token hit - move past token
		pos++;

	return Mid(start,cpChars);
}



//=========================================================================================================
// SHVStringUTF8CRef class - reference counting constant string
//=========================================================================================================

/*************************************
 * Constructors
 *************************************/
SHVStringUTF8CRef::SHVStringUTF8CRef(const SHVChar* buffer)
{
	if (buffer)
	{
	size_t len = SHVString8C::StrLen(buffer)+1;
	Header* header = (Header*)malloc(sizeof(Header)+len*sizeof(SHVChar));

		header->RefCount = 1;
#ifdef __SHVSTRING_HEAPPROTECT
		header->DestroyBuffer = DestroyBuffer;
#endif

		Buffer = (SHVChar*)(header+1);
		memcpy(Buffer, buffer, len*sizeof(SHVChar));
	}
	else
	{
		Buffer = NULL;
	}
}
SHVStringUTF8CRef::SHVStringUTF8CRef(const SHVStringUTF8CRef& buffer)
{
	if (buffer.Buffer)
	{
	Header* header = (Header*)buffer.Buffer;
		header--; // the actual buffer was stored from the beginning of the string

		header->RefCount++;

	}

	Buffer = (SHVChar*)buffer.Buffer;
}

/*************************************
 * Destructor
 *************************************/
SHVStringUTF8CRef::~SHVStringUTF8CRef()
{
	if (Buffer)
	{
	Header* header = (Header*)Buffer;
		header--; // the actual buffer was stored from the beginning of the string

		// dereference and check if we must delete it
		if ( --header->RefCount <= 0 )
		{
#ifdef __SHVSTRING_HEAPPROTECT
			(header->DestroyBuffer)(header);
#else
			free(header);
#endif
		}
		Buffer = NULL;
	}
}

/*************************************
 * operators
 *************************************/
SHVStringUTF8CRef& SHVStringUTF8CRef::operator=(const SHVStringUTF8CRef& str)
{
	if (Buffer)
	{
		Header* header = (Header*)Buffer;
		header--; // the actual buffer was stored from the beginning of the string

		// dereference and check if we must delete it
		if ( --header->RefCount <= 0 )
		{
#ifdef __SHVSTRING_HEAPPROTECT
			(header->DestroyBuffer)(header);
#else
			free(header);
#endif
		}
		Buffer = NULL;
	}
	
	if (str.Buffer)
	{
		Header* header = (Header*)str.Buffer;
		header--; // the actual buffer was stored from the beginning of the string

		header->RefCount++;

	}

	Buffer = (SHVChar*)str.Buffer;

	return *this;
}

#ifdef __SHVSTRING_HEAPPROTECT
/*************************************
 * DestroyBuffer
 *************************************/
void SHVStringUTF8CRef::DestroyBuffer(SHVStringUTF8CRef::Header* buffer)
{
	free(buffer);
}
#endif



#ifdef __SHVSTRING_HEAPPROTECT
///\cond INTERNAL
void SHVStrUTF8_DestroyBufferFunc(SHVChar* chars) { delete [] chars; }
///\endcond
# define BUFFER_ALLOC(buf,size) buf = (size > 0 ? new SHVChar[size] : NULL);
# define BUFFER_MOVE(buf,funcPtr) Buffer = buf; DestroyFunc = funcPtr; buf=NULL; funcPtr=NULL;
# define BUFFER_DESTROY(buf) if (buf) { (*DestroyFunc)(buf); buf = NULL; }
# define BUFFER_DESTROY2(code,buf) SHVStrUTF8_DestroyBuffer d = (buf ? DestroyFunc : NULL); code; if (d) { (*d)(buf); }
# define BUFFER_SETDESTROYFUNC DestroyFunc = &SHVStrUTF8_DestroyBufferFunc;
#else
# define BUFFER_ALLOC(buf,size) buf = (size > 0 ? new SHVChar[size] : NULL);
# define BUFFER_MOVE(buf,funcPtr) Buffer = buf; buf=NULL;
# define BUFFER_DESTROY(buf) if (buf) { delete [] buf; buf = NULL; }
# define BUFFER_DESTROY2(code,buf) code; if (buf) { delete [] buf; buf = NULL; }
# define BUFFER_SETDESTROYFUNC
#endif


// ========================================================================================================
//  SHVStringBuffer class
// ========================================================================================================

// constructor
SHVStringBufferUTF8::SHVStringBufferUTF8(const SHVStringBufferUTF8& buffer)
{
	BUFFER_MOVE(((SHVStringBufferUTF8*)&buffer)->Buffer,((SHVStringBufferUTF8*)&buffer)->DestroyFunc);
}
SHVStringBufferUTF8::~SHVStringBufferUTF8() { BUFFER_DESTROY(Buffer); }
/// Encapsulates the C style string buffer into a SHVStringBuffer
SHVStringBufferUTF8 SHVStringBufferUTF8::Encapsulate(SHVChar* buffer)
{
	SHVStringBufferUTF8 retVal;
	retVal.Buffer = buffer;
#ifdef __SHVSTRING_HEAPPROTECT
	retVal.DestroyFunc = &SHVStrUTF8_DestroyBufferFunc;
#endif
	return retVal;
}

// assignment operator
SHVStringBufferUTF8& SHVStringBufferUTF8::operator=(SHVStringBufferUTF8& buffer)
{
	BUFFER_DESTROY(Buffer);
	BUFFER_MOVE(buffer.Buffer,buffer.DestroyFunc);
	return *this;
}



// ========================================================================================================
//  SHVString class
// ========================================================================================================


/*************************************
 * Copy static
 *************************************/
#ifdef __SHIVA_EPOC
SHVStringBufferUTF8 SHVStringUTF8::Copy(const TPtrC8& aPtr)
{
SHVStringBufferUTF8 retVal;
size_t len = aPtr.Length();

	if (len)
	{
		retVal.Buffer = new SHVChar[len+1];
		if (retVal.Buffer)
		{
			memcpy(retVal.Buffer, aPtr.Ptr(), len*sizeof(SHVChar));
			retVal.Buffer[len] = 0;
		}
	}

	return retVal;
}
#endif

/*************************************
 * Constructors
 *************************************/
SHVStringUTF8::SHVStringUTF8()
{
	Buffer = NULL;
	BufferLen = 0;
}
SHVStringUTF8::SHVStringUTF8(const SHVStringUTF8C& str)
{
	Buffer = NULL;
	BufferLen = 0;
	
	*this = str;
}
SHVStringUTF8::SHVStringUTF8(const SHVStringUTF8& str)
{
	Buffer = NULL;
	BufferLen = 0;
	
	*this = str;
}
SHVStringUTF8::SHVStringUTF8(const SHVStringBufferUTF8& str)
{
	Buffer = NULL;
	BufferLen = 0;
	
	*this = str;
}


/*************************************
 * Destructor
 *************************************/
SHVStringUTF8::~SHVStringUTF8()
{
	BUFFER_DESTROY(Buffer);
}

///\cond INTERNAL
/*************************************
 * Buffer handling
 *************************************/
void SHVStringUTF8::AllocBuffer(size_t size)
{
	BUFFER_DESTROY(Buffer);
	BufferLen = size;
	BUFFER_ALLOC(Buffer,size);
	BUFFER_SETDESTROYFUNC;
}
SHVStringBufferUTF8 SHVStringUTF8::ReleaseBuffer()
{
SHVChar* buf = Buffer;

	Buffer = NULL;
	BufferLen = 0;

	return SHVStringBufferUTF8::Encapsulate(buf);
}
///\endcond

/*************************************
 * SetToNull
 *************************************/
void SHVStringUTF8::SetToNull()
{
	BUFFER_DESTROY(Buffer);
}


/*************************************
 * AddChars
 *************************************/
void SHVStringUTF8::AddChars(const SHVChar* chars, size_t len)
{
size_t oldlen = SHVString8C::StrLen(Buffer);

	if (!len)
		return;

	if (BufferLen < (oldlen+len+1))
	{
	SHVChar* tempbuf = Buffer;

		BufferLen = oldlen+len+1;
		BUFFER_ALLOC(Buffer,BufferLen);
		Buffer[0] = '\0';

		if (Buffer && oldlen)
			StrCat(Buffer, tempbuf);

		if (tempbuf)
			BUFFER_DESTROY(tempbuf);

		BUFFER_SETDESTROYFUNC;
	}

	if (chars)
	{
		memcpy(Buffer+oldlen, chars, len*sizeof(SHVChar));
		Buffer[oldlen+len] = '\0';
	}

}

/*************************************
 * MakeUpper
 *************************************/
void SHVStringUTF8::MakeUpper()
{
	for (size_t i=0; i<BufferLen; i++)
	{
		if ( Buffer[i] >= 'a' && Buffer[i] <= 'z' )
			Buffer[i] -= 'a'-'A';
	}
}

/*************************************
 * MakeLower
 *************************************/
void SHVStringUTF8::MakeLower()
{
	for (size_t i=0; i<BufferLen; i++)
	{
		if ( Buffer[i] >= 'A' && Buffer[i] <= 'Z' )
			Buffer[i] += 'a'-'A';
	}
}

/*************************************
 * Trim functions
 *************************************/
void SHVStringUTF8::TrimLeft(const SHVChar* trimString)
{
SHVChar* oldBuf = Buffer;
size_t cut = 0;
static const SHVChar DefaultTrimStr[] = {' ','\t','\r','\n','\0'};
SHVString8C TrimStr(DefaultTrimStr);
	
	if (trimString) TrimStr = trimString;

	if (IsNull()) return;

	while (Buffer[cut] != '\0')
	{
		if (TrimStr.LocateChar(oldBuf[cut])>=0) cut++;
		else break;
	}
	
	if (cut > 0)
	{
		Buffer = NULL;
		BUFFER_DESTROY2(*this = oldBuf+cut,oldBuf); // sets the new buffer, and destroys the old (heap protection)
	}
}
void SHVStringUTF8::TrimRight(const SHVChar* trimString)
{
static const SHVChar DefaultTrimStr[] = {' ','\t','\r','\n','\0'};
SHVString8C TrimStr(DefaultTrimStr);
size_t len;
	
	if (trimString) TrimStr = trimString;

	if (IsNull()) return;

	len = SHVString8C::StrLen(Buffer);

	while (len > 0)
	{
		if (TrimStr.LocateChar(Buffer[len-1])>=0)
		{
			if ((len-1) > 0 && Buffer[len-2] > 0)
				len--;
			else // unicode char - not a white space after all
				break;
		}
		else break;
	}
	
	Buffer[len] = '\0';
}
void SHVStringUTF8::Trim()
{
	TrimRight();
	TrimLeft();
}

/*************************************
 * Replace
 *************************************/
void SHVStringUTF8::Replace(const SHVStringUTF8C& search, const SHVStringUTF8C& repl)
{
SHVString8 self;
SHVString8C srch(search.GetBufferConst());
SHVString8C rpl(repl.GetBufferConst());

	self.Buffer = Buffer;
	Buffer = NULL;

	self.Replace(srch,rpl);
	*this = SHVStringBufferUTF8::Encapsulate(self.Buffer);
	self.Buffer = NULL;
}

/*************************************
 * Format
 *************************************/
void SHVStringUTF8::Format(const SHVChar* s, ...)
{
SHVVA_LIST args;
	SHVVA_START(args,s);
	FormatList(s,args);
	SHVVA_END(args);
}
void SHVStringUTF8::FormatList(const SHVChar* s, SHVVA_LIST args)
{
SHVString8 self;
	
	self.FormatList(s,args);
	*this = SHVStringBufferUTF8::Encapsulate(self.Buffer);
	self.Buffer = NULL;
}

/*************************************
 * Operators
 *************************************/
SHVStringUTF8& SHVStringUTF8::operator=(const SHVStringUTF8C& str)
{
	BUFFER_DESTROY(Buffer);

	BufferLen = SHVString8C::StrLen(str.GetBufferConst());

	if (!str.IsNull())
	{
		BUFFER_ALLOC(Buffer,BufferLen+1);
		BUFFER_SETDESTROYFUNC;
		if (BufferLen) ::memcpy(Buffer,str.GetBufferConst(),BufferLen*sizeof(SHVChar));
		Buffer[BufferLen] = '\0';
	}
	return *this;
}
SHVStringUTF8& SHVStringUTF8::operator=(const SHVStringBufferUTF8& str)
{
	BUFFER_DESTROY(Buffer);

	BUFFER_MOVE(((SHVStringBufferUTF8*)&str)->Buffer,((SHVStringBufferUTF8*)&str)->DestroyFunc);
	
	BufferLen = SHVString8C::StrLen(Buffer) + 1; // we assume the buffer has the length og the string, + the '\0' termination
	return *this;
}
SHVStringUTF8& SHVStringUTF8::operator+=(const SHVStringUTF8C& str)
{
	if (!str.IsNull())
		AddChars(str.GetBufferConst(), SHVString8C::StrLen(str.Buffer));
	return *this;
}
SHVStringBufferUTF8 operator+(const SHVStringUTF8C& str1, const SHVStringUTF8C& str2)
{
SHVStringUTF8 s;
size_t len = SHVString8C::StrLen(str1.Buffer) + SHVString8C::StrLen(str2.Buffer) + 1;

	s.SetBufferSize(len);
	s.GetBuffer()[0] = '\0';

	if (!str1.IsNull())
		SHVStringUTF8C::StrCat(s.GetBuffer(), str1.GetBufferConst());
	if (!str2.IsNull())
		SHVStringUTF8C::StrCat(s.GetBuffer(), str2.GetBufferConst());
	return s.ReleaseBuffer();
}
