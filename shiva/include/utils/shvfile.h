#ifndef __SHIVA_UTILS_FILE_H
#define __SHIVA_UTILS_FILE_H

#include "shvfilebase.h"


//-=========================================================================================================
/// SHVFile class - standard file operations
/**
 * Expands SHVFileBase with some basic non-platform specific file operations.
 */

class SHVAPI SHVFile : public SHVFileBase
{
public:


	// constructor
	SHVFile();


	// Text operations
	bool ReadString8(SHVString8& str);
	bool WriteString8(const SHVString8C& str);
	bool ReadLine8(SHVString8& str);
	bool WriteLine8(const SHVString8C& str);
#ifndef __SHVSTRING_EXCLUDE_UNICODE
	bool ReadStringUTF8(SHVStringUTF8& str);
	bool WriteStringUTF8(const SHVStringUTF8C& str);
	bool ReadLineUTF8(SHVStringUTF8& str);
	bool WriteLineUTF8(const SHVStringUTF8C& str);
	bool ReadString16(SHVString16& str);
	bool WriteString16(const SHVString16C& str);
	bool ReadLine16(SHVString16& str);
	bool WriteLine16(const SHVString16C& str);
#endif

	inline bool ReadString(SHVString& str);
	inline bool WriteString(const SHVStringC& str);
	inline bool ReadLine(SHVString& str);
	inline bool WriteLine(const SHVStringC& str);

private:
	///\cond INTERNAL
	SHVFilePos ReadLineReal(char*& result, size_t charSize);
	///\endcond
};



// ============================================= implementation ============================================= //

#ifdef UNICODE
bool SHVFile::ReadString(SHVString& str) { return ReadString16(str); }
bool SHVFile::WriteString(const SHVStringC& str) { return WriteString16(str); }
bool SHVFile::ReadLine(SHVString& str) { return ReadLine16(str); }
bool SHVFile::WriteLine(const SHVStringC& str) { return WriteLine16(str); }
#else
bool SHVFile::ReadString(SHVString& str) { return ReadString8(str); }
bool SHVFile::WriteString(const SHVStringC& str) { return WriteString8(str); }
bool SHVFile::ReadLine(SHVString& str) { return ReadLine8(str); }
bool SHVFile::WriteLine(const SHVStringC& str) { return WriteLine8(str); }
#endif

#endif
