#ifndef __SHIVA_UTILS_STRING16C_H
#define __SHIVA_UTILS_STRING16C_H

#ifdef __SHIVA_EPOC
# include <e32std.h>
# include <e32des16.h>
#endif

#include "shvhashvalue.h"
#include "shvrefobject.h"



// forward declares
class SHVString16C;
class SHVString16;
class SHVString16CRef;
class SHVStringBuffer8;
class SHVStringBufferUTF8;
class SHVStringBuffer16;
#ifndef __SHIVA_UTILS_STRINGC_H
class SHVString8C;
class SHVString8;
class SHVString8CRef;
#endif



// defines
#if defined(UNICODE)
typedef SHVString16C SHVStringC;
typedef SHVString16  SHVString;
typedef SHVString16CRef SHVStringCRef;
typedef SHVStringBuffer16 SHVStringBuffer;
# define _SHVS8(x)  SHVStringC(_S(x)).ToStr8()
# define _SHVS16(x) SHVString16C(_S(x))
///\cond INTERNAL
# ifdef __SHIVA_POSIX_LINUX
#  error "Linux is currently UCS4 only - doesn't work with shiva in unicode"
# endif
# ifdef _T
#  undef _T
# endif
# define _S(x)  (const SHVWChar*)L##x
# define _SD(x) _S(x)
# define _T(x)  L##x
# define _TD(x) _T(x)
///\endcond
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
	int  Compare(const SHVString16C& str) const;
	int  CompareNoCase(const SHVString16C& str) const;
	friend SHVStringBuffer16 SHVAPI operator+(const SHVString16C& str1, const SHVString16C& str2);


	// Access/Conversion functions
	inline const SHVWChar* GetBufferConst() const;
	const SHVWChar* GetSafeBuffer() const; ///< will return the real buffer, or "" if the string is null
	long ToLong(SHVWChar** endChar = NULL) const;
	SHVInt64Val ToInt64(SHVWChar** endChar = NULL) const;
	double ToDouble(SHVWChar** endChar = NULL) const;
	static SHVStringBuffer16 LongToString(long val);
	static SHVStringBuffer16 Int64ToString(SHVInt64Val val);
	static SHVStringBuffer16 DoubleToString(double val);
	SHVStringBuffer8 ToStr8() const;
	SHVStringBufferUTF8 ToStrUTF8() const;
	SHVStringBuffer16 ToStr16() const;
	inline SHVStringBuffer ToStrT() const;
	bool ConvertBufferToChar(SHVChar* buffer, size_t len) const;
	bool ConvertBufferToUTF8(SHVChar* buffer, size_t& len) const;
#ifdef __SHIVA_EPOC
	inline TPtrC16 ToPtr() const;
#endif


	// buffer checks
	inline bool IsNull() const;
	inline bool IsEmpty() const;
	size_t GetLength() const;
	operator SHVHashValue() const; ///< hashing function


	// buffer operations
	SHVStringBuffer16 Right(size_t len) const;
	SHVStringBuffer16 Left(size_t len) const;
	SHVStringBuffer16 Mid(size_t first,size_t length = SIZE_T_MAX) const;
	long LocateChar(SHVWChar ch) const;
	long Find(const SHVString16C& str,long offset=0) const;
	long ReverseFind(const SHVString16C& str) const;
	SHVStringBuffer16 Tokenize(const SHVString16C& tokens, size_t& pos) const;


	// convenience functions for easy portability
	static long   StrToL(const SHVWChar* str, SHVWChar** ptr, int base = 10); ///< only works for base10 on some platforms for now
	static SHVInt64Val StrToInt64(const SHVWChar* str, SHVWChar** ptr, int base = 10); ///< only works for base10 on some platforms for now
	static double StrToDouble(const SHVWChar* str, SHVWChar** ptr);
	static size_t StrLen(const SHVWChar* str);
	static int    StrCmp(const SHVWChar* str1,const SHVWChar* str2);
	static int    StrCaseCmp(const SHVWChar* str1,const SHVWChar* str2);
	static SHVWChar* StrCat(SHVWChar* dest, const SHVWChar* source);
	static SHVStringBuffer16 Format(const SHVWChar* str, ...);
	static SHVStringBuffer16 FormatList(const SHVWChar* str, SHVVA_LIST args);

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
bool SHVString16C::IsEmpty() const { return Buffer == NULL || *Buffer == 0; }
#ifdef __SHIVA_EPOC
TPtrC16 SHVString16C::ToPtr() const { return TPtrC16((TUint16*)Buffer,GetLength()); }
#endif
#ifdef __SHIVASTR_WCHAR_T
const wchar_t* SHVString16C::GetWcharConst() const { return (const wchar_t*)Buffer; }
const wchar_t* SHVString16C::GetSafeWchar() const { return (const wchar_t*)GetSafeBuffer(); }
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

#include "shvstringc.h"

