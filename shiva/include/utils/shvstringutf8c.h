#ifndef __SHIVA_UTILS_STRINGC_H
# include "shvstringc.h"
#elif !defined(__SHIVA_UTILS_STRINGUTF8C_H)
#define __SHIVA_UTILS_STRINGUTF8C_H


// forward declares
class SHVStringUTF8C;
class SHVString8C;
class SHVStringUTF8;
class SHVStringUTF8CRef;
class SHVStringBufferUTF8;
class SHVStringBuffer8;
class SHVStringBuffer16;

#if __SHVSTRINGDEFAULT == utf8
typedef SHVStringUTF8C  SHVStringC;
typedef SHVStringUTF8   SHVString;
typedef SHVStringUTF8CRef SHVStringCRef;
typedef SHVStringBufferUTF8 SHVStringBuffer;
# define _SHVS8(x)  SHVStringC(x)
# define _SHVS16(x) SHVStringC(x).ToStr16()
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
	int  Compare(const SHVStringUTF8C& str) const;
	int  CompareNoCase(const SHVStringUTF8C& str) const;
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
	SHVStringBufferUTF8 ToStrUTF8() const;
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
	inline size_t GetSizeInChars() const;
	inline size_t GetSizeInBytes() const;
	operator SHVHashValue() const; ///< hashing function


	// buffer operations
	SHVStringBufferUTF8 Right(size_t len) const;
	SHVStringBufferUTF8 Left(size_t len) const;
	SHVStringBufferUTF8 Mid(size_t first,size_t length = SIZE_T_MAX) const;
	long LocateChar(SHVChar ch) const;
	long Find(const SHVStringUTF8C& str,long offset=0) const;
	long ReverseFind(const SHVStringUTF8C& str) const;
	SHVStringBufferUTF8 Tokenize(const SHVString8C& tokens, size_t& pos) const;


	// convenience functions for easy portability
	static long   StrToL(const SHVChar* str, SHVChar** ptr, int base = 10); ///< only works for base10 on some platforms for now
	static SHVInt64Val StrToInt64(const SHVChar* str, SHVChar** ptr, int base = 10); ///< only works for base10 on some platforms for now
	static double StrToDouble(const SHVChar* str, SHVChar** ptr);
	static size_t StrLen(const SHVChar* str);
	inline static size_t StrSizeInChars(const SHVChar* str);
	inline static size_t StrSizeInBytes(const SHVChar* str);
	static size_t SizeOfChars(const char* str, size_t chars = SIZE_T_MAX);
	static size_t SizeOfCharsReverse(const char* strStart, size_t chars = SIZE_T_MAX);
	static int    StrCmp(const SHVChar* str1,const SHVChar* str2);
	static int    StrCaseCmp(const SHVChar* str1,const SHVChar* str2);
	static SHVChar* StrCat(SHVChar* dest, const SHVChar* source);
	static SHVStringBufferUTF8 Format(const SHVChar* str, ...);
	static SHVStringBufferUTF8 FormatList(const SHVChar* str, SHVVA_LIST args);


	// For validating UTF encoding
	static size_t GetUTF8CharLen(const SHVChar ch);
	static bool IsValidUTF8(const SHVChar* str);


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

#endif
