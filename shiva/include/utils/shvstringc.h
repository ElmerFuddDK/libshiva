#ifndef __SHIVA_UTILS_STRINGC_H
#define __SHIVA_UTILS_STRINGC_H

#include "shvhashvalue.h"
#include "shvrefobject.h"

#if defined(__SHIVA_WIN32)
# define __SHVSTRING_HEAPPROTECT
#endif

// Typedefs and forward declares
class SHVString8C;
class SHVString8;
class SHVString8CRef;
class SHVStringBuffer8;

#ifndef UNICODE // other define is in shvstring16c.h
typedef SHVString8C  SHVStringC;
typedef SHVString8   SHVString;
typedef SHVString8CRef SHVStringCRef;
typedef SHVStringBuffer8 SHVStringBuffer;
# define _SHVS8(x)  SHVStringC(x)
# ifndef __SHVSTRING_EXCLUDE_UNICODE
#  define _SHVS16(x) SHVStringC(x).ToStr16()
# endif
///\cond INTERNAL
# ifdef _T
#  undef _T
# endif
# define _S(x)  x
# define _SD(x) x
# define _T(x)  x
# define _TD(x) x
///\endcond
#endif

#ifndef __SHVSTRING_EXCLUDE_UNICODE
# include "shvstring16c.h"
#endif

#ifdef __SHVSTRING_HEAPPROTECT
typedef void (*SHVStr8_DestroyBuffer)(SHVChar*);
#endif



//-=========================================================================================================
/// SHVString8C class - Constant string
/**
 * This class contains all the constant operations of a string, thus being safe from modifying the internal
 * string buffer.\n
 * Therefore it does not own the internal string. The other 2 string classes derives from this class,
 * providing a safe interface to use. This mechanism provides a better, more compact way of handling strings
 * than that of using refcounted string buffers, or standard C strings.\n
 * The rule of thumb is you should always use the SHVStringC class for parameters, and SHVStringBuffer as
 * return types:
\code
SHVStringBuffer CopyString(const SHVStringC& str)
{
SHVString copy(str);
	return copy.ReleaseBuffer();
}
\endcode
 */

class SHVAPI SHVString8C
{
public:


	// constructor
	inline SHVString8C(const SHVChar* buffer);


	// Comparison
	bool operator==(const SHVString8C& str) const;
	bool operator!=(const SHVString8C& str) const;
	bool operator<(const SHVString8C& str) const;
	bool operator<=(const SHVString8C& str) const;
	bool operator>(const SHVString8C& str) const;
	bool operator>=(const SHVString8C& str) const;
	int  Compare(const SHVString8C& str) const;
	int  CompareNoCase(const SHVString8C& str) const;
	friend SHVStringBuffer8 SHVAPI operator+(const SHVString8C& str1, const SHVString8C& str2);


	// Access/Conversion functions
	inline const SHVChar* GetBufferConst() const;
	const SHVChar* GetSafeBuffer() const; ///< will return the real buffer, or "" if the string is null
	long ToLong(SHVChar** endChar = NULL) const;
	SHVInt64Val ToInt64(SHVChar** endChar = NULL) const;
	double ToDouble(SHVChar** endChar = NULL) const;
	static SHVStringBuffer8 LongToString(long val);
	static SHVStringBuffer8 Int64ToString(SHVInt64Val val);
	static SHVStringBuffer8 DoubleToString(double val);
#ifndef __SHVSTRING_EXCLUDE_UNICODE
	SHVStringBuffer16 ToStr16() const;
	SHVStringBufferUTF8 ToStrUTF8() const;
	bool ConvertBufferToWChar(SHVWChar* buffer, size_t len) const;
	bool ConvertBufferToUTF8(SHVChar* buffer, size_t& len) const;
#endif
	SHVStringBuffer8 ToStr8() const;
	inline SHVStringBuffer ToStrT() const;
#ifdef __SHIVA_EPOC
	inline TPtrC8 ToPtr() const;
#endif


	// buffer checks
	inline bool IsNull() const;
	inline bool IsEmpty() const;
	size_t GetLength() const;
	inline size_t GetSizeInChars() const;
	inline size_t GetSizeInBytes() const;
	operator SHVHashValue() const; ///< hashing function


	// buffer operations
	SHVStringBuffer8 Right(size_t len) const;
	SHVStringBuffer8 Left(size_t len) const;
	SHVStringBuffer8 Mid(size_t first,size_t length = SIZE_T_MAX) const;
	long LocateChar(SHVChar ch) const;
	long Find(const SHVString8C& str,long offset=0) const;
	long ReverseFind(const SHVString8C& str) const;
	SHVStringBuffer8 Tokenize(const SHVString8C& tokens, size_t& pos) const;


	// convenience functions for easy portability
	static long   StrToL(const SHVChar* str, SHVChar** ptr, int base = 10); ///< only works for base10 on some platforms for now
	static SHVInt64Val StrToInt64(const SHVChar* str, SHVChar** ptr, int base = 10); ///< only works for base10 on some platforms for now
	static double StrToDouble(const SHVChar* str, SHVChar** ptr);
	static size_t StrLen(const SHVChar* str);
	inline static size_t StrSizeInChars(const SHVChar* str);
	inline static size_t StrSizeInBytes(const SHVChar* str);
	static int    StrCmp(const SHVChar* str1,const SHVChar* str2);
	static int    StrCaseCmp(const SHVChar* str1,const SHVChar* str2);
	static SHVChar* StrCat(SHVChar* dest, const SHVChar* source);
	static SHVStringBuffer8 Format(const SHVChar* str, ...);
	static SHVStringBuffer8 FormatList(const SHVChar* str, SHVVA_LIST args);


protected:
	///\cond INTERNAL
	inline SHVString8C() {}
	SHVChar* Buffer;
	size_t CalcFormatBufferSize(const SHVChar* s, SHVVA_LIST args);
	///\endcond
};



//-=========================================================================================================
/// SHVString8CRef class - reference counting constant string
/**
 * Used where reference counting strings are preferrable
 \see SHVString8C
 */
class SHVAPI SHVString8CRef : public SHVString8C
{
public:

	// constructor
	inline SHVString8CRef();
	SHVString8CRef(const SHVChar* buffer);
	SHVString8CRef(const SHVString8CRef& buffer);
	~SHVString8CRef();


	// operators
	SHVString8CRef& operator=(const SHVString8CRef& str);
	inline bool operator==(const SHVChar* str) const;
	inline bool operator!=(const SHVChar* str) const;
	inline bool operator<(const SHVChar* str) const;
	inline bool operator<=(const SHVChar* str) const;
	inline bool operator>(const SHVChar* str) const;
	inline bool operator>=(const SHVChar* str) const;
	inline bool operator==(const SHVString8C str) const;
	inline bool operator!=(const SHVString8C str) const;
	inline bool operator<(const SHVString8C str) const;
	inline bool operator<=(const SHVString8C str) const;
	inline bool operator>(const SHVString8C str) const;
	inline bool operator>=(const SHVString8C str) const;


private:
	///\cond INTERNAL
	struct Header
	{
		SHVRefObject::RefData RefCount;
#ifdef __SHVSTRING_HEAPPROTECT
		typedef void (*DestroyFunc)(Header*);
		DestroyFunc DestroyBuffer;
#endif
	};

#ifdef __SHVSTRING_HEAPPROTECT
	static void DestroyBuffer(Header* buffer);
#endif
	///\endcond
};



//-=========================================================================================================
/// SHVStringBuffer8 class - for buffer transferring
/**
 * Used for transfering string buffers.
 \see SHVString8C
 */

class SHVAPI SHVStringBuffer8 : public SHVString8C
{
friend class SHVString8;
friend class SHVString8C;
friend class SHVString16C;
public:


	// constructor
	SHVStringBuffer8(const SHVStringBuffer8& buffer);
	~SHVStringBuffer8();
	static SHVStringBuffer8 Encapsulate(SHVChar* buffer);


	// assignment operator
	SHVStringBuffer8& operator=(SHVStringBuffer8& buffer);


private:
	///\cond INTERNAL
#ifdef __SHVSTRING_HEAPPROTECT
	SHVStr8_DestroyBuffer DestroyFunc;
#endif
	inline SHVStringBuffer8();
	///\endcond
};



// ====================================== implementation - SHVStringC ======================================= //

SHVString8C::SHVString8C(const SHVChar* buffer) { Buffer = (SHVChar*)buffer; }
const SHVChar* SHVString8C::GetBufferConst() const { return Buffer; }
bool SHVString8C::IsNull() const { return Buffer == NULL; }
bool SHVString8C::IsEmpty() const { return Buffer == NULL || *Buffer == 0; }
size_t SHVString8C::GetSizeInChars() const { return GetLength(); }
size_t SHVString8C::GetSizeInBytes() const { return GetLength(); }
#ifdef __SHIVA_EPOC
TPtrC8 SHVString8C::ToPtr() const { return TPtrC8((TUint8*)Buffer,GetLength()); }
#endif
size_t SHVString8C::StrSizeInChars(const SHVChar* str) { return StrLen(str); }
size_t SHVString8C::StrSizeInBytes(const SHVChar* str) { return StrLen(str); }

/*************************************
 * ToStrT
 *************************************/
SHVStringBuffer SHVString8C::ToStrT() const
{
#ifdef UNICODE
	return ToStr16();
#else
	return ToStr8();
#endif
}

// ==================================== implementation - SHVString16C ==================================== //

/*************************************
 * ToStrT
 *************************************/
#ifndef __SHVSTRING_EXCLUDE_UNICODE
SHVStringBuffer SHVString16C::ToStrT() const
{
#ifdef UNICODE
	return ToStr16();
#else
	return ToStr8();
#endif
}
#endif


// ===================================== implementation - SHVStringCRef ===================================== //
SHVString8CRef::SHVString8CRef() { Buffer = NULL; }
bool SHVString8CRef::operator==(const SHVChar* str) const { return SHVString8C::operator==(str); }
bool SHVString8CRef::operator!=(const SHVChar* str) const { return SHVString8C::operator!=(str); }
bool SHVString8CRef::operator<(const SHVChar* str) const  { return SHVString8C::operator<(str); }
bool SHVString8CRef::operator<=(const SHVChar* str) const { return SHVString8C::operator<=(str); }
bool SHVString8CRef::operator>(const SHVChar* str) const  { return SHVString8C::operator>(str); }
bool SHVString8CRef::operator>=(const SHVChar* str) const { return SHVString8C::operator>=(str); }
bool SHVString8CRef::operator==(const SHVString8C str) const { return SHVString8C::operator==(str); }
bool SHVString8CRef::operator!=(const SHVString8C str) const { return SHVString8C::operator!=(str); }
bool SHVString8CRef::operator<(const SHVString8C str) const  { return SHVString8C::operator<(str); }
bool SHVString8CRef::operator<=(const SHVString8C str) const { return SHVString8C::operator<=(str); }
bool SHVString8CRef::operator>(const SHVString8C str) const  { return SHVString8C::operator>(str); }
bool SHVString8CRef::operator>=(const SHVString8C str) const { return SHVString8C::operator>=(str); }


// ==================================== implementation - SHVStringBuffer ==================================== //
///\cond INTERNAL
SHVStringBuffer8::SHVStringBuffer8() { Buffer = NULL; }
///\endcond

#ifndef __SHVSTRING_EXCLUDE_UNICODE
# include "shvstringutf8c.h"
#endif

#endif
