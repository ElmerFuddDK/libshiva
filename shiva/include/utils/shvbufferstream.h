#ifndef __SHIVA_UTILS_BUFFERSTREAM_H
#define __SHIVA_UTILS_BUFFERSTREAM_H

#include "shvbuffer.h"
#include "shvlist.h"
#include "shvdynarray.h"

//-=========================================================================================================
/// SHVBufferStream class - Writeable buffer stream class
/**
 * 
 */
class SHVAPI SHVBufferStream : public SHVBuffer
{
public:
	SHVBufferStream(size_t chunkSize = 512);
	virtual SHVBool ReadBytes(SHVByte* buf, size_t bufLen, size_t& pos) const;
	virtual SHVBool WriteBytes(const SHVByte* buf, size_t bufLen, size_t& pos);
	virtual SHVBool SetBufferSize(size_t newSize, size_t toCopyFromOld = 0);
protected:
	virtual size_t Seek(SHVByte* seekBuf, size_t bufSize, size_t startPos = 0) const;
	virtual SHVByte* Buffer() const;
	virtual size_t Size() const;
private:
	SHVDynArray<SHVBufferPtr> Buffers;
	size_t TotalBufferSize;
	size_t ChunkSize;
};
typedef SHVRefObjectContainer<SHVBufferStream> SHVBufferStreamRef;

#endif
