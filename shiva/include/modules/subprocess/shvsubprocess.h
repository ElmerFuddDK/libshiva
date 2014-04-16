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
 \note At the moment it is only supported in linux
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


	virtual ~SHVSubProcess() {}


	// Properties
	virtual SHVBool IsRunning() = 0;
	virtual SHVStreamIn& GetStdOut() = 0;
	virtual SHVStreamIn& GetStdErr() = 0;
	virtual SHVStreamOut& GetStdIn() = 0;
	
	
	// System properties
	virtual SHVInt64 GetMemUsage() = 0;


	// Startup and shutdown
	virtual SHVBool Start(const SHVStringC program, const SHVStringC args = NULL, int streams = StreamDefaults, bool nonBlocking = false) = 0;
	virtual SHVBool Start(const SHVStringC program, SHVFileList& args, int streams = StreamDefaults, bool nonBlocking = false) = 0;
	virtual void Shutdown() = 0;
	virtual void Kill() = 0;
	virtual void WaitForTermination() = 0;


	// Convenience functions
	virtual SHVBool ReadLine8(SHVString8& line, Streams stream = StdOut) = 0;
	virtual SHVBool ReadLine16(SHVString16& line, Streams stream = StdOut) = 0;
	inline SHVBool ReadLine(SHVString& line, Streams stream = StdOut);
	virtual SHVBool WriteLine8(const SHVString8C line) = 0;
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
#ifdef UNICODE
	return ReadLine16(line,stream);
#else
	return ReadLine8(line,stream);
#endif
}

/*************************************
 * WriteChar
 *************************************/
SHVBool SHVSubProcess::WriteLine(const SHVStringC line)
{
#ifdef UNICODE
	return WriteLine16(line);
#else
	return WriteLine8(line);
#endif
}

#endif
