#ifndef __SHIVA_UTILS_STRING16C_H
#define __SHIVA_UTILS_STRING16C_H

#include "shvstringdefaults.h"

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
	inline SHVString16C(const SHVString16C& str);
#ifdef __SHIVA_WIN32
	inline static const SHVString16C FromWin32(const WCHAR* buffer);
#endif


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
#if defined(__SHIVA_WIN32)
	inline const WCHAR* GetBufferConstWin32() const;
	inline const WCHAR* GetSafeBufferWin32() const;
#endif
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
	inline const SHVStringBuffer8 AsStr8C() const;
	inline const SHVString16C AsStr16C() const;
	inline const SHVStringBufferUTF8 AsStrUTF8C() const;
#ifdef __SHIVA_EPOC
	inline TPtrC16 ToPtr() const;
#endif


	// buffer checks
	inline bool IsNull() const;
	inline bool IsEmpty() const;
	size_t GetLength() const;
	inline size_t GetLengthInChars() const;
	inline size_t GetLengthInBytes() const;
	operator SHVHashValue() const; ///< hashing function


	// buffer operations
	SHVStringBuffer16 Right(size_t len) const;
	SHVStringBuffer16 Left(size_t len) const;
	SHVStringBuffer16 Mid(size_t first,size_t length = SIZE_T_MAX) const;
	long LocateChar(SHVWChar ch) const;
	long Find(const SHVString16C& str,long offset=0) const;
	long ReverseFind(const SHVString16C& str) const;
	SHVStringBuffer16 Tokenize(const SHVString16C& tokens, size_t& pos) const;

	// compat functions with utf8
	inline SHVStringBuffer16 RightInChars(size_t len) const;
	inline SHVStringBuffer16 LeftInChars(size_t len) const;
	inline SHVStringBuffer16 MidInChars(size_t first,size_t length = SIZE_T_MAX) const;
	inline long FindInChars(const SHVString16C& str,long offset=0) const;
	inline long ReverseFindInChars(const SHVString16C& str) const;


	// convenience functions for easy portability
	static long   StrToL(const SHVWChar* str, SHVWChar** ptr, int base = 10); ///< only works for base10 on some platforms for now
	static SHVInt64Val StrToInt64(const SHVWChar* str, SHVWChar** ptr, int base = 10); ///< only works for base10 on some platforms for now
	static double StrToDouble(const SHVWChar* str, SHVWChar** ptr);
	static size_t StrLen(const SHVWChar* str);
	inline static size_t StrLenInChars(const SHVWChar* str);
	inline static size_t StrLenInBytes(const SHVWChar* str);
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
SHVString16C::SHVString16C(const SHVString16C& str) { Buffer = str.Buffer; }
const SHVWChar* SHVString16C::GetBufferConst() const { return Buffer; }
#if defined(__SHIVA_WIN32)
const SHVString16C SHVString16C::FromWin32(const WCHAR* buffer) { return SHVString16C((SHVWChar*)buffer); }
const WCHAR* SHVString16C::GetBufferConstWin32() const { return (const WCHAR*)Buffer; }
const WCHAR* SHVString16C::GetSafeBufferWin32() const { return (const WCHAR*)GetSafeBuffer(); }
#endif
bool SHVString16C::IsNull() const { return Buffer == NULL; }
bool SHVString16C::IsEmpty() const { return Buffer == NULL || *Buffer == 0; }
size_t SHVString16C::GetLengthInChars() const { return GetLength(); }
size_t SHVString16C::GetLengthInBytes() const { return GetLength()*sizeof(SHVWChar); }
#ifdef __SHIVA_EPOC
TPtrC16 SHVString16C::ToPtr() const { return TPtrC16((TUint16*)Buffer,GetLength()); }
#endif
#ifdef __SHIVASTR_WCHAR_T
const wchar_t* SHVString16C::GetWcharConst() const { return (const wchar_t*)Buffer; }
const wchar_t* SHVString16C::GetSafeWchar() const { return (const wchar_t*)GetSafeBuffer(); }
#endif
size_t SHVString16C::StrLenInChars(const SHVWChar* str) { return StrLen(str); }
size_t SHVString16C::StrLenInBytes(const SHVWChar* str) { return StrLen(str)*sizeof(SHVWChar); }

SHVStringBuffer16 SHVString16C::RightInChars(size_t len) const { return Right(len); }
SHVStringBuffer16 SHVString16C::LeftInChars(size_t len) const { return Left(len); }
SHVStringBuffer16 SHVString16C::MidInChars(size_t first,size_t length) const { return Mid(first,length); }
long SHVString16C::FindInChars(const SHVString16C& str,long offset) const { return Find(str,offset); }
long SHVString16C::ReverseFindInChars(const SHVString16C& str) const { return ReverseFind(str); }


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
#ifndef __SHIVA_UTILS_STRING_H
# include "shvstring.h"
#endif
