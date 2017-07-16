#ifndef __SHIVA_UTILS_STREAMBUFFEROUT_H
#define __SHIVA_UTILS_STREAMBUFFEROUT_H

#include "shvstreamout.h"
#include "shvbuffer.h"
//-=========================================================================================================
/// SHVStreamBufferIn class - Stream input interface
/**
 * Input stream buffer implementation
 */

class SHVAPI SHVStreamBufferOut: public SHVStreamOut
{
public:
	SHVStreamBufferOut(SHVBuffer* buffer);
	SHVStreamBufferOut();
	virtual SHVBool IsOk() const;
	virtual SHVBool WriteBuffer(const void* buffer, size_t len);

	virtual SHVBool WriteString16(const SHVWChar* buffer, size_t maxsize = SIZE_T_MAX);
	virtual SHVBool WriteChar16(SHVWChar ch);

	virtual SHVBool WriteString8(const SHVChar* buffer, size_t maxsize = SIZE_T_MAX);
	virtual SHVBool WriteChar8(const SHVChar ch);
	
	virtual SHVBool WriteStringUTF8(const SHVChar* buffer, size_t maxsize = SIZE_T_MAX);
	virtual SHVBool WriteCharUTF8(const SHVChar ch);
	
	virtual void Close();
	inline const SHVBufferC* GetBuffer();
private:
	SHVBufferRef Buffer;
	size_t Position;
};


// ============================================= implementation ============================================= //


/*************************************
 * Getbuffer
 *************************************/
const SHVBufferC* SHVStreamBufferOut::GetBuffer()
{
	return Buffer.AsConst();
}

#endif
