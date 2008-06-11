#ifndef __SHIVA_UTILS_MD5SUM_H
#define __SHIVA_UTILS_MD5SUM_H

#include "shvbufferc.h"
#include "shvstring.h"


//-=========================================================================================================
/// SHVMd5Sum class - calculates md5 sums
/**
 * Will calculate an md5 sum for one or more buffers.
 */

class SHVAPI SHVMd5Sum
{
public:

	enum { Md5BinaryLen = 16 };


	// constructor
	SHVMd5Sum();
	~SHVMd5Sum();


	void Initialize();
	bool IsInitialized();
	void Calculate(const SHVBufferC* buffer);
	inline void CalculateString8(const SHVString8C str);
	inline void CalculateString16(const SHVString16C str);
	inline void CalculateString(const SHVStringC str);
	SHVStringBuffer GetMd5();
	SHVBuffer* GetMd5AsBinary();


private:

	void* Md5Context;
};



// ============================================= implementation ============================================= //

/*************************************
 * CalculateString8
 *************************************/
void SHVMd5Sum::CalculateString8(const SHVString8C str)
{
SHVBufferCPtr buf((SHVByte*)str.GetSafeBuffer(),str.GetLength());
	Calculate(&buf);
}

/*************************************
 * CalculateString16
 *************************************/
void SHVMd5Sum::CalculateString16(const SHVString16C str)
{
SHVBufferCPtr buf((SHVByte*)str.GetSafeBuffer(),str.GetLength()*sizeof(SHVWChar));
	Calculate(&buf);
}

/*************************************
 * CalculateString
 *************************************/
void SHVMd5Sum::CalculateString(const SHVStringC str)
{
SHVBufferCPtr buf((SHVByte*)str.GetSafeBuffer(),str.GetLength()*sizeof(SHVTChar));
	Calculate(&buf);
}

#endif
