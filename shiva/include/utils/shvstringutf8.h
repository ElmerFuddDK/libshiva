#ifndef __SHIVA_UTILS_STRINGUTF8_H
#define __SHIVA_UTILS_STRINGUTF8_H

#include "shvstringutf8c.h"



//-=========================================================================================================
/// SHVStringUTF8 class - Modifiable string
/**
 * This string can be modified.
 \see SHVStringUTF8C
 */

class SHVAPI SHVStringUTF8 : public SHVStringUTF8C
{
friend class SHVStringUTF8C;
public:


	// OS specific copy functions
#ifdef __SHIVA_EPOC
	static SHVStringBufferUTF8 Copy(const TPtrC8& aPtr);
#endif


	// constructors
	SHVStringUTF8();
	SHVStringUTF8(const SHVStringUTF8C& str);
	SHVStringUTF8(const SHVStringUTF8& str); // copy constructor
	SHVStringUTF8(const SHVStringBufferUTF8& str);
	~SHVStringUTF8();


	// Buffer handling
	SHVStringBufferUTF8 ReleaseBuffer();
	inline SHVChar* GetBuffer();
	inline SHVChar* SetBufferSize(size_t len);
	inline size_t   GetBufferLen();
	void SetToNull();


	// case functions
	void MakeUpper();
	void MakeLower();
	inline SHVStringBufferUTF8 ToUpper();
	inline SHVStringBufferUTF8 ToLower();


	// Buffer modification
	void AddChars(const SHVChar* chars, size_t len);
	void AddCharsInChars(const SHVChar* chars, size_t len);
	void TrimLeft(const SHVChar* trimString = NULL);
	void TrimRight(const SHVChar* trimString = NULL);
	void Trim();
	void Replace(const SHVStringUTF8C& search, const SHVStringUTF8C& repl);
	void Format(const SHVChar* s, ...);
	void FormatList(const SHVChar* s, SHVVA_LIST args);


	// operators
	SHVStringUTF8& operator=(const SHVStringUTF8C& str);
	SHVStringUTF8& operator=(const SHVStringBufferUTF8& str);
	SHVStringUTF8& operator+=(const SHVStringUTF8C& str);
	inline SHVStringUTF8& operator=(const SHVStringUTF8& str);
	inline SHVStringUTF8& operator+=(const SHVStringUTF8& str);


private:
	///\cond INTERNAL
	void AllocBuffer(size_t size);
	size_t BufferLen;
#ifdef __SHVSTRING_HEAPPROTECT
	SHVStrUTF8_DestroyBuffer DestroyFunc;
#endif
	///\endcond
};



// ============================================= implementation ============================================= //

// Buffer handling
SHVChar* SHVStringUTF8::GetBuffer() { return Buffer; }
SHVChar* SHVStringUTF8::SetBufferSize(size_t len) { AllocBuffer(len); return Buffer; }
size_t   SHVStringUTF8::GetBufferLen() { return BufferLen; }  ///< returns length of buffer in characters, including null termination

SHVStringBufferUTF8 SHVStringUTF8::ToUpper() {SHVStringUTF8 str(*this); str.MakeUpper(); return str.ReleaseBuffer(); }
SHVStringBufferUTF8 SHVStringUTF8::ToLower() {SHVStringUTF8 str(*this); str.MakeLower(); return str.ReleaseBuffer(); }

SHVStringUTF8& SHVStringUTF8::operator=(const SHVStringUTF8& str)   { return *this = (*(const SHVStringUTF8C*)&str);  }
SHVStringUTF8& SHVStringUTF8::operator+=(const SHVStringUTF8& str)  { return *this += (*(const SHVStringUTF8C*)&str); }

#endif
