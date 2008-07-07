#ifndef __SHIVA_UTILS_BUFFERCSTREAM_H
#define __SHIVA_UTILS_BUFFERCSTREAM_H

#include "shvbufferc.h"
#include "shvlist.h"
#include "shvdynarray.h"

class SHVBufferCStreamIterator;


//-=========================================================================================================
/// SHVBufferCStream class - Constant buffer stream class
/**
 * Use this class when you want have a number of buffers you read as one structure.
 * Please keep in mind that the following functions from SHVBufferC do not work in
 * the streaming variant:
 \arg Hashing operator (size_t)
 \arg GetBufferConst
 *
 * \n
 * Warning: This class considers the buffers added to them static. Adding expanding
 * buffers or even BufferStreams, and then expanding them will result in unintended
 * behaviour.\n
 * Adding it to itself will make the sky green, and ignite your monitor.
 */

class SHVAPI SHVBufferCStream : public SHVBufferC
{
public:


	// Constructors
	SHVBufferCStream();


	// From SHVBufferC
	virtual SHVBool ReadBytes(SHVByte* buf, size_t bufLen, size_t& pos) const;


	// Buffer manipulation operations
	SHVBool AddBuffer(SHVBufferC* buffer);
	SHVBool Truncate(size_t& pos);


protected:
	friend class SHVBufferCStreamIterator;
	///\cond INTERNAL
	virtual size_t Seek(SHVByte* seekBuf, size_t bufSize, size_t startPos = 0) const;
	virtual SHVByte* Buffer() const;
	virtual size_t Size() const;
	
	struct BufferContainer {
		
		SHVBufferCRef Buffer;
		size_t Start;
		size_t End;
		
		inline BufferContainer(SHVBufferC* buffer, size_t s) : Buffer(buffer), Start(s) { End = s + buffer->GetSize(); }
	};
	static int ListCompare(size_t** a,BufferContainer** b);
	
	SHVDynArray<BufferContainer, size_t> BufferList;
	size_t BufferSize;
	///\endcond
};
typedef SHVRefObjectContainer<SHVBufferCStream> SHVBufferCStreamRef;


// ============================================= implementation ============================================= //
#endif
