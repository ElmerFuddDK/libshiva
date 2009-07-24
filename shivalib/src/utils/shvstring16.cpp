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
#include "../../../include/utils/shvstring16.h"
#include "../../../include/utils/shvlist.h"

#ifndef va_list
# include <stdarg.h>
#endif

#if defined __SHVSTRING_INCLUDE_UNICODE
# ifdef __SHIVA_WINCE
#  define vswprintf _vsnwprintf
# elif defined(__SHIVA_WIN32)
#  include <wchar.h>
# elif defined(__SHIVA_EPOC)
#  include <string.h>
# endif
#endif

#ifdef __SHIVA_LINUX
# include <wchar.h>
# include <stdlib.h>
# include "../../../include/utils/shvstringutf8.h" // for Format .. horrible hack
#endif

#include <math.h>


// ========================================================================================================
//  convenience functions for easy portability
// ========================================================================================================

long SHVString16C::StrToL(const SHVWChar* str, SHVWChar** ptr, int base)
{
#if defined(__SHIVA_LINUX) || defined(__SHIVA_EPOC)
long retVal = 0;
SHVWChar firstVal = *str;

	if (firstVal == '-' || firstVal == '+')
		str++;

	for(*ptr = (SHVWChar*)str; str && *ptr != '\0'; (*ptr)++)
	{
		if (**ptr < '0' || **ptr > '9') // only base10 for now
			break;
	}
	
	long i=1;
	for(SHVWChar* cPtr = (*ptr); cPtr != str; cPtr--, i*=base)
	{
		retVal += (*(cPtr-1) - '0')*i;
	}

	if (firstVal == '-')
		retVal *= -1;

	return retVal;
#else
	return (str ? wcstol((const wchar_t*)str,(wchar_t**)ptr,base) : NULL);
#endif

}
SHVInt64Val SHVString16C::StrToInt64(const SHVWChar* str, SHVWChar** ptr, int base)
{
#if defined(__SHIVA_LINUX) || defined(__SHIVA_EPOC) || defined(__SHIVA_WINCE)
SHVInt64Val retVal = 0;
SHVWChar firstVal = *str;

	if (firstVal == '-' || firstVal == '+')
		str++;

	for(*ptr = (SHVWChar*)str; str && *ptr != '\0'; (*ptr)++)
	{
		if (**ptr < '0' || **ptr > '9') // only base10 for now
			break;
	}
	
	long i=1;
	for(SHVWChar* cPtr = (*ptr); cPtr != str; cPtr--, i*=base)
	{
		retVal += (*(cPtr-1) - '0')*i;
	}

	if (firstVal == '-')
		retVal *= -1;

	return retVal;
#else
	return (str ? _wcstoi64((const wchar_t*)str,(wchar_t**)ptr,base) : NULL);
#endif

}
double SHVString16C::StrToDouble(const SHVWChar* str, SHVWChar** ptr)
{
double retVal = 0;
SHVWChar firstVal = *str;
SHVWChar* dummyPtr;
SHVWChar* cPtr;
long i;

	if (!ptr)
		ptr = &dummyPtr;

	if (firstVal == '-' || firstVal == '+')
		str++;

	for(*ptr = (SHVWChar*)str; str && *ptr != '\0'; (*ptr)++)
	{
		if (**ptr < '0' || **ptr > '9')
			break;
	}
	
	i=1;
	for(cPtr = (*ptr); cPtr != str; cPtr--, i*=10)
	{
		retVal += (*(cPtr-1) - '0')*i;
	}

	if (**ptr == '.')
	{
		double dec = 0.1;
		for((*ptr)++;**ptr;(*ptr)++,dec/=10)
		{
			if (**ptr < '0' || **ptr > '9')
				break;
			retVal += ((**ptr) - '0')*dec;
		}
	}

	if (**ptr == 'e' || **ptr == 'E')
	{
		(*ptr)++;
		retVal *= pow(10.0, (double)StrToL(*ptr,ptr,10) );
	}

	if (firstVal == '-')
		retVal *= -1.0;

	return retVal;
}
size_t SHVString16C::StrLen(const SHVWChar* str)
{
#if defined(__SHIVA_LINUX) || defined(__SHIVA_EPOC)
size_t len;
	for (len=0;str && str[len];len++) ;
	return len;
	// TODO: Use memmem in linux
#else
	return (str ? ::wcslen((const wchar_t*)str) : 0);
#endif
}
int SHVString16C::StrCmp(const SHVWChar* str1,const SHVWChar* str2)
{
#if defined(__SHIVA_LINUX) || defined(__SHIVA_EPOC)
int retVal;
SHVWChar *ch1 = (SHVWChar*)str1;
SHVWChar *ch2 = (SHVWChar*)str2;

	for (; *ch1 == *ch2 && *ch1 != 0; ch1++, ch2++)
		;

	if (*ch1 < *ch2)
		retVal = -1;
	else if (*ch1 > *ch2)
		retVal = 1;
	else
		retVal=0;

	return retVal;
#else
	return wcscmp((const wchar_t*)str1,(const wchar_t*)str2);
#endif
}
int SHVString16C::StrCaseCmp(const SHVWChar* s1,const SHVWChar* s2)
{
#if defined(__SHIVA_LINUX) || defined(__SHIVA_EPOC)
SHVString16 str1(s1);
SHVString16 str2(s2);
	str1.MakeLower();
	str2.MakeLower();
	return str1.Compare(str2);
#elif defined(__SHIVA_WIN32)
	return _wcsicmp((const wchar_t*)s1,(const wchar_t*)s2);
#else
	return wcscasecmp(s1,s2);
#endif
}
SHVWChar* SHVString16C::StrCat(SHVWChar* dest, const SHVWChar* source)
{
#if defined(__SHIVA_LINUX) || defined(__SHIVA_EPOC)
size_t sourcelen = StrLen(source);
	if (sourcelen>0)
	{
	size_t dstlen = StrLen(dest);
	SHVWChar* dPtr = dest+dstlen;
		memcpy(dPtr,source,sourcelen*sizeof(SHVWChar));
		dPtr[sourcelen] = 0;
	}
	return dest;
#else
	return (source ? (SHVWChar*)wcscat((wchar_t*)dest,(const wchar_t*)source) : dest);
#endif
}
SHVStringBuffer16 SHVString16C::Format(const SHVWChar* str, ...)
{
SHVString16 retVal;
SHVVA_LIST args;
	SHVVA_START(args,str);
	retVal.FormatList(str,args);
	SHVVA_END(args);
	return retVal.ReleaseBuffer();
}
SHVStringBuffer16 SHVString16C::FormatList(const SHVWChar* str, SHVVA_LIST args)
{
SHVString16 retVal;
	retVal.FormatList(str,args);
	return retVal.ReleaseBuffer();
}


// ========================================================================================================
//  SHVStringC class Constant
// ========================================================================================================

/*************************************
 * Comparison operators
 *************************************/
bool SHVString16C::operator==(const SHVString16C& str) const
{ return Compare(str) == 0; }
bool SHVString16C::operator!=(const SHVString16C& str) const
{ return Compare(str) != 0; }
bool SHVString16C::operator<(const SHVString16C& str) const
{ return Compare(str) < 0;  }
bool SHVString16C::operator<=(const SHVString16C& str) const
{ return Compare(str) <= 0; }
bool SHVString16C::operator>(const SHVString16C& str) const
{ return Compare(str) > 0;  }
bool SHVString16C::operator>=(const SHVString16C& str) const
{ return Compare(str) >= 0; }

/*************************************
 * GetSafeBuffer
 *************************************/
const SHVWChar* SHVString16C::GetSafeBuffer() const
{
static const SHVWChar nChar[] = { '\0' };
	return (IsNull() ? nChar : Buffer);
}

/*************************************
 * ToLong
 *************************************/
long SHVString16C::ToLong(SHVWChar** endChar) const
{
SHVWChar* charBuf = Buffer;
	if (IsNull())
		return 0;
	if (endChar == NULL)
		endChar = &charBuf;
	return StrToL(Buffer,endChar,10);
}

/*************************************
 * ToInt64
 *************************************/
SHVInt64Val SHVString16C::ToInt64(SHVWChar** endChar) const
{
SHVWChar* charBuf = Buffer;
	if (IsNull())
		return 0;
	if (endChar == NULL)
		endChar = &charBuf;
	return StrToInt64(Buffer,endChar,10);
}

/*************************************
 * ToDouble
 *************************************/
double SHVString16C::ToDouble(SHVWChar** endChar) const
{
SHVWChar* charBuf = Buffer;
	if (IsNull())
		return 0;
	if (endChar == NULL)
		endChar = &charBuf;
	return StrToDouble(Buffer,endChar);
}

/*************************************
 * LongToString
 *************************************/
SHVStringBuffer16 SHVString16C::LongToString(long val)
{
static const SHVWChar nChar[] = { '%', 'd', '\0' };
SHVString16 str;
	str.Format(nChar, val);
	return str.ReleaseBuffer();
}

/*************************************
 * Int64ToString
 *************************************/
SHVStringBuffer16 SHVString16C::Int64ToString(SHVInt64Val val)
{
static const SHVWChar nChar[] = { '%', 'l', 'l', 'd', '\0' };
SHVString16 str;
	str.Format(nChar, val);
	return str.ReleaseBuffer();
}

/*************************************
 * DoubleToString
 *************************************/
SHVStringBuffer16 SHVString16C::DoubleToString(double val)
{
static const SHVWChar nChar[] = { '%', 'g', '\0' };
static const SHVWChar comma[] = { ',', '\0' };
static const SHVWChar dot[] = { '.', '\0' };
SHVString16 str;
	str.Format(nChar, val);
	str.Replace(comma, dot);
	return str.ReleaseBuffer();
}

/*************************************
 * ToStr16
 *************************************/
SHVStringBuffer16 SHVString16C::ToStr16() const
{
SHVString16 str(*this);
	return str.ReleaseBuffer();
}

/*************************************
 * GetLength
 *************************************/
size_t SHVString16C::GetLength() const
{
	if (IsNull())
		return 0;
	return StrLen(Buffer);
}

/*************************************
 * hashing function
 *************************************/
SHVString16C::operator SHVHashValue() const
{
size_t retVal = 0;
	for(const SHVWChar* ch = GetSafeBuffer(); *ch; ch++)
		retVal = *ch + retVal * 5;
	return retVal;
}

/*************************************
 * Compare*
 *************************************/
short SHVString16C::Compare(const SHVString16C& str) const
{
	if (Buffer == NULL && str.IsNull())
		return 0;
	if (Buffer == NULL && !str.IsNull())
		return -1;
	if (Buffer && str.IsNull())
		return 1;
	return (short)StrCmp(Buffer,str.GetBufferConst());
}
short SHVString16C::CompareNoCase(const SHVString16C& str) const
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
SHVStringBuffer16 SHVString16C::Right(size_t len) const
{
SHVString16 retVal;

	if (!IsNull())
	{
	size_t strLen = GetLength();
		if (len>strLen) len=strLen;

		retVal.SetBufferSize(len+1);
		if (len>0) memcpy(retVal.Buffer,Buffer+(strLen-len),len*sizeof(SHVWChar));
		retVal[len] = '\0';
	}
	return retVal.ReleaseBuffer();
}

/*************************************
 * Left
 *************************************/
SHVStringBuffer16 SHVString16C::Left(size_t len) const
{
SHVString16 retVal;

	if (!IsNull())
	{
	size_t strLen = GetLength();
		if (len>strLen) len=strLen;

		retVal.SetBufferSize(len+1);
		if (len>0) memcpy(retVal.Buffer,Buffer,len*sizeof(SHVWChar));
		retVal[len] = '\0';
	}
	return retVal.ReleaseBuffer();
}

/*************************************
 * Mid
 *************************************/
SHVStringBuffer16 SHVString16C::Mid(size_t first, size_t len) const
{
SHVString16 retVal;
size_t strLen = GetLength();
	
	if (first>strLen)
		; // return null string
	else if (len>=strLen)
		retVal = Buffer+first;
	else
	{
		if ( (first+len) >= strLen) len = strLen - first;

		retVal.SetBufferSize(len+1);
		if (len>0) ::memcpy(retVal.Buffer,Buffer+first,len*sizeof(SHVWChar));
		retVal[len] = '\0';
	}
	return retVal.ReleaseBuffer();
}

/*************************************
 * LocateChar
 *************************************/
long SHVString16C::LocateChar(SHVWChar ch) const
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
long SHVString16C::Find(const SHVString16C& str,long offset) const
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
long SHVString16C::ReverseFind(const SHVString16C& str) const
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
SHVStringBuffer16 SHVString16C::Tokenize(const SHVString16C& tokens, size_t& pos) const
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
size_t SHVString16C::CalcFormatBufferSize(const SHVWChar* s, SHVVA_LIST args)
{
SHVVA_LIST argList;
size_t len = StrLen(s)+1;
long pos;
static const SHVWChar percentStr[] = { '%', '\0' };
static const SHVWChar tokenStr[] = { '%', 's', 'd', 'i', 'o', 'u', 'x', 'X', 'e', 'E', 'f', 'F', 'g', 'G', 'a', 'A', 'p', 'c', '\0' };
SHVString16C str(s);
SHVString16C tokStr(tokenStr);
const SHVWChar* tokChar;

// types
SHVWChar* tStr;
int tChar;
int tInt;
void* tPtr;
double tDbl;
int lLen=0,rLen=0;
const SHVWChar* vCharL;
SHVWChar* vCharR;

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
				tStr = SHVVA_ARG(argList,SHVWChar*);
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
// SHVString16CRef class - reference counting constant string
//=========================================================================================================

/*************************************
 * Constructors
 *************************************/
SHVString16CRef::SHVString16CRef(const SHVWChar* buffer)
{
	if (buffer)
	{
	size_t len = StrLen(buffer)+1;
	Header* header = (Header*)malloc(sizeof(Header)+len*sizeof(SHVWChar));

		header->RefCount = 1;
#ifdef __SHVSTRING_HEAPPROTECT
		header->DestroyBuffer = DestroyBuffer;
#endif

		Buffer = (SHVWChar*)(header+1);
		memcpy(Buffer, buffer, len*sizeof(SHVWChar));
	}
	else
	{
		Buffer = NULL;
	}
}
SHVString16CRef::SHVString16CRef(const SHVString16CRef& buffer) : SHVString16C()
{
	if (buffer.Buffer)
	{
	Header* header = (Header*)buffer.Buffer;
		header--; // the actual buffer was stored from the beginning of the string

		header->RefCount++;

	}

	Buffer = (SHVWChar*)buffer.Buffer;
}

/*************************************
 * Destructor
 *************************************/
SHVString16CRef::~SHVString16CRef()
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
SHVString16CRef& SHVString16CRef::operator=(const SHVString16CRef& str)
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

	Buffer = (SHVWChar*)str.Buffer;

	return *this;
}

#ifdef __SHVSTRING_HEAPPROTECT
/*************************************
 * DestroyBuffer
 *************************************/
void SHVString16CRef::DestroyBuffer(SHVString16CRef::Header* buffer)
{
	free(buffer);
}
#endif



#ifdef __SHVSTRING_HEAPPROTECT
///\cond INTERNAL
void SHVStr16_DestroyBufferFunc(SHVWChar* chars) { delete [] chars; }
///\endcond
# define BUFFER_ALLOC(buf,size) buf = (size > 0 ? new SHVWChar[size] : NULL);
# define BUFFER_MOVE(buf,funcPtr) Buffer = buf; DestroyFunc = funcPtr; buf=NULL; funcPtr=NULL;
# define BUFFER_DESTROY(buf) if (buf) { (*DestroyFunc)(buf); buf = NULL; }
# define BUFFER_DESTROY2(code,buf) SHVStr16_DestroyBuffer d = (buf ? DestroyFunc : NULL); code; if (d) { (*d)(buf); }
# define BUFFER_SETDESTROYFUNC DestroyFunc = &SHVStr16_DestroyBufferFunc;
#else
# define BUFFER_ALLOC(buf,size) buf = (size > 0 ? new SHVWChar[size] : NULL);
# define BUFFER_MOVE(buf,funcPtr) Buffer = buf; buf=NULL;
# define BUFFER_DESTROY(buf) if (buf) { delete [] buf; buf = NULL; }
# define BUFFER_DESTROY2(code,buf) code; if (buf) { delete [] buf; buf = NULL; }
# define BUFFER_SETDESTROYFUNC
#endif


// ========================================================================================================
//  SHVStringBuffer class
// ========================================================================================================

// constructor
SHVStringBuffer16::SHVStringBuffer16(const SHVStringBuffer16& buffer) : SHVString16C()
{
	BUFFER_MOVE(((SHVStringBuffer16*)&buffer)->Buffer,((SHVStringBuffer16*)&buffer)->DestroyFunc);
}
SHVStringBuffer16::~SHVStringBuffer16() { BUFFER_DESTROY(Buffer); }
/// Encapsulates the C style string buffer into a SHVStringBuffer
SHVStringBuffer16 SHVStringBuffer16::Encapsulate(SHVWChar* buffer)
{
	SHVStringBuffer16 retVal;
	retVal.Buffer = buffer;
#ifdef __SHVSTRING_HEAPPROTECT
	retVal.DestroyFunc = &SHVStr16_DestroyBufferFunc;
#endif
	return retVal;
}

// assignment operator
SHVStringBuffer16& SHVStringBuffer16::operator=(SHVStringBuffer16& buffer)
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
SHVStringBuffer16 SHVString16::Copy(const TPtrC16& aPtr)
{
SHVStringBuffer16 retVal;
size_t len = aPtr.Length();

	if (len)
	{
		retVal.Buffer = new SHVWChar[len+1];
		if (retVal.Buffer)
		{
			memcpy(retVal.Buffer, aPtr.Ptr(), len*sizeof(SHVWChar));
			retVal.Buffer[len] = 0;
		}
	}

	return retVal;
}
#endif

/*************************************
 * Constructors
 *************************************/
SHVString16::SHVString16()
{
	Buffer = NULL;
	BufferLen = 0;
}
SHVString16::SHVString16(const SHVString16C& str)
{
	Buffer = NULL;
	BufferLen = 0;
	
	*this = str;
}
SHVString16::SHVString16(const SHVString16& str) : SHVString16C()
{
	Buffer = NULL;
	BufferLen = 0;
	
	*this = str;
}
SHVString16::SHVString16(const SHVStringBuffer16& str)
{
	Buffer = NULL;
	BufferLen = 0;
	
	*this = str;
}


/*************************************
 * Destructor
 *************************************/
SHVString16::~SHVString16()
{
	BUFFER_DESTROY(Buffer);
}

///\cond INTERNAL
/*************************************
 * Buffer handling
 *************************************/
void SHVString16::AllocBuffer(size_t size)
{
	BUFFER_DESTROY(Buffer);
	BufferLen = size;
	BUFFER_ALLOC(Buffer,size);
	BUFFER_SETDESTROYFUNC;
}
SHVStringBuffer16 SHVString16::ReleaseBuffer()
{
SHVWChar* buf = Buffer;

	Buffer = NULL;
	BufferLen = 0;

	return SHVStringBuffer16::Encapsulate(buf);
}
///\endcond

/*************************************
 * SetToNull
 *************************************/
void SHVString16::SetToNull()
{
	BUFFER_DESTROY(Buffer);
}


/*************************************
 * AddChars
 *************************************/
void SHVString16::AddChars(const SHVWChar* chars, size_t len)
{
size_t oldlen = GetLength();

	if (!len)
		return;

	if (BufferLen < (oldlen+len+1))
	{
	SHVWChar* tempbuf = Buffer;

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
		memcpy(Buffer+oldlen, chars, len*sizeof(SHVWChar));
		Buffer[oldlen+len] = '\0';
	}

}

/*************************************
 * MakeUpper
 *************************************/
void SHVString16::MakeUpper()
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
void SHVString16::MakeLower()
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
void SHVString16::TrimLeft(const SHVWChar* trimString)
{
SHVWChar* oldBuf = Buffer;
size_t cut = 0;
static const SHVWChar DefaultTrimStr[] = {' ','\t','\r','\n','\0'};
SHVString16C TrimStr(DefaultTrimStr);
	
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
void SHVString16::TrimRight(const SHVWChar* trimString)
{
static const SHVWChar DefaultTrimStr[] = {' ','\t','\r','\n','\0'};
SHVString16C TrimStr(DefaultTrimStr);
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
void SHVString16::Trim()
{
	TrimRight();
	TrimLeft();
}

/*************************************
 * Replace
 *************************************/
void SHVString16::Replace(const SHVString16C& search, const SHVString16C& repl)
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
				::memcpy(Buffer+hititr.Get(),repl.GetSafeBuffer(),searchlen*sizeof(SHVWChar));
			}
		}
		else
		{
		size_t len = GetLength();
		SHVWChar* oldbuf = Buffer;
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
void SHVString16::Format(const SHVWChar* s, ...)
{
SHVVA_LIST args;
	SHVVA_START(args,s);
	FormatList(s,args);
	SHVVA_END(args);
}
void SHVString16::FormatList(const SHVWChar* s, SHVVA_LIST args)
{
#ifdef __SHIVA_LINUX
static const SHVWChar pcnt[] = { '%', '\0' };
SHVString16C str(s);
SHVList<SHVString16, const SHVString16C> chunks;
SHVListIterator<SHVString16, const SHVString16C> chunkItr(chunks);
size_t chunklen = 0;
long oldpos = 0;
long pos = 0;
	
	for (pos=str.Find(pcnt,pos);pos>=0;pos=str.Find(pcnt,pos))
	{
		// copy preceding text
		if (pos>oldpos)
		{
			chunks.AddTail(SHVString16C(NULL));
			chunks.GetLast().AddChars(s+oldpos,(size_t)(pos-oldpos));
			chunklen += (size_t)(pos-oldpos);
		}
		
		chunks.AddTail(SHVString16C(NULL));
		pos++; // jump over %
		// check for exception
		if (s[pos] == '%')
		{
			chunks.GetLast().AddChars(s+pos,1);
			chunklen += 1;
			pos++;
		}
		else
		{
		long newpos = pos;
		SHVStringUTF8 fStr, tmpStr;
		bool isString = false;
		bool running = true;
			for (;running && s[newpos];newpos++)
			{
				switch (s[newpos]) {
				case 's':
					isString = true;
				case 'd': case 'i': case 'o': case 'u': case 'x': case 'X':
				case 'e': case 'E': case 'f': case 'F': case 'g': case 'G': case 'a':
				case 'A': case 'c': case 'p': case 'n': case 'm':
					running = false;
					break;
					// panicky exit point ...
				case '%': // Actually it would be ok to just scan for '%' chars, if it wasn't for 's'
					SHVASSERT(false);
					newpos--;
					running = false;
					break;
				}
			}
			fStr = str.Mid((size_t)pos-1,(size_t)(newpos-pos+1)).ToStrUTF8();
			if (isString)
				tmpStr.Format(fStr.GetSafeBuffer(),SHVString16C(SHVVA_ARG(args,const SHVWChar*)).ToStrUTF8().GetSafeBuffer());
			else
				tmpStr.FormatList(fStr.GetSafeBuffer(),args);
			chunks.GetLast() = tmpStr.ToStr16();
			pos = newpos;
		}
		
		oldpos = pos;
	}
	
	if (oldpos<=(pos=str.GetLength()))
	{
		chunks.AddTail(SHVString16C(NULL));
		chunks.GetLast().AddChars(s+oldpos,(size_t)(pos-oldpos+1));
		chunklen += (size_t)(pos-oldpos);
	}
	
	AllocBuffer(chunklen+1);
	Buffer[0] = '\0';
	while (chunkItr.MoveNext())
	{
		AddChars(chunkItr.Get().GetSafeBuffer(), chunkItr.Get().GetLength());
	}
#elif defined(__SHIVA_EPOC)
size_t newSize = CalcFormatBufferSize(s,args);

	AllocBuffer(newSize);

	if (Buffer)
	{
	TPtr16 p((TUint16*)Buffer,BufferLen-1);
	TPtrC16 fStr( (const TUint16*)s );
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
		n = vswprintf( (wchar_t*)Buffer, newSize, (const wchar_t*)s, argList );
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
SHVWChar& SHVString16::operator[](size_t i)
{
	return Buffer[i];
}
SHVString16& SHVString16::operator=(const SHVString16C& str)
{
	BUFFER_DESTROY(Buffer);

	BufferLen = str.GetLength();

	if (!str.IsNull())
	{
		BUFFER_ALLOC(Buffer,BufferLen+1);
		BUFFER_SETDESTROYFUNC;
		if (BufferLen) ::memcpy(Buffer,str.GetBufferConst(),BufferLen*sizeof(SHVWChar));
		Buffer[BufferLen] = '\0';
		BufferLen++;
	}
	return *this;
}
SHVString16& SHVString16::operator=(const SHVStringBuffer16& str)
{
	BUFFER_DESTROY(Buffer);

	BUFFER_MOVE(((SHVStringBuffer16*)&str)->Buffer,((SHVStringBuffer16*)&str)->DestroyFunc);
	
	BufferLen = (Buffer ? GetLength() + 1 : 0); // we assume the buffer has the length og the string, + the '\0' termination
	return *this;
}
SHVString16& SHVString16::operator+=(const SHVString16C& str)
{
	if (!str.IsNull())
		AddChars(str.GetBufferConst(), str.GetLength());
	return *this;
}
SHVStringBuffer16 operator+(const SHVString16C& str1, const SHVString16C& str2)
{
SHVString16 s;
size_t len = str1.GetLength() + str2.GetLength() + 1;

	s.SetBufferSize(len);
	s.GetBuffer()[0] = '\0';

	if (!str1.IsNull())
		SHVString16C::StrCat(s.GetBuffer(), str1.GetBufferConst());
	if (!str2.IsNull())
		SHVString16C::StrCat(s.GetBuffer(), str2.GetBufferConst());
	return s.ReleaseBuffer();
}
