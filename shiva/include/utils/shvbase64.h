#ifndef __SHIVA_UTILS_BASE64_H
#define __SHIVA_UTILS_BASE64_H

#include "../shvtypes.h"
#include "shvbuffer.h"
#include "shvbuffercstream.h"
#include "shvlist.h"
#include "shvstring.h"


//-=========================================================================================================
/// SHVBase64Encoder class - Base64 encoding
/**
 * Implements functions for encoding base64
 */

class SHVAPI SHVBase64Encoder
{
public:


	// constructor
	SHVBase64Encoder();
	~SHVBase64Encoder();


	// Initialization functions
	void Initialize();
	bool IsInitialized();

	// Calculation functions
	void Calculate(const SHVBufferC* buffer);
	inline void CalculateString8(const SHVString8C str);
	inline void CalculateStringUTF8(const SHVStringUTF8C str);
	inline void CalculateString16(const SHVString16C str);
	inline void CalculateString(const SHVStringC str);

	// Finalization functions
	SHVStringBuffer8 GetDataAsString(int lineWrap = 0, const SHVString8C lineToken = "\n");
	SHVBuffer* GetDataAsBuffer();


private:

	///\cond INTERNAL
	void CommitWorkset();
	void Finalize();

	bool Initialized;
	SHVList<SHVString8,const SHVString8C> Data;
	size_t RemainingDataBuffer;
	unsigned char Workset[3];
	int WorksetIndex;

	SHV_DISABLE_COPY_AND_ASSIGNMENT(SHVBase64Encoder);
	///\endcond
};


//-=========================================================================================================
/// SHVBase64Decoder class - Base64 decoding
/**
 * Implements functions for decoding base64
 */

class SHVAPI SHVBase64Decoder
{
public:

	enum Errors {
		ErrNone = SHVBool::True,
		ErrInvalidChar = 1,
		ErrGeneric = SHVBool::False
	};


	// constructor
	SHVBase64Decoder();
	~SHVBase64Decoder();

	SHVBool GetError();


	// Initialization functions
	void Initialize();
	bool IsInitialized();

	// Calculation functions
	SHVBool Calculate(const SHVBufferC* buffer);
	inline SHVBool CalculateString8(const SHVString8C str);
	inline SHVBool CalculateStringUTF8(const SHVStringUTF8C str);
	inline SHVBool CalculateString16(const SHVString16C str);
	inline SHVBool CalculateString(const SHVStringC str);

	// Finalization functions
	SHVBuffer* GetData();


private:

	///\cond INTERNAL
	void CommitWorkset();
	void Finalize();

	bool Initialized;
	SHVBufferCStreamRef Data;
	SHVBufferPtrRef WorkingBuffer;
	size_t RemainingDataBuffer;
	unsigned char Workset[4];
	int WorksetIndex;
	SHVBool Error;

	SHV_DISABLE_COPY_AND_ASSIGNMENT(SHVBase64Decoder);
	///\endcond
};


// ============================================= implementation ============================================= //

/*************************************
 * CalculateString8
 *************************************/
void SHVBase64Encoder::CalculateString8(const SHVString8C str)
{
SHVBufferCPtr buf((SHVByte*)str.GetSafeBuffer(),str.GetSizeInBytes());
	Calculate(&buf);
}

/*************************************
 * CalculateStringUTF8
 *************************************/
void SHVBase64Encoder::CalculateStringUTF8(const SHVStringUTF8C str)
{
SHVBufferCPtr buf((SHVByte*)str.GetSafeBuffer(),str.GetSizeInBytes());
	Calculate(&buf);
}

/*************************************
 * CalculateString16
 *************************************/
void SHVBase64Encoder::CalculateString16(const SHVString16C str)
{
SHVBufferCPtr buf((SHVByte*)str.GetSafeBuffer(),str.GetSizeInBytes());
	Calculate(&buf);
}

/*************************************
 * CalculateString
 *************************************/
void SHVBase64Encoder::CalculateString(const SHVStringC str)
{
SHVBufferCPtr buf((SHVByte*)str.GetSafeBuffer(),str.GetSizeInBytes());
	Calculate(&buf);
}

/*************************************
 * CalculateString8
 *************************************/
SHVBool SHVBase64Decoder::CalculateString8(const SHVString8C str)
{
SHVBufferCPtr buf((SHVByte*)str.GetSafeBuffer(),str.GetSizeInBytes());
	return Calculate(&buf);
}

/*************************************
 * CalculateStringUTF8
 *************************************/
SHVBool SHVBase64Decoder::CalculateStringUTF8(const SHVStringUTF8C str)
{
SHVBufferCPtr buf((SHVByte*)str.GetSafeBuffer(),str.GetSizeInBytes());
	return Calculate(&buf);
}

/*************************************
 * CalculateString16
 *************************************/
SHVBool SHVBase64Decoder::CalculateString16(const SHVString16C str)
{
	return CalculateString8(str.ToStr8());
}

/*************************************
 * CalculateString
 *************************************/
SHVBool SHVBase64Decoder::CalculateString(const SHVStringC str)
{
#if __SHVSTRINGDEFAULT == 8
	return CalculateString8(str);
#elif __SHVSTRINGDEFAULT == 16
	return CalculateString8(str.ToStr8());
#elif __SHVSTRINGDEFAULT == utf8
	return CalculateStringUTF8(str);
#endif
}

#endif
