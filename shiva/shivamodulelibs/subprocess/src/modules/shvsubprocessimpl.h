#ifndef __SHIVA_SUBPROCESSIMPL_SUBPROCESS_H
#define __SHIVA_SUBPROCESSIMPL_SUBPROCESS_H

#include "../../../../include/modules/subprocess/shvsubprocess.h"
///\cond INTERNAL
class SHVSubProcessStreamIn;
class SHVSubProcessStreamOut;
///\endcond

//-=========================================================================================================
/// SHVSubProcessImpl class
/**
 * Implementation of SHVSubProcess
 */

class SHVSubProcessImpl : public SHVSubProcess
{
public:


	// Constructor
	SHVSubProcessImpl();
	~SHVSubProcessImpl();


	// Properties
	virtual SHVBool IsRunning();
	virtual SHVStreamIn& GetStdOut();
	virtual SHVStreamIn& GetStdErr();
	virtual SHVStreamOut& GetStdIn();
	virtual ProcessID GetPID();


	// System properties
	virtual SHVInt64 GetMemUsage();


	// Startup and shutdown
	virtual SHVBool Start(const SHVStringC program, const SHVStringC args = NULL, int streams = StreamDefaults, bool nonBlocking = false);
	virtual SHVBool Start(const SHVStringC program, SHVFileList& args, int streams = StreamDefaults, bool nonBlocking = false);
	virtual void Shutdown();
	virtual void Kill();
	virtual void WaitForTermination();


	// Convenience functions
	virtual SHVBool ReadLine8(SHVString8& line, Streams stream = StdOut);
	virtual SHVBool ReadLineUTF8(SHVStringUTF8& line, Streams stream = StdOut);
	virtual SHVBool ReadLine16(SHVString16& line, Streams stream = StdOut);
	virtual SHVBool WriteLine8(const SHVString8C line);
	virtual SHVBool WriteLineUTF8(const SHVStringUTF8C line);
	virtual SHVBool WriteLine16(const SHVString16C line);


private:
	///\cond INTERNAL
	void ParseArgs(SHVFileList& argList, const SHVStringC args);
	void Trim(const SHVTChar*& ch);

	SHVSubProcessStreamIn *StreamStdOut, *StreamStdErr;
	SHVSubProcessStreamOut *StreamStdIn;
	SHVBool LastError;
	bool NonBlocking;
#ifdef __SHIVA_POSIX
	pid_t Pid;
	int PipeStdOut[2], PipeStdErr[2], PipeStdIn[2];

	static void SafeCloseFd(int& fd);
#elif defined(__SHIVA_WIN32)
	PROCESS_INFORMATION Process;
	HANDLE hStdOut, hStdErr, hStdIn;
#endif
	///\endcond
};


// ============================================= implementation ============================================= //

///\cond INTERNAL
//=========================================================================================================
// SHVSubProcessStreamIn class
class SHVSubProcessStreamIn : public SHVStreamIn
{
public:

#ifdef __SHIVA_POSIX
	SHVSubProcessStreamIn(int& fd);
#elif defined(__SHIVA_WIN32)
	SHVSubProcessStreamIn(HANDLE& fd);
#endif
	
	virtual bool Eof() const;
	virtual SHVBool IsOk() const;
	virtual SHVBool ReadBuffer(void* buffer, size_t bufferSize, size_t& actualLength);

	virtual SHVBool ReadString16(SHVWChar* buffer, size_t maxlen);
	virtual SHVWChar ReadChar16();

	virtual SHVBool ReadString8(SHVChar* buffer, size_t maxlen);
	virtual SHVChar ReadChar8();
	
	virtual SHVBool ReadStringUTF8(SHVChar* buffer, size_t maxlen);
	
	virtual void Close();

	SHVBool ReadLine8(SHVString8& line);
	SHVBool ReadLineUTF8(SHVStringUTF8& line);
	SHVBool ReadLine16(SHVString16& line);
	
private:

	SHVBool ReadIntoBuffer(size_t sz = 512);
	
#ifdef __SHIVA_POSIX
	int& Fd;
#elif defined(__SHIVA_WIN32)
	HANDLE& Fd;
#endif
	bool EofFlag;
	SHVBufferCStream ExcessData;
	size_t ExcessDataPos;
	inline size_t ExcessDataLeft() { return ExcessData.GetSize() - ExcessDataPos; }
};

//=========================================================================================================
// SHVSubProcessStreamOut class
class SHVSubProcessStreamOut : public SHVStreamOut
{
public:

#ifdef __SHIVA_POSIX
	SHVSubProcessStreamOut(int& fd);
#elif defined(__SHIVA_WIN32)
	SHVSubProcessStreamOut(HANDLE& fd);
#endif
	
	virtual SHVBool IsOk() const;
	virtual SHVBool WriteBuffer(const void* buffer, size_t len);

	virtual SHVBool WriteString16(const SHVWChar* buffer, size_t maxlen = SIZE_T_MAX);
	virtual SHVBool WriteChar16(SHVWChar ch);

	virtual SHVBool WriteString8(const SHVChar* buffer, size_t maxlen = SIZE_T_MAX);
	virtual SHVBool WriteChar8(const SHVChar ch);
	
	virtual SHVBool WriteStringUTF8(const SHVChar* buffer, size_t maxlen = SIZE_T_MAX);
	
	virtual void Close();

private:
#ifdef __SHIVA_POSIX
	int& Fd;
#elif defined(__SHIVA_WIN32)
	HANDLE& Fd;
#endif
};
///\endcond

#endif
