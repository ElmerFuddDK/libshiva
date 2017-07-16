#ifndef __SHIVA_UTILS_STREAMBUFFERIN_H
#define __SHIVA_UTILS_STREAMBUFFERIN_H

#include "shvbufferc.h"
#include "shvstreamin.h"
//-=========================================================================================================
/// SHVStreamBufferIn class - Stream input interface
/**
 * Input stream buffer implementation
 */

class SHVAPI SHVStreamBufferIn: public SHVStreamIn
{
public:
	SHVStreamBufferIn(SHVBufferC* buffer);
	virtual bool Eof() const;
	virtual SHVBool IsOk() const;
	virtual SHVBool ReadBuffer(void* buffer, size_t bufferSize, size_t& actualLength);

	virtual SHVBool ReadString16(SHVWChar* buffer, size_t size);
	virtual SHVWChar ReadChar16();

	virtual SHVBool ReadString8(SHVChar* buffer, size_t size);
	virtual SHVChar ReadChar8();

	virtual SHVBool ReadStringUTF8(SHVChar* buffer, size_t size);
	virtual SHVChar ReadCharUTF8();

	virtual void Close();
	inline void SetBuffer(SHVBufferC* buffer);
private:
	SHVBufferCRef Buffer;
	size_t Position; 
	bool BufferEof;
};

// ============================================= implementation ============================================= //


/*************************************
 * SetBuffer
 *************************************/
void SHVStreamBufferIn::SetBuffer(SHVBufferC* buffer)
{
	Buffer = buffer;
}

#endif
