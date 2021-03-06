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
#define __SHIVA_UTILS_STRING_CPP
#include "../../../include/utils/shvstring.h"
#include "../../../include/utils/shvlist.h"
#undef __SHIVA_UTILS_STRING_CPP
#include "../../../include/utils/shvstringc.inl"

#include <string.h>
#include <stdio.h>

#ifndef va_list
# include <stdarg.h>
#endif

#ifndef __SHVSTRING_EXCLUDE_UNICODE
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
SHVInt64Val SHVStringUTF8C::StrToInt64(const SHVChar* str, SHVChar** ptr, int base)
{
	return SHVString8C::StrToInt64(str,ptr,base);
}
double SHVStringUTF8C::StrToDouble(const SHVChar* str, SHVChar** ptr)
{
	return SHVString8C::StrToDouble(str,ptr);
}
size_t SHVStringUTF8C::StrLen(const SHVChar* str)
{
	return SHVString8C::StrLen(str);
}
size_t SHVStringUTF8C::StrLenInChars(const SHVChar* str)
{
size_t retVal = 0;
size_t charLen;

	if (!str)
		return 0;

	while (*str)
	{
		charLen = GetUTF8CharLen(*str);
		while (charLen && *str) { charLen--; str++; if ((*str&0xC0) != 0x80) { break; } }
		retVal++;
	}

	return retVal;
}
size_t SHVStringUTF8C::SizeOfChars(const char* str, size_t chars)
{
const char* offset = str;
size_t charLen;

	if (!str)
		return 0;

	while (*offset && chars)
	{
		charLen = GetUTF8CharLen(*offset);
		while (charLen && *offset) { charLen--; offset++; if ((*offset&0xC0) != 0x80) { break; } }
		chars--;
	}
	
	return offset-str;
}
size_t SHVStringUTF8C::SizeOfCharsReverse(const char* str, size_t chars)
{
const char* end = str + SHVString8C::StrLen(str);
const char* offset = end;
size_t utf8Chars;
size_t charLen;

	if (!str)
		return 0;

	for(;offset > str && chars;)
	{
		offset--;
		chars--;

		// move past unicode chars
		if ( ((*offset)&0xC0) == 0x80 )
		{
			// Scan for char header
			for (utf8Chars=0;offset > str;)
			{
				utf8Chars++;
				offset--;
				if ( ((*offset)&0xC0) == 0x00 ) // invalid utf8 char, rollback
				{
					offset+=utf8Chars;
					break;
				}
				else if ( ((*offset)&0xC0) == 0xC0 ) // start char
				{
					charLen = GetUTF8CharLen(*offset);
					if (charLen <= utf8Chars) // too many bytes, rollback
						offset+=utf8Chars;
					break;
				}
			}
		}
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
SHVStringBufferUTF8 SHVStringUTF8C::Format(const SHVChar* str, ...)
{
SHVStringUTF8 retVal;
SHVVA_LIST args;
	SHVVA_START(args,str);
	retVal.FormatList(str,args);
	SHVVA_END(args);
	return retVal.ReleaseBuffer();
}
SHVStringBufferUTF8 SHVStringUTF8C::FormatList(const SHVChar* str, SHVVA_LIST args)
{
SHVStringUTF8 retVal;
	retVal.FormatList(str,args);
	return retVal.ReleaseBuffer();
}
size_t SHVStringUTF8C::GetUTF8CharLen(const SHVChar ch)
{
	if ( (ch&0x80) == 0x00 ) return 1;
	if ( (ch&0xE0) == 0xC0 ) return 2;
	if ( (ch&0xF0) == 0xE0 ) return 3;
	if ( (ch&0xF8) == 0xF0 ) return 4;
	if ( (ch&0xFC) == 0xF8 ) return 5;
	if ( (ch&0xFE) == 0xFC ) return 6;
	return 1; // invalid char
}
bool SHVStringUTF8C::IsValidUTF8(const SHVChar* str)
{
bool retVal = true;
size_t tlen;

	if (!str)
		return retVal;

	while (*str && retVal)
	{
		if ( ((*str)&0x80) )
		{
			tlen = GetUTF8CharLen(*str);
			retVal = (tlen > 1);
			for (str++, tlen--; *str && retVal && tlen; str++, tlen--)
			{
				retVal = ( ((*str)&0xC0) == 0x80 );
			}
		}
		else
		{
			str++;
		}
	}

	return retVal;
}
size_t SHVStringUTF8C::UTF8CharLenToBytes(const SHVChar* str, size_t len)
{
size_t retVal = 0;
size_t charLen = 0;

	if (!str || len == 0)
		return retVal;

	while (*str && (len || charLen))
	{
		if (charLen == 0 && len)
		{
			charLen = GetUTF8CharLen(*str);
			len--;
		}
		else if (charLen > 1 && (*str&0xC0) != 0x80) // Safety measure, in case the utf8 encoding is b0rked
		{
			charLen = 1;
		}
		
		str++;
		charLen--;
		retVal++;
	}

	return retVal;
}
size_t SHVStringUTF8C::UTF8BytesToCharLen(const SHVChar* str, size_t len)
{
size_t retVal = 0;
size_t charLen;

	if (!str)
		return 0;

	while (*str && len)
	{
		charLen = GetUTF8CharLen(*str);
		while (charLen && len && *str) { charLen--; str++; len--; if ((*str&0xC0) != 0x80) { break; } }
		retVal++;
	}

	return retVal;
}


// ========================================================================================================
//  SHVStringC class Constant
// ========================================================================================================
/// \class SHVStringUTF8C shvstringc.h "shiva/include/utils/shvstringc.h"

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
 * ToIn64
 *************************************/
SHVInt64Val SHVStringUTF8C::ToInt64(SHVChar** endChar) const
{
SHVChar* charBuf = Buffer;
	if (IsNull())
		return 0;
	if (endChar == NULL)
		endChar = &charBuf;
	return StrToInt64(Buffer,endChar,10);
}

/*************************************
 * ToDouble
 *************************************/
double SHVStringUTF8C::ToDouble(SHVChar** endChar) const
{
SHVChar* charBuf = Buffer;
	if (IsNull())
		return 0;
	if (endChar == NULL)
		endChar = &charBuf;
	return StrToDouble(Buffer,endChar);
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
 * Int64ToString
 *************************************/
SHVStringBufferUTF8 SHVStringUTF8C::Int64ToString(SHVInt64Val val)
{
static const SHVChar nChar[] = { '%', 'l', 'l', 'd', '\0' };
SHVStringUTF8 str;
	str.Format(nChar, val);
	return str.ReleaseBuffer();
}

/*************************************
 * DoubleToString
 *************************************/
SHVStringBufferUTF8 SHVStringUTF8C::DoubleToString(double val)
{
static const SHVChar nChar[] = { '%', '.', '1', '4', 'g', '\0' };
static const SHVChar comma[] = { ',', '\0' };
static const SHVChar dot[] = { '.', '\0' };
SHVStringUTF8 str;
	str.Format(nChar, val);
	str.Replace(comma, dot);
	return str.ReleaseBuffer();
}

/*************************************
 * ToStr8
 *************************************/
SHVStringBufferUTF8 SHVStringUTF8C::ToStrUTF8() const
{
SHVStringUTF8 str(*this);
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
size_t SHVStringUTF8C::GetLengthInChars() const
{
	if (IsNull())
		return 0;
	return StrLenInChars(Buffer);
}

/*************************************
 * hashing function
 *************************************/
SHVStringUTF8C::operator SHVHashValue() const
{
size_t retVal = 0;
	for(const SHVChar* ch = GetSafeBuffer(); *ch; ch++)
		retVal = *ch + retVal * 5;
	return retVal;
}

/*************************************
 * Compare*
 *************************************/
int SHVStringUTF8C::Compare(const SHVStringUTF8C& str) const
{
	if (Buffer == NULL && str.IsNull())
		return 0;
	if (Buffer == NULL && !str.IsNull())
		return -1;
	if (Buffer && str.IsNull())
		return 1;
	return StrCmp(Buffer,str.GetBufferConst());
}
int SHVStringUTF8C::CompareNoCase(const SHVStringUTF8C& str) const
{
	if (Buffer == NULL && str.IsNull())
		return 0;
	if (Buffer == NULL && !str.IsNull())
		return -1;
	if (Buffer && str.IsNull())
		return 1;
	return StrCaseCmp(Buffer,str.GetBufferConst());
}

/*************************************
 * Right
 *************************************/
SHVStringBufferUTF8 SHVStringUTF8C::Right(size_t len) const
{
SHVStringUTF8 retVal;

	if (!IsNull())
	{
	size_t strLen = GetLength();
		if (len>strLen) len=strLen;

		retVal.SetBufferSize(len+1);
		if (len > 0) memcpy(retVal.Buffer,Buffer+(strLen-len),len*sizeof(SHVChar));
		retVal.Buffer[len] = '\0';
	}
	return retVal.ReleaseBuffer();
}
SHVStringBufferUTF8 SHVStringUTF8C::RightInChars(size_t len) const
{
SHVStringUTF8 retVal;

	if (!IsNull())
	{
	size_t strLen = GetLength();

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
	size_t strLen = GetLength();
		if (len>strLen) len=strLen;

		retVal.SetBufferSize(len+1);
		if (len>0) memcpy(retVal.Buffer,Buffer,len*sizeof(SHVChar));
		retVal.Buffer[len] = '\0';
	}
	return retVal.ReleaseBuffer();
}
SHVStringBufferUTF8 SHVStringUTF8C::LeftInChars(size_t len) const
{
SHVStringUTF8 retVal;

	if (!IsNull())
	{
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
size_t strLen = GetLength();
	
	if (first>strLen)
		; // return null string
	else if (len>=strLen)
		retVal = Buffer+first;
	else
	{
		if ( (first+len) >= strLen) len = strLen - first;

		retVal.SetBufferSize(len+1);
		if (len>0) ::memcpy(retVal.Buffer,Buffer+first,len*sizeof(SHVChar));
		retVal.GetBuffer()[len] = '\0';
	}
	return retVal.ReleaseBuffer();
}
SHVStringBufferUTF8 SHVStringUTF8C::MidInChars(size_t first, size_t len) const
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
 * LocateChar
 *************************************/
long SHVStringUTF8C::LocateChar(SHVChar ch) const
{
long retVal;

	if (IsNull())
		return -1;	

	for (retVal = 0; Buffer[retVal] != '\0' && Buffer[retVal] != ch; retVal++) ;
	return (Buffer[retVal] != '\0' ? retVal : -1);
}

/*************************************
 * Find
 *************************************/
long SHVStringUTF8C::Find(const SHVStringUTF8C& str,long offset) const
{
	return SHVString8C(Buffer).Find(str.GetBufferConst(), offset);
}
long SHVStringUTF8C::FindInChars(const SHVStringUTF8C& str,long offset) const
{
const SHVChar* self = Buffer;
long retVal = -1;

	if (Buffer)
	{
		if (offset > 0)
		{
			self += UTF8CharLenToBytes(Buffer,(size_t)offset);
		}
		
		retVal = SHVString8C(self).Find(str.GetBufferConst());
		if (retVal > 0)
			retVal = (long)UTF8BytesToCharLen(self,(size_t)retVal) + offset;
	}

	return retVal;
}

/*************************************
 * ReverseFind
 *************************************/
long SHVStringUTF8C::ReverseFind(const SHVStringUTF8C& str) const
{
	return SHVString8C(Buffer).ReverseFind(str.Buffer);
}
long SHVStringUTF8C::ReverseFindInChars(const SHVStringUTF8C& str) const
{
long retVal = SHVString8C(Buffer).ReverseFind(str.Buffer);
	if (retVal > 0)
		retVal = UTF8BytesToCharLen(Buffer,retVal);
	return retVal;
}

/*************************************
 * Tokenize
 *************************************/
SHVStringBufferUTF8 SHVStringUTF8C::Tokenize(const SHVStringUTF8C& tokens, size_t& pos) const
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
/// \class SHVStringUTF8CRef shvstringc.h "shiva/include/utils/shvstringc.h"

/*************************************
 * Constructors
 *************************************/
SHVStringUTF8CRef::SHVStringUTF8CRef(const SHVChar* buffer)
{
	if (buffer)
	{
	size_t len = SHVString8C::StrLen(buffer)+1;
	Header* header = (Header*)malloc(sizeof(Header)+len*sizeof(SHVChar));

		header->RefCount.Initialize(1);
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
SHVStringUTF8CRef::SHVStringUTF8CRef(const SHVStringUTF8CRef& buffer) : SHVStringUTF8C()
{
	if (buffer.Buffer)
	{
	Header* header = (Header*)buffer.Buffer;
		header--; // the actual buffer was stored from the beginning of the string

		header->RefCount.LockedIncrement();

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
		if ( header->RefCount.LockedDecrementAndCheckDelete() )
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
		if ( header->RefCount.LockedDecrementAndCheckDelete() )
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

		header->RefCount.LockedIncrement();

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
void SHVStrUTF8_DestroyBufferFunc(SHVChar* chars) { ::free(chars); }
///\endcond
# define BUFFER_ALLOC(buf,size) buf = (size > 0 ? (SHVChar*)::malloc((size)*sizeof(SHVChar)) : NULL);
# define BUFFER_MOVE(buf,funcPtr) Buffer = buf; DestroyFunc = funcPtr; buf=NULL; funcPtr=NULL;
# define BUFFER_DESTROY(buf) if (buf) { (*DestroyFunc)(buf); buf = NULL; }
# define BUFFER_DESTROY2(code,buf) SHVStrUTF8_DestroyBuffer d = (buf ? DestroyFunc : NULL); code; if (d) { (*d)(buf); }
# define BUFFER_SETDESTROYFUNC DestroyFunc = &SHVStrUTF8_DestroyBufferFunc;
#else
# define BUFFER_ALLOC(buf,size) buf = (size > 0 ? (SHVChar*)::malloc((size)*sizeof(SHVChar)) : NULL);
# define BUFFER_MOVE(buf,funcPtr) Buffer = buf; buf=NULL;
# define BUFFER_DESTROY(buf) if (buf) { ::free(buf); buf = NULL; }
# define BUFFER_DESTROY2(code,buf) code; if (buf) { ::free(buf); buf = NULL; }
# define BUFFER_SETDESTROYFUNC
#endif


// ========================================================================================================
//  SHVStringBuffer class
// ========================================================================================================
/// \class SHVStringBufferUTF8 shvstring.h "shiva/include/utils/shvstring.h"

// constructor
SHVStringBufferUTF8::SHVStringBufferUTF8(const SHVStringBufferUTF8& buffer) : SHVStringUTF8C()
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
/// \class SHVStringUTF8 shvstring.h "shiva/include/utils/shvstring.h"

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
		retVal.Buffer = (SHVChar*)::malloc((len+1)*sizeof(SHVChar));
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
SHVStringUTF8::SHVStringUTF8(const SHVStringUTF8& str) : SHVStringUTF8C()
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
	BufferLen = 0;
}


/*************************************
 * AddChars*
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
void SHVStringUTF8::AddCharsInChars(const SHVChar* chars, size_t len)
{
	AddChars(chars,UTF8CharLenToBytes(chars,len));
}

/*************************************
 * MakeUpper
 *************************************/
void SHVStringUTF8::MakeUpper()
{
size_t len=GetLength();
	for (size_t i=0; i<len; i++)
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
size_t len=GetLength();
	for (size_t i=0; i<len; i++)
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

	self = SHVStringBuffer8::Encapsulate(Buffer);
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
		BufferLen++;
	}
	return *this;
}
SHVStringUTF8& SHVStringUTF8::operator=(const SHVStringBufferUTF8& str)
{
	BUFFER_DESTROY(Buffer);

	BUFFER_MOVE(((SHVStringBufferUTF8*)&str)->Buffer,((SHVStringBufferUTF8*)&str)->DestroyFunc);
	
	BufferLen = (Buffer ? SHVString8C::StrLen(Buffer) + 1 : 0); // we assume the buffer has the length og the string, + the '\0' termination
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
