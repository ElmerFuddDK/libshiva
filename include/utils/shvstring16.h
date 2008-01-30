#ifndef __SHIVA_UTILS_STRING16_H
#define __SHIVA_UTILS_STRING16_H

#include "shvstringc.h"



//-=========================================================================================================
/// SHVString16 class - Modifiable string
/**
 * This string can be modified.
 \see SHVString16C
 */

class SHVAPI SHVString16 : public SHVString16C
{
friend class SHVString16C;
public:


	// OS specific copy functions
#ifdef __SHIVA_EPOC
	static SHVStringBuffer16 Copy(const TPtrC16& aPtr);
#endif


	// constructors
	SHVString16();
	SHVString16(const SHVString16C& str);
	SHVString16(const SHVString16& str); // copy constructor
	SHVString16(const SHVStringBuffer16& str);
	~SHVString16();


	// Buffer handling
	SHVStringBuffer16 ReleaseBuffer();
	inline SHVWChar* GetBuffer();
	inline SHVWChar* SetBufferSize(size_t len);
	inline size_t   GetBufferLen();
	void SetToNull();


	// case functions
	void MakeUpper();
	void MakeLower();


	// Buffer modification
	void AddChars(const SHVWChar* chars, size_t len);
	void TrimLeft(const SHVWChar* trimString = NULL);
	void TrimRight(const SHVWChar* trimString = NULL);
	void Trim();
	void Replace(const SHVString16C& search, const SHVString16C& repl);
	void Format(const SHVWChar* s, ...);
	void FormatList(const SHVWChar* s, SHVVA_LIST args);


	// operators
	SHVWChar& operator[](size_t i);
	SHVString16& operator=(const SHVString16C& str);
	SHVString16& operator=(const SHVStringBuffer16& str);
	SHVString16& operator+=(const SHVString16C& str);
	inline SHVString16& operator=(const SHVString16& str);
	inline SHVString16& operator+=(const SHVString16& str);


private:
	///\cond INTERNAL
	void AllocBuffer(size_t size);
	size_t BufferLen;
#ifdef __SHVSTRING_HEAPPROTECT
	SHVStr16_DestroyBuffer DestroyFunc;
#endif
	///\endcond
};



// ============================================= implementation ============================================= //

// Buffer handling
SHVWChar* SHVString16::GetBuffer() { return Buffer; }
SHVWChar* SHVString16::SetBufferSize(size_t len) { AllocBuffer(len); return Buffer; }
size_t   SHVString16::GetBufferLen() { return BufferLen; }  ///< returns length of buffer in characters, including null termination

SHVString16& SHVString16::operator=(const SHVString16& str)   { return *this = (*(const SHVString16C*)&str);  }
SHVString16& SHVString16::operator+=(const SHVString16& str)  { return *this += (*(const SHVString16C*)&str); }

#endif
