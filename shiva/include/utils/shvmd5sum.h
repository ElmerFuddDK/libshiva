#ifndef __SHIVA_UTILS_MD5SUM_H
#define __SHIVA_UTILS_MD5SUM_H

#include "shvbufferc.h"
#include "shvstring.h"


//-=========================================================================================================
/// SHVMd5Sum class - calculates md5 sums
/**
 * Will calculate an md5 sum for one or more buffers.\n
 * The md5 sum is calculated by initializing the md5sum object,
 * and the subsequently calling one of the Calculate functions
 * until all data has been calculated. Finalizing the calculation
 * is done automatically by calling either GetMd5 or
 * GetMd5AsBinary:
\code
void CalcData(const SHVStringC data)
{
SHVMd5Sum md5;

	md5.Initialize();

	if (!data.IsEmpty())
		md5.CalculateString(data);

	SHVConsole::Printf(_S("Md5 sum : %s\n"), md5.GetMd5().GetSafeBuffer());
}
\endcode
 * However it is not necessary to call Initialize in simple cases
 * where you are certain that calculate will be called at least
 * once:
\code
void CalcDataSimple()
{
SHVMd5Sum md5;

	md5.CalculateString8("Some text");
	SHVConsole::Printf(_S("Md5 sum : %s\n"), md5.GetMd5().GetSafeBuffer());
}
\endcode
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
	inline void CalculateStringUTF8(const SHVStringUTF8C str);
	inline void CalculateString16(const SHVString16C str);
	inline void CalculateString(const SHVStringC str);
	SHVStringBuffer GetMd5();
	SHVBuffer* GetMd5AsBinary();


private:

	void* Md5Context;
	///\cond INTERNAL
	SHV_DISABLE_COPY_AND_ASSIGNMENT(SHVMd5Sum);
	///\endcond
};



// ============================================= implementation ============================================= //

/*************************************
 * CalculateString8
 *************************************/
/**
 \param str String to calculate md5 data for
 *
 * Adds to the md5 sum calculation. Finalize with either GetMd5 or GetMd5AsBinary.
 */
void SHVMd5Sum::CalculateString8(const SHVString8C str)
{
SHVBufferCPtr buf((SHVByte*)str.GetSafeBuffer(),str.GetLengthInBytes());
	Calculate(&buf);
}

/*************************************
 * CalculateStringUTF8
 *************************************/
/**
 \param str String to calculate md5 data for
 *
 * Adds to the md5 sum calculation. Finalize with either GetMd5 or GetMd5AsBinary.
 */
void SHVMd5Sum::CalculateStringUTF8(const SHVStringUTF8C str)
{
SHVBufferCPtr buf((SHVByte*)str.GetSafeBuffer(),str.GetLengthInBytes());
	Calculate(&buf);
}

/*************************************
 * CalculateString16
 *************************************/
/**
 \param str String to calculate md5 data for
 *
 * Adds to the md5 sum calculation. Finalize with either GetMd5 or GetMd5AsBinary.
 */
void SHVMd5Sum::CalculateString16(const SHVString16C str)
{
SHVBufferCPtr buf((SHVByte*)str.GetSafeBuffer(),str.GetLengthInBytes());
	Calculate(&buf);
}

/*************************************
 * CalculateString
 *************************************/
/**
 \param str String to calculate md5 data for
 *
 * Adds to the md5 sum calculation. Finalize with either GetMd5 or GetMd5AsBinary.
 */
void SHVMd5Sum::CalculateString(const SHVStringC str)
{
SHVBufferCPtr buf((SHVByte*)str.GetSafeBuffer(),str.GetLengthInBytes());
	Calculate(&buf);
}

#endif
