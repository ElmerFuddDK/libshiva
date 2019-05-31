#ifndef __SHIVA_UTILS_STRINGCONV_H
#define __SHIVA_UTILS_STRINGCONV_H

#include "shvstring.h"
#include "shvstringutf8.h"
#include "shvptrcontainer.h"



//-=========================================================================================================
/// SHVStringConv class - Conversion between strings
/**
 * Allows continuous conversion of characters between 2 different string types.
 \see SHVStringConv::Convert
 */

class SHVAPI SHVStringConv
{
public:

	
	enum Enc {
		Enc8,
		Enc16,
		EncUtf8
	};

	
	SHVStringConv(Enc from, Enc to);
	~SHVStringConv();
	
	bool IsValid();
	
#ifdef __SHIVA_WIN32
	void SetDosEncoding(int win32cp = -1); ///\ only relevant for win32 - enables conversion to/from DOS encoding for Enc8 - defaults to CP_OEMCP which is standard dos encoding
#endif
	
	const SHVByte* Convert(const SHVByte* inBuffer, void* outBuffer, size_t len, size_t* charsWritten = NULL);
	

private:
	///\cond INTERNAL
	const SHVChar* Convert8To16(const SHVChar* inBuffer, SHVWChar* outBuffer, size_t len, size_t* charsWritten);
	const SHVWChar* Convert16To8(const SHVWChar* inBuffer, SHVChar* outBuffer, size_t len, size_t* charsWritten);
	const SHVChar* ConvertUTF8To8(const SHVChar* inBuffer, SHVChar* outBuffer, size_t len, size_t* charsWritten);
	const SHVChar* ConvertUTF8To16(const SHVChar* inBuffer, SHVWChar* outBuffer, size_t len, size_t* charsWritten);
	const SHVChar* Convert8ToUTF8(const SHVChar* inBuffer, SHVChar* outBuffer, size_t len, size_t* charsWritten);
	const SHVWChar* Convert16ToUTF8(const SHVWChar* inBuffer, SHVChar* outBuffer, size_t len, size_t* charsWritten);
	
	Enc From, To;
#ifdef __SHIVA_WIN32
	int DosEncoding;
#elif defined(__SHIVA_EPOC)
	RFs fs;
	void* convTo;
	void* convFrom;
#else
	size_t iconvData;
#endif
	///\endcond
};
typedef SHVPtrContainer<SHVStringConv> SHVStringConvPtr ;



// ============================================= implementation ============================================= //


#endif
