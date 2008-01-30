#ifndef __SHIVA_UTILS_STRING16C_H
#define __SHIVA_UTILS_STRING16C_H

#ifdef __SHIVA_EPOC
# include <e32std.h>
# include <e32des16.h>
#endif



// forward declares
class SHVString16C;
class SHVString16;
class SHVString16CRef;
class SHVStringBuffer8;
class SHVStringBuffer16;
#include "shvstringc.h"



// defines
#if defined(UNICODE)
typedef SHVString16C SHVStringC;
typedef SHVString16  SHVString;
typedef SHVString16CRef SHVStringCRef;
typedef SHVStringBuffer16 SHVStringBuffer;
# define SHVTChar SHVWChar
# define SHVForeignChar SHVChar
# define _SHVS8(x)  SHVStringC(_T(x)).ToStr8()
# define _SHVS16(x) _T(x)
///\cond INTERNAL
# ifdef __SHIVA_LINUX
#  error "Linux is currently UCS4 only - doesn't work with shiva in unicode"
# elif !defined(_T)
#  define _T(x) (const SHVWChar*) L##x
# endif
///\endcond
#endif
#ifndef _SHVWSTR
# define _SHVWSTR(x) (const SHVWChar*) L##x
#endif

#ifdef __SHVSTRING_HEAPPROTECT
typedef void (*SHVStr16_DestroyBuffer)(SHVWChar*);
#endif



//-=========================================================================================================
/// SHVString16C class - Constant string
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

class SHVAPI SHVString16C
{
public:


	// constructor
	inline SHVString16C(const SHVWChar* buffer);


	// Comparison
	bool operator==(const SHVString16C& str) const;
	bool operator!=(const SHVString16C& str) const;
	bool operator<(const SHVString16C& str) const;
	bool operator<=(const SHVString16C& str) const;
	bool operator>(const SHVString16C& str) const;
	bool operator>=(const SHVString16C& str) const;
	short Compare(const SHVString16C& str) const;
	short CompareNoCase(const SHVString16C& str) const;
	friend SHVStringBuffer16 operator+(const SHVString16C& str1, const SHVString16C& str2);


	// Access/Conversion functions
	inline const SHVWChar* GetBufferConst() const;
	const SHVWChar* GetSafeBuffer() const; ///< will return the real buffer, or "" if the string is null
	long ToLong(SHVWChar** endChar = NULL) const;
	static SHVStringBuffer16 LongToString(long val);
#ifdef __SHIVA_UTILS_STRINGC_H
	SHVStringBuffer8 ToStr8() const;
	SHVStringBuffer ToStrT() const;
	bool ConvertBufferToChar(SHVChar* buffer, size_t len) const;
	static SHVStringBuffer16 FromUTF8(const char* utf8Str);
#endif
#ifdef __SHIVA_EPOC
	inline TPtrC16 ToPtr() const;
#endif


	// buffer checks
	inline bool IsNull() const;
	size_t GetLength() const;
	operator size_t() const; ///< hashing function


	// buffer operations
	SHVStringBuffer16 Right(size_t len) const;
	SHVStringBuffer16 Left(size_t len) const;
	SHVStringBuffer16 Mid(size_t first,size_t length = SIZE_T_MAX) const;
	long LocateChar(SHVWChar ch) const;
	long Find(const SHVString16C& str,long offset=0) const;
	long ReverseFind(const SHVString16C& str) const;
	SHVStringBuffer16 Tokenize(const SHVString16C& tokens, size_t& pos) const;


	// convenience functions for easy portability
	static long   StrToL(const SHVWChar* str, SHVWChar** ptr, int base); ///< only works for base10 on some platforms for now
	static size_t StrLen(const SHVWChar* str);
	static int    StrCmp(const SHVWChar* str1,const SHVWChar* str2);
	static int    StrCaseCmp(const SHVWChar* str1,const SHVWChar* str2);
	static SHVWChar* StrCat(SHVWChar* dest, const SHVWChar* source);


protected:
	///\cond INTERNAL
	inline SHVString16C() {}
	SHVWChar* Buffer;
	size_t CalcFormatBufferSize(const SHVWChar* s, SHVVA_LIST args);
	///\endcond
};



//-=========================================================================================================
/// SHVString16CRef class - reference counting constant string
/**
 * Used where reference counting strings are preferrable
 \see SHVString16C
 */
class SHVAPI SHVString16CRef : public SHVString16C
{
public:

	// constructor
	inline SHVString16CRef();
	SHVString16CRef(const SHVWChar* buffer);
	SHVString16CRef(const SHVString16CRef& buffer);
	~SHVString16CRef();


	// operators
	SHVString16CRef& operator=(const SHVString16CRef& str);
	inline bool operator==(const SHVWChar* str) const;
	inline bool operator!=(const SHVWChar* str) const;
	inline bool operator<(const SHVWChar* str) const;
	inline bool operator<=(const SHVWChar* str) const;
	inline bool operator>(const SHVWChar* str) const;
	inline bool operator>=(const SHVWChar* str) const;
	inline bool operator==(const SHVString16C str) const;
	inline bool operator!=(const SHVString16C str) const;
	inline bool operator<(const SHVString16C str) const;
	inline bool operator<=(const SHVString16C str) const;
	inline bool operator>(const SHVString16C str) const;
	inline bool operator>=(const SHVString16C str) const;


private:
	///\cond INTERNAL
	struct Header
	{
		int RefCount;
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
/// SHVStringBuffer16 class - for buffer transferring
/**
 * Used for transfering string buffers.
 \see SHVString16C
 */

class SHVAPI SHVStringBuffer16 : public SHVString16C
{
friend class SHVString16;
friend class SHVString16C;
friend class SHVString8C;
public:


	// constructor
	SHVStringBuffer16(const SHVStringBuffer16& buffer);
	~SHVStringBuffer16();
	static SHVStringBuffer16 Encapsulate(SHVWChar* buffer);


	// assignment operator
	SHVStringBuffer16& operator=(SHVStringBuffer16& buffer);


private:
	///\cond INTERNAL
#ifdef __SHVSTRING_HEAPPROTECT
	SHVStr16_DestroyBuffer DestroyFunc;
#endif
	inline SHVStringBuffer16();
	///\endcond
};



// ====================================== implementation - SHVStringC ======================================= //

SHVString16C::SHVString16C(const SHVWChar* buffer) { Buffer = (SHVWChar*)buffer; }
const SHVWChar* SHVString16C::GetBufferConst() const { return Buffer; }
bool SHVString16C::IsNull() const { return Buffer == NULL; }
#ifdef __SHIVA_EPOC
TPtrC16 SHVString16C::ToPtr() const { return TPtrC16((TUint16*)Buffer,GetLength()); }
#endif


// ===================================== implementation - SHVStringCRef ===================================== //
SHVString16CRef::SHVString16CRef() { Buffer = NULL; }
bool SHVString16CRef::operator==(const SHVWChar* str) const { return SHVString16C::operator==(str); }
bool SHVString16CRef::operator!=(const SHVWChar* str) const { return SHVString16C::operator!=(str); }
bool SHVString16CRef::operator<(const SHVWChar* str) const  { return SHVString16C::operator<(str); }
bool SHVString16CRef::operator<=(const SHVWChar* str) const { return SHVString16C::operator<=(str); }
bool SHVString16CRef::operator>(const SHVWChar* str) const  { return SHVString16C::operator>(str); }
bool SHVString16CRef::operator>=(const SHVWChar* str) const { return SHVString16C::operator>=(str); }
bool SHVString16CRef::operator==(const SHVString16C str) const { return SHVString16C::operator==(str); }
bool SHVString16CRef::operator!=(const SHVString16C str) const { return SHVString16C::operator!=(str); }
bool SHVString16CRef::operator<(const SHVString16C str) const  { return SHVString16C::operator<(str); }
bool SHVString16CRef::operator<=(const SHVString16C str) const { return SHVString16C::operator<=(str); }
bool SHVString16CRef::operator>(const SHVString16C str) const  { return SHVString16C::operator>(str); }
bool SHVString16CRef::operator>=(const SHVString16C str) const { return SHVString16C::operator>=(str); }


// ==================================== implementation - SHVStringBuffer ==================================== //
///\cond INTERNAL
SHVStringBuffer16::SHVStringBuffer16() { Buffer = NULL; }
///\endcond

#endif
