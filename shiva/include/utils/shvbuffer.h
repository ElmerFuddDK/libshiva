#ifndef __SHIVA_UTILS_BUFFER_H
#define __SHIVA_UTILS_BUFFER_H

#include "shvbufferc.h"
#include "shvstring.h"
#include "shvrefobject.h"


//-=========================================================================================================
/// SHVBuffer class - Buffer handling
/**
 * Implements non-constant operations on a buffer
 */

class SHVAPI SHVBuffer : public SHVBufferC
{
public:


	// Buffer operations
	inline SHVByte* GetBuffer();
	inline void* GetBufferAsVoid();
	
	
	// Write functions
	SHVBool WriteChar(const char c, size_t& pos);
	SHVBool WriteShort(const short s, size_t& pos);
	SHVBool WriteInt(const int i, size_t& pos);
	SHVBool WriteSHVInt(const SHVInt i, size_t& pos);
	SHVBool WriteSHVDouble(const SHVDouble i, size_t& pos);
	inline SHVBool WriteNullString(const SHVStringC str, size_t& pos);
	SHVBool WriteNullString8(const SHVString8C str, size_t& pos);
	SHVBool WriteNullStringUTF8(const SHVStringUTF8C str, size_t& pos);
	SHVBool WriteNullString16(const SHVString16C str, size_t& pos);
	inline SHVBool WriteString(const SHVStringC str, size_t& pos);
	SHVBool WriteString8(const SHVString8C str, size_t& pos);
	SHVBool WriteStringUTF8(const SHVStringUTF8C str, size_t& pos);
	SHVBool WriteString16(const SHVString16C str, size_t& pos);
	virtual SHVBool WriteBytes(const SHVByte* buf, size_t bufLen, size_t& pos);
	
	
	// Buffer operations
	virtual SHVBool SetBufferSize(size_t newSize, size_t toCopyFromOld = 0);
	
protected:
	inline SHVBuffer() : SHVBufferC() {}
	virtual SHVByte* Buffer() const = 0;
	virtual size_t Size() const = 0;
	///\cond INTERNAL
#ifdef __SHVSTRING_HEAPPROTECT
	virtual SHVByte* AllocBuffer(size_t len);
	virtual void DestroyBuffer(SHVByte* buf);
#else
	SHVByte* AllocBuffer(size_t len);
	void DestroyBuffer(SHVByte* buf);
#endif
	///\endcond
};
typedef SHVRefObjectContainer<SHVBuffer> SHVBufferRef;


class SHVBufferIterator : public SHVBufferCIterator
{
public:

	// constructor
	inline SHVBufferIterator(SHVBuffer& Buffer);
	
	
	// Write functions
	inline SHVBool WriteChar(const char c);
	inline SHVBool WriteShort(const short s);
	inline SHVBool WriteInt(const int i);
	inline SHVBool WriteSHVInt(const SHVInt i);
	inline SHVBool WriteSHVDouble(const SHVDouble i);
	inline SHVBool WriteNullString(const SHVStringC str);
	inline SHVBool WriteNullString8(const SHVString8C str);
	inline SHVBool WriteNullStringUTF8(const SHVStringUTF8C str);
	inline SHVBool WriteNullString16(const SHVString16C str);
	inline SHVBool WriteString(const SHVStringC str);
	inline SHVBool WriteString8(const SHVString8C str);
	inline SHVBool WriteString16(const SHVString16C str);
	inline SHVBool WriteBytes(const SHVByte* buf, size_t bufLen);
	
protected:
	///\cond INTERNAL
	inline SHVBuffer* Buf() { return (SHVBuffer*)Buffer; }
	///\endcond
};



//-=========================================================================================================
/// SHVBufferPtr class - Contains a heap allocated buffer
/**
 * Use this class when you want to use a variable length buffer. If you have a compile time
 * fixed size buffer use SHVBufferChunk
 */

class SHVAPI SHVBufferPtr : public SHVBuffer
{
public:


	// Constructors
	SHVBufferPtr();
	SHVBufferPtr(const SHVBufferPtr& str); // copy constructor
	virtual ~SHVBufferPtr();


	// Buffer operations
	virtual SHVBool SetBufferSize(size_t newSize, size_t toCopyFromOld = 0);
	SHVBool Expand(int expandBy);
	SHVBool Add(const SHVBufferC& buffer);
	
	
protected:
	///\cond INTERNAL
	SHVByte* BufferPtr;
	size_t BufferSize;
	
	virtual SHVByte* Buffer() const;
	virtual size_t Size() const;
	///\endcond
};
typedef SHVRefObjectContainer<SHVBufferPtr> SHVBufferPtrRef;


//-=========================================================================================================
/// SHVBufferChunk class - Template for 
/**
 * Use if you want to use a compile time fixed size buffer
 */

template<size_t BufferSize>
class SHVBufferChunk : public SHVBuffer
{
public:


	// Constructors
	SHVBufferChunk();

protected:
	///\cond INTERNAL
	SHVByte BufferPtr[BufferSize];
	
	virtual SHVByte* Buffer() const;
	virtual size_t Size() const;
	///\endcond
};


// ============================================= implementation ============================================= //


/*************************************
 * Getbuffer
 *************************************/
SHVByte* SHVBuffer::GetBuffer()
{
	return Buffer();
}
/*************************************
 * GetBufferAsVoid
 *************************************/
void* SHVBuffer::GetBufferAsVoid()
{
	return (void*)Buffer();
}

/*************************************
 * Write*
 *************************************/
#if __SHVSTRINGDEFAULT == 8
SHVBool SHVBuffer::WriteNullString(const SHVStringC str, size_t& pos)
{
	return WriteNullString8(str,pos);
}
SHVBool SHVBuffer::WriteString(const SHVStringC str, size_t& pos)
{
	return WriteString8(str,pos);
}
#elif __SHVSTRINGDEFAULT == 16
SHVBool SHVBuffer::WriteNullString(const SHVStringC str, size_t& pos)
{
	return WriteNullString16(str,pos);
}
SHVBool SHVBuffer::WriteString(const SHVStringC str, size_t& pos)
{
	return WriteString16(str,pos);
}
#elif __SHVSTRINGDEFAULT == utf8
SHVBool SHVBuffer::WriteNullString(const SHVStringC str, size_t& pos)
{
	return WriteNullStringUTF8(str,pos);
}
SHVBool SHVBuffer::WriteString(const SHVStringC str, size_t& pos)
{
	return WriteStringUTF8(str,pos);
}
#endif


/// \class SHVBufferIterator shvbuffer.h "shiva/include/utils/shvbuffer.h"

SHVBufferIterator::SHVBufferIterator(SHVBuffer& buffer) : SHVBufferCIterator()
{ Buffer = (SHVBufferC*)&buffer; }
// Write functions
SHVBool SHVBufferIterator::WriteChar(const char c)
{ Field++; return (Error ? Error = Buf()->WriteChar(c,Pos) : Error); }
SHVBool SHVBufferIterator::WriteShort(const short s)
{ Field++; return (Error ? Error = Buf()->WriteShort(s,Pos) : Error); }
SHVBool SHVBufferIterator::WriteInt(const int i)
{ Field++; return (Error ? Error = Buf()->WriteInt(i,Pos) : Error); }
SHVBool SHVBufferIterator::WriteSHVInt(const SHVInt i)
{ Field++; return (Error ? Error = Buf()->WriteSHVInt(i,Pos) : Error); }
SHVBool SHVBufferIterator::WriteSHVDouble(const SHVDouble i)
{ Field++; return (Error ? Error = Buf()->WriteSHVDouble(i,Pos) : Error); }
SHVBool SHVBufferIterator::WriteNullString(const SHVStringC str)
{ Field++; return (Error ? Error = Buf()->WriteNullString(str,Pos) : Error); }
SHVBool SHVBufferIterator::WriteNullString8(const SHVString8C str)
{ Field++; return (Error ? Error = Buf()->WriteNullString8(str,Pos) : Error); }
SHVBool SHVBufferIterator::WriteNullStringUTF8(const SHVStringUTF8C str)
{ Field++; return (Error ? Error = Buf()->WriteNullStringUTF8(str,Pos) : Error); }
SHVBool SHVBufferIterator::WriteNullString16(const SHVString16C str)
{ Field++; return (Error ? Error = Buf()->WriteNullString16(str,Pos) : Error); }
SHVBool SHVBufferIterator::WriteString(const SHVStringC str)
{ Field++; return (Error ? Error = Buf()->WriteString(str,Pos) : Error); }
SHVBool SHVBufferIterator::WriteString8(const SHVString8C str)
{ Field++; return (Error ? Error = Buf()->WriteString8(str,Pos) : Error); }
SHVBool SHVBufferIterator::WriteString16(const SHVString16C str)
{ Field++; return (Error ? Error = Buf()->WriteString16(str,Pos) : Error); }
SHVBool SHVBufferIterator::WriteBytes(const SHVByte* buf, size_t bufLen)
{ Field++; return (Error ? Error = Buf()->WriteBytes(buf,bufLen,Pos) : Error); }

/// \class SHVBufferChunk shvbuffer.h "shiva/include/utils/shvbuffer.h"

template<size_t BufferSize>
SHVBufferChunk<BufferSize>::SHVBufferChunk() : SHVBuffer()
{}

///\cond INTERNAL
template<size_t BufferSize>
SHVByte* SHVBufferChunk<BufferSize>::Buffer() const
{ return (SHVByte*)BufferPtr; }
template<size_t BufferSize>
size_t SHVBufferChunk<BufferSize>::Size() const
{ return BufferSize; }
///\endcond


#endif
