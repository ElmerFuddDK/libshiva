#ifndef __SHIVA_UTILS_STRINGSTREAM_H
#define __SHIVA_UTILS_STRINGSTREAM_H

#include "shvstring.h"
#include "shvstringconv.h"

class SHVStringStream8;
class SHVStringStreamUTF8;
class SHVStringStream16;

#ifdef UNICODE
typedef SHVStringStream16  SHVStringStream;
#else
typedef SHVStringStream8  SHVStringStream;
#endif


//-=========================================================================================================
/// SHVStringStream8 class - Streaming of strings
/**
 * Use this class if you want to build a large string consisting
 * of several smaller strings. It inherits from SHVString8C, thus
 * all routines from there are accessible once finalized.\n
 * It works by constantly reallocating a buffer in a given increment
 * size in order to make sure there is room for the streamed data.
 * When you are done streaming you will need to finalize it, in order
 * to make the string properly accessible through the SHVString8C
 * functions.
 \code
SHVStringStream8 stream;
	stream.WriteString(_S("This "));
	stream.WriteString(_S(" is a st"));
	stream.WriteString(_S("ream of data written."));
	stream.Finalize();
	
	if (stream == "This is a stream of data written.") // notice the written data is now 8 bit
		SHVConsole::Printf(_S("Yay\n"));
 \endcode
 * Reencoded strings are written directly to the buffer.
 */

class SHVAPI SHVStringStream8 : public SHVString8C
{
public:


	// Constructor
	SHVStringStream8(size_t incrementSize = 512);
	~SHVStringStream8();
	
	
	// Write functions
	SHVBool WriteString8(const SHVString8C str);
	SHVBool WriteString16(const SHVString16C str);
	SHVBool WriteStringUTF8(const SHVStringUTF8C str);
	inline SHVBool WriteString(const SHVStringC str);
	
	SHVBool AddChars8(const SHVChar* chars, size_t len);
	SHVBool AddChars16(const SHVWChar* chars, size_t len);
	SHVBool AddCharsUTF8(const SHVChar* chars, size_t len);
	inline SHVBool AddChars(const SHVTChar* chars, size_t len);
	
	
	void Reset();	
	void Finalize();
	
	SHVStringBuffer8 ReleaseBuffer();
	
	
private:
	///\cond INTERNAL
	void EnsureBufferSpaceFree(size_t minChars = 1);
	
	SHVChar* BufferPos;
	size_t BufferLeft;
	size_t Increment;
	
	SHVStringConvPtr ConvUtf8;
	SHVStringConvPtr Conv16;

	// disallow copy construction and assignment
private:
	inline SHVStringStream8(const SHVStringStream8& ) : SHVString8C() {}
	inline SHVStringStream8& operator=(const SHVStringStream8& ) {return *this;}
	///\endcond
};
typedef SHVPtrContainer<SHVStringStream8> SHVStringStream8Ptr;



//-=========================================================================================================
/// SHVStringStreamUTF8 class - Streaming of strings
/**
 * The utf8 variant of string streaming.
 \see SHVStringStream8
 */

class SHVAPI SHVStringStreamUTF8 : public SHVStringUTF8C
{
public:


	// Constructor
	SHVStringStreamUTF8(size_t incrementSize = 512);
	~SHVStringStreamUTF8();
	
	
	// Write functions
	SHVBool WriteString8(const SHVString8C str);
	SHVBool WriteString16(const SHVString16C str);
	SHVBool WriteStringUTF8(const SHVStringUTF8C str);
	inline SHVBool WriteString(const SHVStringC str);
	
	SHVBool AddChars8(const SHVChar* chars, size_t len);
	SHVBool AddChars16(const SHVWChar* chars, size_t len);
	SHVBool AddCharsUTF8(const SHVChar* chars, size_t len);
	inline SHVBool AddChars(const SHVTChar* chars, size_t len);
	
	void Reset();	
	void Finalize();
	
	SHVStringBufferUTF8 ReleaseBuffer();
	
	
private:
	///\cond INTERNAL
	void EnsureBufferSpaceFree(size_t minChars = 1);
	
	SHVChar* BufferPos;
	size_t BufferLeft;
	size_t Increment;
	
	SHVStringConvPtr Conv8;
	SHVStringConvPtr Conv16;

	// disallow copy construction and assignment
private:
	inline SHVStringStreamUTF8(const SHVStringStreamUTF8& ) : SHVStringUTF8C() {}
	inline SHVStringStreamUTF8& operator=(const SHVStringStreamUTF8& ) {return *this;}
	///\endcond
};
typedef SHVPtrContainer<SHVStringStreamUTF8> SHVStringStreamUTF8Ptr;



//-=========================================================================================================
/// SHVStringStream16 class - Streaming of strings
/**
 * The ucs2 variant of string streaming.
 \see SHVStringStream8
 */

class SHVAPI SHVStringStream16 : public SHVString16C
{
public:


	// Constructor
	SHVStringStream16(size_t incrementSize = 512);
	~SHVStringStream16();
	
	
	// Write functions
	SHVBool WriteString8(const SHVString8C str);
	SHVBool WriteString16(const SHVString16C str);
	SHVBool WriteStringUTF8(const SHVStringUTF8C str);
	inline SHVBool WriteString(const SHVStringC str);
	
	SHVBool AddChars8(const SHVChar* chars, size_t len);
	SHVBool AddChars16(const SHVWChar* chars, size_t len);
	SHVBool AddCharsUTF8(const SHVChar* chars, size_t len);
	inline SHVBool AddChars(const SHVTChar* chars, size_t len);

	void Reset();	
	void Finalize();
	
	SHVStringBuffer16 ReleaseBuffer();
	
	
private:
	///\cond INTERNAL
	void EnsureBufferSpaceFree(size_t minChars = 1);
	
	SHVWChar* BufferPos;
	size_t BufferLeft;
	size_t Increment;
	
	SHVStringConvPtr Conv8;
	SHVStringConvPtr ConvUtf8;

	// disallow copy construction and assignment
private:
	inline SHVStringStream16(const SHVStringStream16& ) : SHVString16C() {}
	inline SHVStringStream16& operator=(const SHVStringStream16& ) {return *this;}
	///\endcond
};
typedef SHVPtrContainer<SHVStringStream16> SHVStringStream16Ptr;




// ============================================= implementation ============================================= //

# ifdef UNICODE
SHVBool SHVStringStream8::WriteString(const SHVStringC str) { return WriteString16(str); }
SHVBool SHVStringStreamUTF8::WriteString(const SHVStringC str) { return WriteString16(str); }
SHVBool SHVStringStream16::WriteString(const SHVStringC str) { return WriteString16(str); }
SHVBool SHVStringStream8::AddChars(const SHVTChar* chars, size_t len) { return AddChars16(chars,len); }
SHVBool SHVStringStreamUTF8::AddChars(const SHVTChar* chars, size_t len) { return AddChars16(chars,len); }
SHVBool SHVStringStream16::AddChars(const SHVTChar* chars, size_t len) { return AddChars16(chars,len); }
# else
SHVBool SHVStringStream8::WriteString(const SHVStringC str) { return WriteString8(str); }
SHVBool SHVStringStreamUTF8::WriteString(const SHVStringC str) { return WriteString8(str); }
SHVBool SHVStringStream16::WriteString(const SHVStringC str) { return WriteString8(str); }
SHVBool SHVStringStream8::AddChars(const SHVTChar* chars, size_t len) { return AddChars8(chars,len); }
SHVBool SHVStringStreamUTF8::AddChars(const SHVTChar* chars, size_t len) { return AddChars8(chars,len); }
SHVBool SHVStringStream16::AddChars(const SHVTChar* chars, size_t len) { return AddChars8(chars,len); }
# endif


#endif
