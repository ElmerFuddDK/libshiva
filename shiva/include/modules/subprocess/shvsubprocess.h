#ifndef __SHIVA_SUBPROCESS_SUBPROCESS_H
#define __SHIVA_SUBPROCESS_SUBPROCESS_H

#include "../../utils/shvstring.h"
#include "../../utils/shvptrcontainer.h"
#include "../../utils/shvdir.h"
#include "../../utils/shvstreamin.h"
#include "../../utils/shvstreamout.h"
#include "../../utils/shvbuffercstream.h"


//-=========================================================================================================
/// SHVSubProcess class
/**
 * Use this class for launching sub processes that you can interact with. You can launch the
 * sub process in a way that allows you to read from its stdout/err or write to its stdin.
 */

class SHVSubProcess
{
public:

	// enums
	enum Streams {
		StdIn = 1,
		StdOut = 2,
		StdErr = 4,
		StreamDefaults = StdIn|StdOut
	};
	enum Errors {
		ErrGeneric = SHVBool::False,
		ErrNone = SHVBool::True,
		ErrEof,
		ErrInvalidOperation,
		ErrUnknownProgram,
		ErrAlreadyRunning
	};
	enum Flags {
		FlagNone = 0,
		FlagLineBuffer = 1,
		FlagNonBlocking = 2,
		FlagStartDefaults = FlagLineBuffer
	};

	typedef size_t ProcessID;


	virtual ~SHVSubProcess() {}


	// Properties
	virtual SHVBool IsRunning() = 0;
	virtual SHVStreamIn& GetStdOut() = 0;
	virtual SHVStreamIn& GetStdErr() = 0;
	virtual SHVStreamOut& GetStdIn() = 0;
	virtual ProcessID GetPID() = 0;
	
	
	// System properties
	virtual SHVInt64 GetMemUsage() = 0;


	// Startup and shutdown
	virtual SHVBool Start(const SHVStringC program, const SHVStringC args = NULL, int streams = StreamDefaults, int flags = FlagStartDefaults) = 0;
	virtual SHVBool Start(const SHVStringC program, SHVFileList& args, int streams = StreamDefaults, int flags = FlagStartDefaults) = 0;
	virtual void Shutdown() = 0;
	virtual void Kill() = 0;
	virtual void WaitForTermination() = 0;


	// Convenience functions
	virtual SHVBool ReadLine8(SHVString8& line, Streams stream = StdOut) = 0;
	virtual SHVBool ReadLineUTF8(SHVStringUTF8& line, Streams stream = StdOut) = 0;
	virtual SHVBool ReadLine16(SHVString16& line, Streams stream = StdOut) = 0;
	inline SHVBool ReadLine(SHVString& line, Streams stream = StdOut);
	virtual SHVBool WriteLine8(const SHVString8C line) = 0;
	virtual SHVBool WriteLineUTF8(const SHVStringUTF8C line) = 0;
	virtual SHVBool WriteLine16(const SHVString16C line) = 0;
	inline SHVBool WriteLine(const SHVStringC line);

protected:
	inline SHVSubProcess() {}
};
typedef SHVPtrContainer<SHVSubProcess> SHVSubProcessPtr;



// ============================================= implementation ============================================= //

/*************************************
 * WriteChar
 *************************************/
SHVBool SHVSubProcess::ReadLine(SHVString& line, Streams stream)
{
#if __SHVSTRINGDEFAULT == 8
	return ReadLine8(line,stream);
#elif __SHVSTRINGDEFAULT == 16
	return ReadLine16(line,stream);
#elif __SHVSTRINGDEFAULT == utf8
	return ReadLineUTF8(line,stream);
#endif
}

/*************************************
 * WriteChar
 *************************************/
SHVBool SHVSubProcess::WriteLine(const SHVStringC line)
{
#if __SHVSTRINGDEFAULT == 8
	return WriteLine8(line);
#elif __SHVSTRINGDEFAULT == 16
	return WriteLine16(line);
#elif __SHVSTRINGDEFAULT == utf8
	return WriteLineUTF8(line);
#endif
}

#endif
