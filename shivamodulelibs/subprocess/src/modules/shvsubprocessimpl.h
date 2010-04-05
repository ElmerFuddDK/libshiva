#ifndef __SHIVA_SUBPROCESSIMPL_SUBPROCESS_H
#define __SHIVA_SUBPROCESSIMPL_SUBPROCESS_H

#include "shiva/include/modules/subprocess/shvsubprocess.h"
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


	// Startup and shutdown
	virtual SHVBool Start(const SHVStringC program, const SHVStringC args = NULL, int streams = StreamDefaults, bool nonBlocking = false);
	virtual SHVBool Start(const SHVStringC program, SHVFileList& args, int streams = StreamDefaults, bool nonBlocking = false);
	virtual void Shutdown();
	virtual void Kill();
	virtual void WaitForTermination();


	// Convenience functions
	virtual SHVBool ReadLine8(SHVString8& line, Streams stream = StdOut);
	virtual SHVBool ReadLine16(SHVString16& line, Streams stream = StdOut);
	virtual SHVBool WriteLine8(const SHVString8C line);
	virtual SHVBool WriteLine16(const SHVString16C line);


private:
	///\cond INTERNAL
	int PipeStdOut[2], PipeStdErr[2], PipeStdIn[2];
	SHVSubProcessStreamIn *StreamStdOut, *StreamStdErr;
	SHVSubProcessStreamOut *StreamStdIn;
	SHVBool LastError;
	bool NonBlocking;
#ifdef __SHIVA_LINUX
	pid_t Pid;

	static void SafeCloseFd(int& fd);
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

	SHVSubProcessStreamIn(int& fd);
	
	virtual bool Eof() const;
	virtual SHVBool IsOk() const;
	virtual SHVBool ReadBuffer(void* buffer, size_t bufferSize, size_t& actualLength);

	virtual SHVBool ReadString16(SHVWChar* buffer, size_t maxlen);
	virtual SHVWChar ReadChar16();

	virtual SHVBool ReadString8(SHVChar* buffer, size_t maxlen);
	virtual SHVChar ReadChar8();
	virtual void Close();

	SHVBool ReadLine8(SHVString8& line);
	SHVBool ReadLine16(SHVString16& line);
	
private:

	SHVBool ReadIntoBuffer(size_t sz = 512);
	
	int& Fd;
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

	SHVSubProcessStreamOut(int& fd);
	
	virtual SHVBool IsOk() const;
	virtual SHVBool WriteBuffer(const void* buffer, size_t len);

	virtual SHVBool WriteString16(const SHVWChar* buffer, size_t maxlen = SIZE_T_MAX);
	virtual SHVBool WriteChar16(SHVWChar ch);

	virtual SHVBool WriteString8(const SHVChar* buffer, size_t maxlen = SIZE_T_MAX);
	virtual SHVBool WriteChar8(const SHVChar ch);
	virtual void Close();

private:
	int& Fd;
};
///\endcond

#endif
