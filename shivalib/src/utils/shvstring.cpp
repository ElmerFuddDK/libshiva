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
#include "../../../include/utils/shvstring.h"
#include "../../../include/utils/shvlist.h"
#include "../../../include/utils/shvrefobject.h"

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

#include <math.h>

// ========================================================================================================
//  convenience functions for easy portability
// ========================================================================================================
long SHVString8C::StrToL(const SHVChar* str, SHVChar** ptr, int base)
{
	return (str ? ::strtol(str,ptr,base) : 0);
}
SHVInt64Val SHVString8C::StrToInt64(const SHVChar* str, SHVChar** ptr, int base)
{
#ifdef __SHIVA_WINCE
SHVInt64Val retVal = (str ? ::_atoi64(str) : 0);
	if (ptr)
	{
		*ptr = (char*)str;
		while (*ptr && (*ptr)[0] >= '0' && (*ptr)[0] <= '9') (*ptr)++;
	}
	return retVal;
#elif defined(__SHIVA_WIN32) && !defined(__MINGW32__)
	return (str ? ::_strtoi64(str,ptr,base) : 0);
#else
	return (str ? ::strtoll(str,ptr,base) : 0);
#endif
}
double SHVString8C::StrToDouble(const SHVChar* str, SHVChar** ptr)
{
SHVString8 copy;
int exponent;
SHVChar ch;
SHVChar* dummyPtr;
bool dotMode = false;

	if (!ptr)
		ptr = &dummyPtr;
	
	if (!str || !StrLen(str))
	{
		*ptr = (SHVChar*)str;
		return 0.0;
	}

	copy.SetBufferSize(StrLen(str));
	copy[0] = '\0';

	if (*str == '-' || *str == '+')
	{
		ch = (SHVChar)*str;
		copy.AddChars(&ch,1);
		str++;
	}

	exponent = 0;
	for(*ptr = (SHVChar*)str; str && **ptr != '\0'; (*ptr)++)
	{
		if (**ptr >= '0' && **ptr <= '9')
		{
			ch = (SHVChar)**ptr;
			copy.AddChars(&ch,1);
			if (dotMode)
				exponent--;
		}
		else if (**ptr == '.')
		{
			if (dotMode)
				break;
			dotMode = true;
		}
		else
		{
			break;
		}
	}

	if (**ptr == 'e' || **ptr == 'E')
	{
		(*ptr)++;
		exponent += StrToL(*ptr,ptr,10);
	}
	
	if (exponent)
		copy += SHVString8C::Format("e%d",exponent);

	return ::strtod(copy.GetSafeBuffer(),NULL);
}
size_t SHVString8C::StrLen(const SHVChar* str)
{
	return (str ? ::strlen(str) : 0);
}
int SHVString8C::StrCmp(const SHVChar* str1,const SHVChar* str2)
{
	return strcmp(str1,str2);
}
int SHVString8C::StrCaseCmp(const SHVChar* s1,const SHVChar* s2)
{
#if defined(__SHIVA_EPOC)
SHVString8 str1(s1);
SHVString8 str2(s2);
	str1.MakeLower();
	str2.MakeLower();
	return str1.Compare(str2);
#elif defined(__SHIVA_WIN32)
	return _stricmp(s1,s2);
#else
	return strcasecmp(s1,s2);
#endif
}
SHVChar* SHVString8C::StrCat(SHVChar* dest, const SHVChar* source)
{
	return (source ? strcat(dest,source) : dest);
}
SHVStringBuffer8 SHVString8C::Format(const SHVChar* str, ...)
{
SHVString8 retVal;
SHVVA_LIST args;
	SHVVA_START(args,str);
	retVal.FormatList(str,args);
	SHVVA_END(args);
	return retVal.ReleaseBuffer();
}
SHVStringBuffer8 SHVString8C::FormatList(const SHVChar* str, SHVVA_LIST args)
{
SHVString8 retVal;
	retVal.FormatList(str,args);
	return retVal.ReleaseBuffer();
}


// ========================================================================================================
//  SHVStringC class Constant
// ========================================================================================================

/*************************************
 * Comparison operators
 *************************************/
bool SHVString8C::operator==(const SHVString8C& str) const
{ return Compare(str) == 0; }
bool SHVString8C::operator!=(const SHVString8C& str) const
{ return Compare(str) != 0; }
bool SHVString8C::operator<(const SHVString8C& str) const
{ return Compare(str) < 0;  }
bool SHVString8C::operator<=(const SHVString8C& str) const
{ return Compare(str) <= 0; }
bool SHVString8C::operator>(const SHVString8C& str) const
{ return Compare(str) > 0;  }
bool SHVString8C::operator>=(const SHVString8C& str) const
{ return Compare(str) >= 0; }

/*************************************
 * GetSafeBuffer
 *************************************/
const SHVChar* SHVString8C::GetSafeBuffer() const
{
static const SHVChar nChar[] = { '\0' };
	return (IsNull() ? nChar : Buffer);
}

/*************************************
 * ToLong
 *************************************/
long SHVString8C::ToLong(SHVChar** endChar) const
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
SHVInt64Val SHVString8C::ToInt64(SHVChar** endChar) const
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
double SHVString8C::ToDouble(SHVChar** endChar) const
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
SHVStringBuffer8 SHVString8C::LongToString(long val)
{
static const SHVChar nChar[] = { '%', 'd', '\0' };
SHVString8 str;
	str.Format(nChar, val);
	return str.ReleaseBuffer();
}

/*************************************
 * Int64ToString
 *************************************/
SHVStringBuffer8 SHVString8C::Int64ToString(SHVInt64Val val)
{
static const SHVChar nChar[] = { '%', 'l', 'l', 'd', '\0' };
SHVString8 str;
	str.Format(nChar, val);
	return str.ReleaseBuffer();
}

/*************************************
 * DoubleToString
 *************************************/
SHVStringBuffer8 SHVString8C::DoubleToString(double val)
{
static const SHVChar nChar[] = { '%', '.', '1', '4', 'g', '\0' };
static const SHVChar comma[] = { ',', '\0' };
static const SHVChar dot[] = { '.', '\0' };
SHVString8 str;
	str.Format(nChar, val);
	str.Replace(comma, dot);
	return str.ReleaseBuffer();
}

/*************************************
 * ToStr8
 *************************************/
SHVStringBuffer8 SHVString8C::ToStr8() const
{
SHVString8 str(*this);
	return str.ReleaseBuffer();
}

/*************************************
 * GetLength
 *************************************/
size_t SHVString8C::GetLength() const
{
	if (IsNull())
		return 0;
	return StrLen(Buffer);
}

/*************************************
 * hashing function
 *************************************/
SHVString8C::operator SHVHashValue() const
{
size_t retVal = 0;
	for(const SHVChar* ch = GetSafeBuffer(); *ch; ch++)
		retVal = *ch + retVal * 5;
	return retVal;
}

/*************************************
 * Compare*
 *************************************/
short SHVString8C::Compare(const SHVString8C& str) const
{
	if (Buffer == NULL && str.IsNull())
		return 0;
	if (Buffer == NULL && !str.IsNull())
		return -1;
	if (Buffer && str.IsNull())
		return 1;
	return (short)StrCmp(Buffer,str.GetBufferConst());
}
short SHVString8C::CompareNoCase(const SHVString8C& str) const
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
SHVStringBuffer8 SHVString8C::Right(size_t len) const
{
SHVString8 retVal;

	if (!IsNull())
	{
	size_t strLen = GetLength();
		if (len>strLen) len=strLen;

		retVal.SetBufferSize(len+1);
		if (len>0) memcpy(retVal.Buffer,Buffer+(strLen-len),len*sizeof(SHVChar));
		retVal[len] = '\0';
	}
	return retVal.ReleaseBuffer();
}

/*************************************
 * Left
 *************************************/
SHVStringBuffer8 SHVString8C::Left(size_t len) const
{
SHVString8 retVal;

	if (!IsNull())
	{
	size_t strLen = GetLength();
		if (len>strLen) len=strLen;

		retVal.SetBufferSize(len+1);
		if (len>0) memcpy(retVal.Buffer,Buffer,len*sizeof(SHVChar));
		retVal[len] = '\0';
	}
	return retVal.ReleaseBuffer();
}

/*************************************
 * Mid
 *************************************/
SHVStringBuffer8 SHVString8C::Mid(size_t first, size_t len) const
{
SHVString8 retVal;
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
		retVal[len] = '\0';
	}
	return retVal.ReleaseBuffer();
}

/*************************************
 * LocateChar
 *************************************/
long SHVString8C::LocateChar(SHVChar ch) const
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
long SHVString8C::Find(const SHVString8C& str,long offset) const
{
long retVal = offset;
long tmpL;
bool found;
long len = (long)GetLength();
long strlen = (long)str.GetLength();

	for (found = false;retVal < (long)(len-strlen+1); retVal++)
	{
		for (tmpL = 0; tmpL < (long)strlen; tmpL++)
		{
			if ( str.Buffer[tmpL] != Buffer[tmpL+retVal] )
				break;
		}
		
		found = (tmpL == (long)strlen);
		
		if (found) break;
	}
	
	return ( retVal >= (long)(len-strlen+1) ? -1 : retVal);
}

/*************************************
 * ReverseFind
 *************************************/
long SHVString8C::ReverseFind(const SHVString8C& str) const
{
long len = (long)GetLength();
long strlen = (long)str.GetLength();
long retVal = len-strlen;
long tmpL;
bool found;

	for (found = false;retVal >= 0; retVal--)
	{
		for (tmpL = 0; tmpL < (long)strlen; tmpL++)
		{
			if ( str.Buffer[tmpL] != Buffer[tmpL+retVal] )
				break;
		}
		
		found = (tmpL == strlen);
		
		if (found) break;
	}

	if (retVal < 0)
		retVal = -1;

	return retVal;
}

/*************************************
 * Tokenize
 *************************************/
SHVStringBuffer8 SHVString8C::Tokenize(const SHVString8C& tokens, size_t& pos) const
{
size_t start = pos;
size_t len   = GetLength();
int cpChars = 0;

	while (pos < len && tokens.LocateChar(Buffer[pos]) == -1)
	{
		cpChars++;
		pos++;
	}

	if (pos<len) // then it was a token hit - move past token
		pos++;

	return Mid(start,cpChars);
}


///\cond INTERNAL
/*************************************
 * CalcFormatBufferSize
 *************************************/
size_t SHVString8C::CalcFormatBufferSize(const SHVChar* s, SHVVA_LIST args)
{
SHVVA_LIST argList;
size_t len = StrLen(s)+1;
long pos;
static const SHVChar percentStr[] = { '%', '\0' };
static const SHVChar tokenStr[] = { '%', 's', 'd', 'i', 'o', 'u', 'x', 'X', 'e', 'E', 'f', 'F', 'g', 'G', 'a', 'A', 'p', 'c', '\0' };
SHVString8C str(s);
SHVString8C tokStr(tokenStr);
const SHVChar* tokChar;

// types
SHVChar* tStr;
int tChar;
int tInt;
void* tPtr;
double tDbl;
int lLen=0,rLen=0;
const SHVChar* vCharL;
SHVChar* vCharR;

	SHVVA_COPY( argList, args );

	for (pos = str.Find(percentStr);pos>=0;pos=str.Find(percentStr,pos))
	{
		pos++;
		for (tokChar=vCharL=str.GetBufferConst()+pos; *tokChar != '\0';tokChar++)
		{
			if (tokStr.LocateChar( *tokChar ) >= 0)
			{
				lLen = StrToL(vCharL,&vCharR,10);
				if (*vCharR == '.') { vCharR++; rLen = StrToL(vCharR, &vCharR, 10); }
				else { rLen = 0; }
				break;
			}
		}

		if (lLen < 0) lLen = -lLen;
		if (rLen < 0) rLen = -rLen;

		if (*tokChar != '%' && *tokChar != '\0')
		{

			switch(*tokChar)
			{
			case 's': // string
				tStr = SHVVA_ARG(argList,SHVChar*);
				if (!lLen && !rLen)
					len += StrLen(tStr)-2;
				else
					len += (lLen > rLen ? lLen : rLen );
				break;
			case 'd': // signed integers
			case 'i': 
			case 'u': // unsigned integers
			case 'x':
			case 'X':
			case 'o': // octal unsigned integer
				tInt = SHVVA_ARG(argList,int);
				if (!lLen && !rLen)
					len +=  (*tokChar == 'o' ? 12 : 11); // max size used by an int
				else
				{
					rLen++; // make room for a possible - sign
					len += (lLen > rLen ? lLen: rLen );
				}
				break;
			case 'g': // doubles with an easy way of length measuring
			case 'G':
#ifndef __SHIVA_EPOC
				if (rLen)
					len += rLen+5; // make room for scientific notation
				else
					len += 11; // seems to be the max size of this type
				break;
#endif
			case 'e': // doubles
			case 'E':
			case 'f':
			case 'F':
			case 'a': // double hex
			case 'A':
				tDbl = SHVVA_ARG(argList,double);
				len += 50; // here we might want to calculate the size at some point
				break;
			case 'p': // pointer in hex
				tPtr = SHVVA_ARG(argList,void*);
				len += 11; // max size used by an int
			case 'c':
				tChar = SHVVA_ARG(argList,int);
				len--; 
				break;
			}
		}
	}
	
	SHVVA_END( argList );

	return len;
}
///\endcond



//=========================================================================================================
// SHVString8CRef class - reference counting constant string
//=========================================================================================================

/*************************************
 * Constructors
 *************************************/
SHVString8CRef::SHVString8CRef(const SHVChar* buffer)
{
	if (buffer)
	{
	size_t len = StrLen(buffer)+1;
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
SHVString8CRef::SHVString8CRef(const SHVString8CRef& buffer) : SHVString8C()
{
	if (buffer.Buffer)
	{
	Header* header = (Header*)buffer.Buffer;
		header--; // the actual buffer was stored from the beginning of the string

		SHVRefObject::LockedIncrement(& header->RefCount);

	}

	Buffer = (SHVChar*)buffer.Buffer;
}

/*************************************
 * Destructor
 *************************************/
SHVString8CRef::~SHVString8CRef()
{
	if (Buffer)
	{
	Header* header = (Header*)Buffer;
		header--; // the actual buffer was stored from the beginning of the string

		SHVRefObject::LockedDecrement(& header->RefCount);

		// dereference and check if we must delete it
		if ( header->RefCount <= 0 )
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
SHVString8CRef& SHVString8CRef::operator=(const SHVString8CRef& str)
{
	if (Buffer)
	{
		Header* header = (Header*)Buffer;
		header--; // the actual buffer was stored from the beginning of the string

		SHVRefObject::LockedDecrement(& header->RefCount);

		// dereference and check if we must delete it
		if ( header->RefCount <= 0 )
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

		SHVRefObject::LockedIncrement(& header->RefCount);

	}

	Buffer = (SHVChar*)str.Buffer;

	return *this;
}

#ifdef __SHVSTRING_HEAPPROTECT
/*************************************
 * DestroyBuffer
 *************************************/
void SHVString8CRef::DestroyBuffer(SHVString8CRef::Header* buffer)
{
	free(buffer);
}
#endif



#ifdef __SHVSTRING_HEAPPROTECT
///\cond INTERNAL
void SHVStr8_DestroyBufferFunc(SHVChar* chars) { delete [] chars; }
///\endcond
# define BUFFER_ALLOC(buf,size) buf = (size > 0 ? new SHVChar[size] : NULL);
# define BUFFER_MOVE(buf,funcPtr) Buffer = buf; DestroyFunc = funcPtr; buf=NULL; funcPtr=NULL;
# define BUFFER_DESTROY(buf) if (buf) { (*DestroyFunc)(buf); buf = NULL; }
# define BUFFER_DESTROY2(code,buf) SHVStr8_DestroyBuffer d = (buf ? DestroyFunc : NULL); code; if (d) { (*d)(buf); }
# define BUFFER_SETDESTROYFUNC DestroyFunc = &SHVStr8_DestroyBufferFunc;
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
SHVStringBuffer8::SHVStringBuffer8(const SHVStringBuffer8& buffer) : SHVString8C()
{
	BUFFER_MOVE(((SHVStringBuffer8*)&buffer)->Buffer,((SHVStringBuffer8*)&buffer)->DestroyFunc);
}
SHVStringBuffer8::~SHVStringBuffer8() { BUFFER_DESTROY(Buffer); }
/// Encapsulates the C style string buffer into a SHVStringBuffer
SHVStringBuffer8 SHVStringBuffer8::Encapsulate(SHVChar* buffer)
{
	SHVStringBuffer8 retVal;
	retVal.Buffer = buffer;
#ifdef __SHVSTRING_HEAPPROTECT
	retVal.DestroyFunc = &SHVStr8_DestroyBufferFunc;
#endif
	return retVal;
}

// assignment operator
SHVStringBuffer8& SHVStringBuffer8::operator=(SHVStringBuffer8& buffer)
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
SHVStringBuffer8 SHVString8::Copy(const TPtrC8& aPtr)
{
SHVStringBuffer8 retVal;
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
SHVString8::SHVString8()
{
	Buffer = NULL;
	BufferLen = 0;
}
SHVString8::SHVString8(const SHVString8C& str)
{
	Buffer = NULL;
	BufferLen = 0;
	
	*this = str;
}
SHVString8::SHVString8(const SHVString8& str) : SHVString8C()
{
	Buffer = NULL;
	BufferLen = 0;
	
	*this = str;
}
SHVString8::SHVString8(const SHVStringBuffer8& str)
{
	Buffer = NULL;
	BufferLen = 0;
	
	*this = str;
}


/*************************************
 * Destructor
 *************************************/
SHVString8::~SHVString8()
{
	BUFFER_DESTROY(Buffer);
}

///\cond INTERNAL
/*************************************
 * Buffer handling
 *************************************/
void SHVString8::AllocBuffer(size_t size)
{
	BUFFER_DESTROY(Buffer);
	BufferLen = size;
	BUFFER_ALLOC(Buffer,size);
	BUFFER_SETDESTROYFUNC;
}
SHVStringBuffer8 SHVString8::ReleaseBuffer()
{
SHVChar* buf = Buffer;

	Buffer = NULL;
	BufferLen = 0;

	return SHVStringBuffer8::Encapsulate(buf);
}
///\endcond

/*************************************
 * SetToNull
 *************************************/
void SHVString8::SetToNull()
{
	BUFFER_DESTROY(Buffer);
}


/*************************************
 * AddChars
 *************************************/
void SHVString8::AddChars(const SHVChar* chars, size_t len)
{
size_t oldlen = GetLength();

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
void SHVString8::MakeUpper()
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
void SHVString8::MakeLower()
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
void SHVString8::TrimLeft(const SHVChar* trimString)
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
void SHVString8::TrimRight(const SHVChar* trimString)
{
static const SHVChar DefaultTrimStr[] = {' ','\t','\r','\n','\0'};
SHVString8C TrimStr(DefaultTrimStr);
size_t len;
	
	if (trimString) TrimStr = trimString;

	if (IsNull()) return;

	len = GetLength();

	while (len > 0)
	{
		if (TrimStr.LocateChar(Buffer[len-1])>=0) len--;
		else break;
	}
	
	Buffer[len] = '\0';
}
void SHVString8::Trim()
{
	TrimRight();
	TrimLeft();
}

/*************************************
 * Replace
 *************************************/
void SHVString8::Replace(const SHVString8C& search, const SHVString8C& repl)
{
SHVList<size_t> hits;
size_t searchlen = search.GetLength();

	for(long offset=0; (offset = Find(search,offset)) >= 0; offset+=(long)searchlen)
		hits.AddTail(offset);

	if (hits.GetCount())
	{
	size_t repllen = repl.GetLength();
	size_t pos;
	SHVListIterator<size_t> hititr(hits);
		
		if (repllen == searchlen)
		{
			while (hititr.MoveNext())
			{
				::memcpy(Buffer+hititr.Get(),repl.GetSafeBuffer(),searchlen*sizeof(SHVChar));
			}
		}
		else
		{
		size_t len = GetLength();
		SHVChar* oldbuf = Buffer;
		size_t newlen;

			Buffer = NULL;
	
			newlen = (len-hits.GetCount()*searchlen) + // whats left of the old string
					hits.GetCount()*repllen;
	
			BufferLen = newlen + 1;
			BUFFER_ALLOC( Buffer, BufferLen );
			Buffer[0] = '\0';
			pos = 0;
			
			while (hititr.MoveNext())
			{
				// copy whatever is between last position and new
				AddChars(oldbuf+pos, hititr.Get()-pos);
				pos=hititr.Get()+searchlen;
	
				// insert replace string
				if (repllen>0) StrCat(Buffer,repl.GetBufferConst());
				
			}
			AddChars(oldbuf+pos, len-pos);
	
			BUFFER_DESTROY(oldbuf);
			BUFFER_SETDESTROYFUNC;
		}
	}

}

/*************************************
 * Format
 *************************************/
void SHVString8::Format(const SHVChar* s, ...)
{
SHVVA_LIST args;
	SHVVA_START(args,s);
	FormatList(s,args);
	SHVVA_END(args);
}
void SHVString8::FormatList(const SHVChar* s, SHVVA_LIST args)
{
#if defined(__SHIVA_EPOC)
size_t newSize = CalcFormatBufferSize(s,args);

	AllocBuffer(newSize);

	if (Buffer)
	{
	TPtr8 p((TUint8*)Buffer,BufferLen-1);
	TPtrC8 fStr( (const TUint8*)s );
	SHVVA_LIST argList;

		SHVVA_COPY( argList, args );
		p.FormatList(fStr,argList);
		SHVVA_END( argList );

		Buffer[p.Length()] = '\0';
	}
#else
size_t sizeSteps = StrLen(s);
size_t newSize = sizeSteps+20;
int n;

	SHVVA_LIST argList;

	while (1)
	{
		AllocBuffer(newSize);
		SHVVA_COPY( argList, args );
		n = vsnprintf( (char*)Buffer, newSize, (const char*)s, argList );
		SHVVA_END( argList );

		if (n > -1 && (size_t)n < newSize)
			break;

		if (n == -1)
			newSize += sizeSteps*10;
		else
			newSize = n+1;
	}
#endif
}

/*************************************
 * Operators
 *************************************/
SHVChar& SHVString8::operator[](size_t i)
{
	return Buffer[i];
}
SHVString8& SHVString8::operator=(const SHVString8C& str)
{
	BUFFER_DESTROY(Buffer);

	BufferLen = str.GetLength();

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
SHVString8& SHVString8::operator=(const SHVStringBuffer8& str)
{
	BUFFER_DESTROY(Buffer);

	BUFFER_MOVE(((SHVStringBuffer8*)&str)->Buffer,((SHVStringBuffer8*)&str)->DestroyFunc);
	
	BufferLen = (Buffer ? GetLength() + 1 : 0); // we assume the buffer has the length og the string, + the '\0' termination
	return *this;
}
SHVString8& SHVString8::operator+=(const SHVString8C& str)
{
	if (!str.IsNull())
		AddChars(str.GetBufferConst(), str.GetLength());
	return *this;
}
SHVStringBuffer8 operator+(const SHVString8C& str1, const SHVString8C& str2)
{
SHVString8 s;
size_t len = str1.GetLength() + str2.GetLength() + 1;

	s.SetBufferSize(len);
	s.GetBuffer()[0] = '\0';

	if (!str1.IsNull())
		SHVString8C::StrCat(s.GetBuffer(), str1.GetBufferConst());
	if (!str2.IsNull())
		SHVString8C::StrCat(s.GetBuffer(), str2.GetBufferConst());
	return s.ReleaseBuffer();
}
