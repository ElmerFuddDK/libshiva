#ifndef __SHIVA_UTILS_STRING8_H
#define __SHIVA_UTILS_STRING8_H

#include "shvstringc.h"



//-=========================================================================================================
/// SHVString8 class - Modifiable string
/**
 * This string can be modified.
 \see SHVString8C
 */

class SHVAPI SHVString8 : public SHVString8C
{
public:


	// OS specific copy functions
#ifdef __SHIVA_EPOC
	static SHVStringBuffer8 Copy(const TPtrC8& aPtr);
#endif


	// constructors
	SHVString8();
	SHVString8(const SHVString8C& str);
	SHVString8(const SHVString8& str); // copy constructor
	SHVString8(const SHVStringBuffer8& str);
	~SHVString8();


	// Buffer handling
	SHVStringBuffer8 ReleaseBuffer();
	inline SHVChar* GetBuffer();
	inline SHVChar* SetBufferSize(size_t len);
	inline size_t   GetBufferLen();
	void SetToNull();


	// case functions
	void MakeUpper();
	void MakeLower();
	inline SHVStringBuffer8 ToUpper();
	inline SHVStringBuffer8 ToLower();


	// Buffer modification
	void AddChars(const SHVChar* chars, size_t len);
	void TrimLeft(const SHVChar* trimString = NULL);
	void TrimRight(const SHVChar* trimString = NULL);
	void Trim();
	void Replace(const SHVString8C& search, const SHVString8C& repl);
	void Format(const SHVChar* s, ...);
	void FormatList(const SHVChar* s, SHVVA_LIST args);


	// operators
	SHVChar& operator[](size_t i);
	SHVString8& operator=(const SHVString8C& str);
	SHVString8& operator=(const SHVStringBuffer8& str);
	SHVString8& operator+=(const SHVString8C& str);
	inline SHVString8& operator=(const SHVString8& str);
	inline SHVString8& operator+=(const SHVString8& str);


private:
friend class SHVString8C;
#ifndef __SHVSTRING_EXCLUDE_UNICODE
friend class SHVStringUTF8;
#endif
	
	///\cond INTERNAL
	void AllocBuffer(size_t size);
	size_t BufferLen;
#ifdef __SHVSTRING_HEAPPROTECT
	SHVStr8_DestroyBuffer DestroyFunc;
#endif
	///\endcond
};



// ============================================= implementation ============================================= //

// Buffer handling
SHVChar* SHVString8::GetBuffer() { return Buffer; }
SHVChar* SHVString8::SetBufferSize(size_t len) { AllocBuffer(len); return Buffer; }
size_t   SHVString8::GetBufferLen() { return BufferLen; }  ///< returns length of buffer in characters, including null termination

SHVStringBuffer8 SHVString8::ToUpper() {SHVString8 str(*this); str.MakeUpper(); return str.ReleaseBuffer(); }
SHVStringBuffer8 SHVString8::ToLower() {SHVString8 str(*this); str.MakeLower(); return str.ReleaseBuffer(); }

SHVString8& SHVString8::operator=(const SHVString8& str)   { return *this = (*(const SHVString8C*)&str);  }
SHVString8& SHVString8::operator+=(const SHVString8& str)  { return *this += (*(const SHVString8C*)&str); }

#endif
