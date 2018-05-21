#ifndef __SHIVA_UTILS_BUFFERC_H
#define __SHIVA_UTILS_BUFFERC_H

#include "shvstringc.h"
#include "shvrefobject.h"

class SHVBuffer; // forward declare


//-=========================================================================================================
/// SHVBufferC class - Constant buffer base class
/**
 * This class contains all the constant buffer routines, such as indexing, reading and copying. It is a
 * virtual class, and in order to utilize its funtionality you will need to use one of the derived classes,
 * or inherit from it yourself.\n
 * If you want to inherit from it you will need to implement the following virtuals:\n
 \arg \ref Buffer
 \arg \ref Size
 */

class SHVAPI SHVBufferC : public SHVRefObject
{
public:

	// defines
	enum { InvalidPos = SIZE_T_MAX };
	enum Errors {
		ErrGeneric = SHVBool::False,
		ErrNone = SHVBool::True,
		ErrEof = 1,
		ErrAlloc = 2,
		ErrNotSupported = 3
	};


	// Destructor
	virtual ~SHVBufferC() {}


	// Access/Conversion functions
	inline const SHVByte* GetBufferConst() const;
#ifdef __SHIVA_EPOC
	inline TPtrC8 ToPtr() const;
#endif


	// buffer checks
	inline bool IsNull() const;
	inline size_t GetSize() const;
	operator SHVHashValue() const; ///< hashing function
	
	
	// Read functions
	SHVBool ReadChar(char& c, size_t& pos) const;
	SHVBool ReadShort(short& s, size_t& pos) const;
	SHVBool ReadInt(int& i, size_t& pos) const;
	SHVBool ReadSHVInt(SHVInt& i, size_t& pos) const;
	SHVBool ReadSHVDouble(SHVDouble& i, size_t& pos) const;
	inline SHVBool ReadNullString(SHVString& str, size_t& pos) const;
	SHVBool ReadNullString8(SHVString8& str, size_t& pos) const;
	SHVBool ReadNullStringUTF8(SHVStringUTF8& str, size_t& pos) const;
	SHVBool ReadNullString16(SHVString16& str, size_t& pos) const;
	inline SHVBool ReadString(SHVString& str, size_t strLen, size_t& pos) const;
	SHVBool ReadString8(SHVString8& str, size_t strLen, size_t& pos) const;
	SHVBool ReadStringUTF8(SHVStringUTF8& str, size_t strSize, size_t& pos) const;
	SHVBool ReadString16(SHVString16& str, size_t strLen, size_t& pos) const;
	virtual SHVBool ReadBytes(SHVByte* buf, size_t bufLen, size_t& pos) const;
	
	
	// Search functions
	size_t SeekByte(SHVByte b, size_t startPos = 0) const;
	size_t SeekShort(short s, size_t startPos = 0) const;
	
	
protected:
	virtual SHVByte* Buffer() const = 0;
	virtual size_t Size() const = 0;
	///\cond INTERNAL
	virtual size_t Seek(SHVByte* seekBuf, size_t bufSize, size_t startPos = 0) const;
	///\endcond
};
typedef SHVRefObjectContainer<SHVBufferC> SHVBufferCRef;


//-=========================================================================================================
/// SHVBufferCIterator class - For iterating through a buffer
/**
 * This class makes it easier to read through a buffer.\n
 * Example:
 \code
SHVBool ReadLeadingByteCountString(const SHVBufferC& buffer, SHVString8& str)
{
SHVBufferCIterator itr(buffer);
char strLen;

	while(itr && itr.GetField() < 2)
	{
		// get the field we are reading
		switch (itr.GetField())
		{
		case 0:
			itr.ReadChar(strLen); // reads a char and moves the position
			break;
		case 1:
			itr.ReadString8(str,strLen);
			break;
		default:
			SetError();
			break;
		}
	};

	return (itr.GetError() || itr.GetField() == 2); // GetError() is true if there is no error
}
 \endcode
 * Of course this rediculously simple example could've been done a bit easier:
 \code
SHVBool ReadLeadingByteCountString(const SHVBufferC& buffer, SHVString8& str)
{
SHVBufferCIterator itr(buffer);
char strLen;
	return itr.ReadChar(strLen) && itr.ReadString8(str,strLen);
}
 \endcode
 */

class SHVAPI SHVBufferCIterator
{
public:

	// constructor
	inline SHVBufferCIterator(const SHVBufferC& Buffer);
	
	
	// Iterator functions
	inline operator bool();

	inline int GetField();
	inline SHVBool GetError();
	inline void SetField(int field);
	inline SHVBool SetError(SHVBool err = SHVBool::False);
	inline size_t GetPosition();
	inline SHVBufferCIterator& SetPosition(size_t pos);
	
	
	// Read functions
	inline SHVBool ReadChar(char& c);
	inline SHVBool ReadShort(short& s);
	inline SHVBool ReadInt(int& i);
	inline SHVBool ReadSHVInt(SHVInt& i);
	inline SHVBool ReadSHVDouble(SHVDouble& i);
	inline SHVBool ReadNullString(SHVString& str);
	inline SHVBool ReadNullString8(SHVString8& str);
	inline SHVBool ReadNullStringUTF8(SHVStringUTF8& str);
	inline SHVBool ReadNullString16(SHVString16& str);
	inline SHVBool ReadString(SHVString& str, size_t strLen);
	inline SHVBool ReadString8(SHVString8& str, size_t strLen);
	inline SHVBool ReadStringUTF8(SHVStringUTF8& str, size_t strLen);
	inline SHVBool ReadString16(SHVString16& str, size_t strLen);
	inline SHVBool ReadBytes(SHVByte* buf, size_t bufLen);
	
	
protected:
	///\cond INTERNAL
	SHVBufferC* Buffer;
	size_t Pos;
	int Field;
	SHVBool Error;
	inline SHVBufferCIterator() : Error(SHVBool::True) {Pos = 0; Field = 0;}
	///\endcond
};



//-=========================================================================================================
/// SHVBufferCPtr class - Encapsulates an external buffer
/**
 * Use this class if you have a pointer to a memory chunk that you want to
 * use in the SHIVA buffer system.\n
 * Example usage:
 \code
// char and string
const char bufferConst[] = { 5, 't', 'e', 's', 't' };
char strLen;
SHVString8 str;
SHVBufferCPtr buffer(bufferConst, sizeof(bufferConst));
size_t pos = 0;

	if (buffer.ReadChar(strLen,pos))
		buffer.ReadString8(str,strLen,pos);
 \endcode
 */

class SHVAPI SHVBufferCPtr : public SHVBufferC
{
public:

	// Constructor
	inline SHVBufferCPtr(const SHVByte* buffer, size_t size);

	// Convenience conversion functions
	inline static SHVBufferCPtr VoidToBuffer(const void* str, size_t len);
	inline static SHVBufferCPtr StringToBuffer8(const SHVString8C str, bool includeNull = false);
	inline static SHVBufferCPtr StringToBufferUTF8(const SHVStringUTF8C str, bool includeNull = false);
	inline static SHVBufferCPtr StringToBuffer16(const SHVString16C str, bool includeNull = false);
	
protected:
	///\cond INTERNAL
	SHVByte* BufferPtr;
	size_t BufferSize;
	virtual SHVByte* Buffer() const;
	virtual size_t Size() const;
	///\endcond
};



// ============================================= implementation ============================================= //


/*************************************
 * GetBufferConst
 *************************************/
/// Returns the internal buffer as const
/**
 \return the internal buffer as const
 *
 * This is not supported by all deriatives. It will return NULL in streams
 */
const SHVByte* SHVBufferC::GetBufferConst() const
{
	return Buffer();
}
#ifdef __SHIVA_EPOC
/// Returns the internal buffer as const
/**
 \return the internal buffer as const
 *
 * This is not supported by all deriatives. It will return NULL in streams
 */
TPtrC8 SHVBufferC::ToPtr() const
{
	TPtrC8((TUint8*)Buffer(),Size());
}
#endif

/*************************************
 * IsNull
 *************************************/
/// Returns true if the buffer is null
/**
 \return true if the buffer is null
 *
 * This is not supported by all deriatives. It will always return true in streams
 */
bool SHVBufferC::IsNull() const
{
	return (Buffer() == NULL);
}

/*************************************
 * IsNull
 *************************************/
/// Returns the size of the buffer
/**
 \return size of buffer
 */
size_t SHVBufferC::GetSize() const
{
	return Size();
}

/*************************************
 * SHVBufferCPtr constructor
 *************************************/
SHVBufferCPtr::SHVBufferCPtr(const SHVByte* buffer, size_t size) : SHVBufferC()
{ BufferPtr = (SHVByte*)buffer; BufferSize = size; }

/*************************************
 * Convenience conversion functions
 *************************************/
/// Creates a SHVBufferCPtr from a void*
SHVBufferCPtr SHVBufferCPtr::VoidToBuffer(const void* str, size_t len)
{
	return SHVBufferCPtr((SHVByte*)str,len);
}
/// Creates a SHVBufferCPtr from a SHVString8C
SHVBufferCPtr SHVBufferCPtr::StringToBuffer8(const SHVString8C str, bool includeNull)
{
	return SHVBufferCPtr((SHVByte*)str.GetBufferConst(),str.GetSizeInBytes()+(includeNull ? 1 : 0));
}
/// Creates a SHVBufferCPtr from a SHVStringUTF8C
SHVBufferCPtr SHVBufferCPtr::StringToBufferUTF8(const SHVStringUTF8C str, bool includeNull)
{
	return SHVBufferCPtr((SHVByte*)str.GetBufferConst(),str.GetSizeInBytes()+(includeNull ? 1 : 0));
}
/// Creates a SHVBufferCPtr from a SHVString16C
SHVBufferCPtr SHVBufferCPtr::StringToBuffer16(const SHVString16C str, bool includeNull)
{
	return SHVBufferCPtr((SHVByte*)str.GetBufferConst(),str.GetSizeInBytes()+(includeNull ? 2 : 0));
}

/*************************************
 * string encoding specific variants of readstring
 *************************************/
#if __SHVSTRINGDEFAULT == 8
SHVBool SHVBufferC::ReadNullString(SHVString& str, size_t& pos) const
{ return ReadNullString8(str,pos); }
SHVBool SHVBufferC::ReadString(SHVString& str, size_t strLen, size_t& pos) const
{ return ReadString8(str,strLen,pos); }
#elif __SHVSTRINGDEFAULT == 16
SHVBool SHVBufferC::ReadNullString(SHVString& str, size_t& pos) const
{ return ReadNullString16(str,pos); }
SHVBool SHVBufferC::ReadString(SHVString& str, size_t strLen, size_t& pos) const
{ return ReadString16(str,strLen,pos); }
#elif __SHVSTRINGDEFAULT == utf8
SHVBool SHVBufferC::ReadNullString(SHVString& str, size_t& pos) const
{ return ReadNullStringUTF8(str,pos); }
SHVBool SHVBufferC::ReadString(SHVString& str, size_t strLen, size_t& pos) const
{ return ReadStringUTF8(str,strLen,pos); }
#endif

/// \class SHVBufferCIterator shvbufferc.h "shiva/include/utils/shvbufferc.h"

/*************************************
 * SHVBufferCIterator constructor
 *************************************/
SHVBufferCIterator::SHVBufferCIterator(const SHVBufferC& buffer) : Error(SHVBool::True)
{ Buffer = (SHVBufferC*)&buffer; Field = 0; Pos = 0; }

/*************************************
 * SHVBufferCIterator functions
 *************************************/
/// Returns true if there is no error set and there is more left in the buffer
SHVBufferCIterator::operator bool()
{ return (Pos < Buffer->GetSize() && Error); }
/// Gets the error value or SHVBool::True if no error is set
SHVBool SHVBufferCIterator::GetError()
{ return Error; }
/// Gets the current field identifier
int SHVBufferCIterator::GetField()
{ return Field; }
/// Sets the field identifier
void SHVBufferCIterator::SetField(int field)
{ Field = field; }
/// Sets an error value
SHVBool SHVBufferCIterator::SetError(SHVBool err)
{ return Error = err; }
/// Obtain the current position in the iterator
size_t SHVBufferCIterator::GetPosition()
{ return Pos; }
/// Set the current position in the iterator
SHVBufferCIterator& SHVBufferCIterator::SetPosition(size_t pos)
{
	Pos = pos;
	
	return *this;
}

/*************************************
 * SHVBufferCIterator read functions
 *************************************/
/// Reads a char and moves the position
SHVBool SHVBufferCIterator::ReadChar(char& c)
{ Field++; return (Error ? Error = Buffer->ReadChar(c,Pos) : Error); }
/// Reads a short and moves the position
SHVBool SHVBufferCIterator::ReadShort(short& s)
{ Field++; return (Error ? Error = Buffer->ReadShort(s,Pos) : Error); }
/// Reads an int and moves the position
SHVBool SHVBufferCIterator::ReadInt(int& i)
{ Field++; return (Error ? Error = Buffer->ReadInt(i,Pos) : Error); }
/// Reads a SHVInt and moves the position
SHVBool SHVBufferCIterator::ReadSHVInt(SHVInt& i)
{ Field++; return (Error ? Error = Buffer->ReadSHVInt(i,Pos) : Error); }
/// Reads an SHVDouble and moves the position
SHVBool SHVBufferCIterator::ReadSHVDouble(SHVDouble& i)
{ Field++; return (Error ? Error = Buffer->ReadSHVDouble(i,Pos) : Error); }
/// Reads a null terminated string and moves the position
/**
 * See \ref SHVBufferC::ReadNullString8
 */
SHVBool SHVBufferCIterator::ReadNullString(SHVString& str)
{ Field++; return (Error ? Error = Buffer->ReadNullString(str,Pos) : Error); }
/// Reads a null terminated string and moves the position
/**
 * See \ref SHVBufferC::ReadNullString8
 */
SHVBool SHVBufferCIterator::ReadNullString8(SHVString8& str)
{ Field++; return (Error ? Error = Buffer->ReadNullString8(str,Pos) : Error); }
/// Reads a null terminated string and moves the position
/**
 * See \ref SHVBufferC::ReadNullStringUTF8
 */
SHVBool SHVBufferCIterator::ReadNullStringUTF8(SHVStringUTF8& str)
{ Field++; return (Error ? Error = Buffer->ReadNullStringUTF8(str,Pos) : Error); }
/// Reads a null terminated string and moves the position
/**
 * See \ref SHVBufferC::ReadNullString16
 */
SHVBool SHVBufferCIterator::ReadNullString16(SHVString16& str)
{ Field++; return (Error ? Error = Buffer->ReadNullString16(str,Pos) : Error); }
/// Reads a string and moves the position
/**
 * See \ref SHVBufferC::ReadString8
 */
SHVBool SHVBufferCIterator::ReadString(SHVString& str, size_t strLen)
{ Field++; return (Error ? Error = Buffer->ReadString(str,strLen,Pos) : Error); }
/// Reads a string and moves the position
/**
 * See \ref SHVBufferC::ReadString8
 */
SHVBool SHVBufferCIterator::ReadString8(SHVString8& str, size_t strLen)
{ Field++; return (Error ? Error = Buffer->ReadString8(str,strLen,Pos) : Error); }
/// Reads a string and moves the position
/**
 * See \ref SHVBufferC::ReadStringUTF8
 */
SHVBool SHVBufferCIterator::ReadStringUTF8(SHVStringUTF8& str, size_t strLen)
{ Field++; return (Error ? Error = Buffer->ReadStringUTF8(str,strLen,Pos) : Error); }
/// Reads a string and moves the position
/**
 * See \ref SHVBufferC::ReadString16
 */
SHVBool SHVBufferCIterator::ReadString16(SHVString16& str, size_t strLen)
{ Field++; return (Error ? Error = Buffer->ReadString16(str,strLen,Pos) : Error); }
/// Reads a number of bytes and moves the position
/**
 * See \ref SHVBufferC::ReadBytes
 */
SHVBool SHVBufferCIterator::ReadBytes(SHVByte* buf, size_t bufLen)
{ Field++; return (Error ? Error = Buffer->ReadBytes(buf,bufLen,Pos) : Error); }

// ============================================== documentation ============================================= //

// SHVBufferC stuff
/** \fn SHVByte* SHVBufferC::Buffer() const
 \brief Must return a pointer to the buffer
 *
 * You will need to cast your way out of the const thing - this is not
 * a bug.
 */
/** \fn size_t SHVBufferC::Size() const
 \brief Must return the size of the buffer
 */

#endif
