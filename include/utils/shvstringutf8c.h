#ifndef __SHIVA_UTILS_STRINGUTF8C_H
#define __SHIVA_UTILS_STRINGUTF8C_H

#ifdef __SHIVA_EPOC
# include <e32std.h>
# include <e32des16.h>
#endif



// forward declares
class SHVStringUTF8C;
class SHVStringUTF8;
class SHVStringUTF8CRef;
class SHVStringBufferUTF8;
#include "shvstringc.h"
#include "shvstring16c.h"



#ifdef __SHVSTRING_HEAPPROTECT
typedef void (*SHVStrUTF8_DestroyBuffer)(SHVChar*);
#endif



//-=========================================================================================================
/// SHVStringUTF8C class - Constant string
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

class SHVAPI SHVStringUTF8C
{
public:


	// constructor
	inline SHVStringUTF8C(const SHVChar* buffer);


	// Comparison
	bool operator==(const SHVStringUTF8C& str) const;
	bool operator!=(const SHVStringUTF8C& str) const;
	bool operator<(const SHVStringUTF8C& str) const;
	bool operator<=(const SHVStringUTF8C& str) const;
	bool operator>(const SHVStringUTF8C& str) const;
	bool operator>=(const SHVStringUTF8C& str) const;
	short Compare(const SHVStringUTF8C& str) const;
	short CompareNoCase(const SHVStringUTF8C& str) const;
	friend SHVStringBufferUTF8 SHVAPI operator+(const SHVStringUTF8C& str1, const SHVStringUTF8C& str2);


	// Access/Conversion functions
	inline const SHVChar* GetBufferConst() const;
	const SHVChar* GetSafeBuffer() const; ///< will return the real buffer, or "" if the string is null
	long ToLong(SHVChar** endChar = NULL) const;
	SHVInt64Val ToInt64(SHVChar** endChar = NULL) const;
	double ToDouble(SHVChar** endChar = NULL) const;
	static SHVStringBufferUTF8 LongToString(long val);
	static SHVStringBufferUTF8 Int64ToString(SHVInt64Val val);
	static SHVStringBufferUTF8 DoubleToString(double val);

	static size_t CalculateUTF8Len8(const SHVChar* str);
	static size_t CalculateUTF8Len16(const SHVWChar* str);

	SHVStringBuffer8 ToStr8() const;
	SHVStringBuffer16 ToStr16() const;
	inline SHVStringBuffer ToStrT() const;
	bool ConvertBufferToChar(SHVChar* buffer, size_t len) const;
	bool ConvertBufferToWChar(SHVWChar* buffer, size_t len) const;

#ifdef __SHIVA_EPOC
	inline TPtrC8 ToPtr() const;
#endif


	// buffer checks
	inline bool IsNull() const;
	inline bool IsEmpty() const;
	size_t GetLength() const;
	operator SHVHashValue() const; ///< hashing function


	// buffer operations
	SHVStringBufferUTF8 Right(size_t len) const;
	SHVStringBufferUTF8 Left(size_t len) const;
	SHVStringBufferUTF8 Mid(size_t first,size_t length = SIZE_T_MAX) const;
	long Find(const SHVStringUTF8C& str,long offset=0) const;
	long ReverseFind(const SHVStringUTF8C& str) const;
	SHVStringBufferUTF8 Tokenize(const SHVString8C& tokens, size_t& pos) const;


	// convenience functions for easy portability
	static long   StrToL(const SHVChar* str, SHVChar** ptr, int base = 10); ///< only works for base10 on some platforms for now
	static SHVInt64Val StrToInt64(const SHVChar* str, SHVChar** ptr, int base = 10); ///< only works for base10 on some platforms for now
	static double StrToDouble(const SHVChar* str, SHVChar** ptr);
	static size_t StrLen(const SHVChar* str);
	static size_t SizeOfChars(const char* str, size_t chars = SIZE_T_MAX);
	static size_t SizeOfCharsReverse(const char* strStart, size_t chars = SIZE_T_MAX);
	static int    StrCmp(const SHVChar* str1,const SHVChar* str2);
	static int    StrCaseCmp(const SHVChar* str1,const SHVChar* str2);
	static SHVChar* StrCat(SHVChar* dest, const SHVChar* source);
	static SHVStringBufferUTF8 Format(const SHVChar* str, ...);
	static SHVStringBufferUTF8 FormatList(const SHVChar* str, SHVVA_LIST args);


protected:
friend class SHVStringUTF8;
friend class SHVStringBufferUTF8;
	///\cond INTERNAL
	inline SHVStringUTF8C() {}
	SHVChar* Buffer;
	///\endcond
};



//-=========================================================================================================
/// SHVStringUTF8CRef class - reference counting constant string
/**
 * Used where reference counting strings are preferrable
 \see SHVStringUTF8C
 */
class SHVAPI SHVStringUTF8CRef : public SHVStringUTF8C
{
public:

	// constructor
	inline SHVStringUTF8CRef();
	SHVStringUTF8CRef(const SHVChar* buffer);
	SHVStringUTF8CRef(const SHVStringUTF8CRef& buffer);
	~SHVStringUTF8CRef();


	// operators
	SHVStringUTF8CRef& operator=(const SHVStringUTF8CRef& str);
	inline bool operator==(const SHVChar* str) const;
	inline bool operator!=(const SHVChar* str) const;
	inline bool operator<(const SHVChar* str) const;
	inline bool operator<=(const SHVChar* str) const;
	inline bool operator>(const SHVChar* str) const;
	inline bool operator>=(const SHVChar* str) const;
	inline bool operator==(const SHVStringUTF8C str) const;
	inline bool operator!=(const SHVStringUTF8C str) const;
	inline bool operator<(const SHVStringUTF8C str) const;
	inline bool operator<=(const SHVStringUTF8C str) const;
	inline bool operator>(const SHVStringUTF8C str) const;
	inline bool operator>=(const SHVStringUTF8C str) const;


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
/// SHVStringBufferUTF8 class - for buffer transferring
/**
 * Used for transfering string buffers.
 \see SHVStringUTF8C
 */

class SHVAPI SHVStringBufferUTF8 : public SHVStringUTF8C
{
friend class SHVStringUTF8;
friend class SHVStringUTF8C;
friend class SHVString8C;
public:


	// constructor
	SHVStringBufferUTF8(const SHVStringBufferUTF8& buffer);
	~SHVStringBufferUTF8();
	static SHVStringBufferUTF8 Encapsulate(SHVChar* buffer);


	// assignment operator
	SHVStringBufferUTF8& operator=(SHVStringBufferUTF8& buffer);


private:
	///\cond INTERNAL
#ifdef __SHVSTRING_HEAPPROTECT
	SHVStrUTF8_DestroyBuffer DestroyFunc;
#endif
	inline SHVStringBufferUTF8();
	///\endcond
};



// ====================================== implementation - SHVStringC ======================================= //

SHVStringUTF8C::SHVStringUTF8C(const SHVChar* buffer) { Buffer = (SHVChar*)buffer; }
const SHVChar* SHVStringUTF8C::GetBufferConst() const { return Buffer; }
bool SHVStringUTF8C::IsNull() const { return Buffer == NULL; }
bool SHVStringUTF8C::IsEmpty() const { return Buffer == NULL || *Buffer == 0; }
#ifdef __SHIVA_EPOC
TPtrC8 SHVStringUTF8C::ToPtr() const { return TPtrC8((TUint8*)Buffer,GetLength()); }
#endif

/*************************************
 * ToStrT
 *************************************/
SHVStringBuffer SHVStringUTF8C::ToStrT() const
{
#ifdef UNICODE
	return ToStr16();
#else
	return ToStr8();
#endif
}


// ===================================== implementation - SHVStringCRef ===================================== //
SHVStringUTF8CRef::SHVStringUTF8CRef() { Buffer = NULL; }
bool SHVStringUTF8CRef::operator==(const SHVChar* str) const { return SHVStringUTF8C::operator==(str); }
bool SHVStringUTF8CRef::operator!=(const SHVChar* str) const { return SHVStringUTF8C::operator!=(str); }
bool SHVStringUTF8CRef::operator<(const SHVChar* str) const  { return SHVStringUTF8C::operator<(str); }
bool SHVStringUTF8CRef::operator<=(const SHVChar* str) const { return SHVStringUTF8C::operator<=(str); }
bool SHVStringUTF8CRef::operator>(const SHVChar* str) const  { return SHVStringUTF8C::operator>(str); }
bool SHVStringUTF8CRef::operator>=(const SHVChar* str) const { return SHVStringUTF8C::operator>=(str); }
bool SHVStringUTF8CRef::operator==(const SHVStringUTF8C str) const { return SHVStringUTF8C::operator==(str); }
bool SHVStringUTF8CRef::operator!=(const SHVStringUTF8C str) const { return SHVStringUTF8C::operator!=(str); }
bool SHVStringUTF8CRef::operator<(const SHVStringUTF8C str) const  { return SHVStringUTF8C::operator<(str); }
bool SHVStringUTF8CRef::operator<=(const SHVStringUTF8C str) const { return SHVStringUTF8C::operator<=(str); }
bool SHVStringUTF8CRef::operator>(const SHVStringUTF8C str) const  { return SHVStringUTF8C::operator>(str); }
bool SHVStringUTF8CRef::operator>=(const SHVStringUTF8C str) const { return SHVStringUTF8C::operator>=(str); }


// ==================================== implementation - SHVStringBuffer ==================================== //
///\cond INTERNAL
SHVStringBufferUTF8::SHVStringBufferUTF8() { Buffer = NULL; }
///\endcond

#endif
