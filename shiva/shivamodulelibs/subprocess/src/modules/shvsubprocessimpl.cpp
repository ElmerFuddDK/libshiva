/*
 *   Copyright (C) 2008-2010 by Lars Eriksen
 *
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 of the
 *   License, or (at your option) any later version with the following
 *   exeptions:
 *
 *   1) Static linking to the library does not constitute derivative work
 *      and does not require the author to provide source code for the
 *      application.
 *      Compiling applications with the source code directly linked in is
 *      Considered static linking as well.
 *
 *   2) You do not have to provide a copy of the license with programs
 *      that are linked against this code.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "stdafx.h"
#include "../../../../include/platformspc.h"

#include "shvsubprocessimpl.h"
#include "../../../../include/utils/shvbuffer.h"

#ifdef __SHIVA_POSIX
# include <sys/types.h>
# include <unistd.h>
# include <signal.h>
# include <sys/wait.h>
# include <errno.h>
# include <sys/ioctl.h>
# include <fcntl.h>
#elif defined(__SHIVA_WIN32)
# include <io.h>
# include <errno.h>
# include <string.h>
# ifdef PSAPI_VERSION
#  include <psapi.h>
# else
#  pragma message("SubProcess does not support memusage - define PSAPI_VERSION=1 and link with psapi")
# endif
# define EWOULDBLOCK EAGAIN
#else
# error Not supported for current platform
#endif


//=========================================================================================================
// SHVSubProcessImpl class
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVSubProcessImpl::SHVSubProcessImpl() : SHVSubProcess(), LastError(ErrNone)
{
	NonBlocking = false;
#ifdef __SHIVA_POSIX
	StreamStdOut = new SHVSubProcessStreamIn(PipeStdOut[0]);
	StreamStdErr = new SHVSubProcessStreamIn(PipeStdErr[0]);
	StreamStdIn = new SHVSubProcessStreamOut(PipeStdIn[1]);
	PipeStdOut[0] = PipeStdOut[1] = PipeStdErr[0] = PipeStdErr[1] = PipeStdIn[0] = PipeStdIn[1] = 0;
	Pid = -1;
#elif defined(__SHIVA_WIN32)
	memset(&Process,0,sizeof(Process));

	///\todo Stream support on windows will duplicate all shareable handles to the sub process at the moment ...
	hStdOut = hStdErr = hStdIn = INVALID_HANDLE_VALUE;
	StreamStdOut = new SHVSubProcessStreamIn(hStdOut);
	StreamStdErr = new SHVSubProcessStreamIn(hStdErr);
	StreamStdIn = new SHVSubProcessStreamOut(hStdIn);
#endif
}

/*************************************
 * Destructor
 *************************************/
SHVSubProcessImpl::~SHVSubProcessImpl()
{
	WaitForTermination();
	delete StreamStdOut;
	delete StreamStdErr;
	delete StreamStdIn;
}

/*************************************
 * IsRunning
 *************************************/
SHVBool SHVSubProcessImpl::IsRunning()
{
#ifdef __SHIVA_POSIX
SHVBool retVal( Pid != -1 ? ErrNone : (LastError ? ErrGeneric : LastError) );
	if (retVal)
	{
	int status;
		if (waitpid(Pid,&status,WNOHANG) != 0)
		{
			waitpid(Pid,&status,0);
			Pid = -1;
			StreamStdOut->Close();
			StreamStdErr->Close();
			StreamStdIn->Close();
			retVal.SetError(ErrGeneric);
		}
	}
	return retVal;
#elif defined(__SHIVA_WIN32)
SHVBool retVal( Process.hProcess ? ErrNone : (LastError ? ErrGeneric : LastError) );
	if (retVal)
	{
		if (WaitForSingleObject(Process.hProcess,0) == WAIT_OBJECT_0)
		{
		DWORD exitCode;
			GetExitCodeProcess(Process.hProcess,&exitCode);
			CloseHandle(Process.hProcess);
			CloseHandle(Process.hThread);
			memset(&Process,0,sizeof(Process));
			StreamStdOut->Close();
			StreamStdErr->Close();
			StreamStdIn->Close();
			retVal.SetError(ErrGeneric);
		}
	}
	return retVal;
#else
	return ErrGeneric;
#endif
}

/*************************************
 * GetStdOut
 *************************************/
SHVStreamIn& SHVSubProcessImpl::GetStdOut()
{
	return *StreamStdOut;
}

/*************************************
 * GetStdErr
 *************************************/
SHVStreamIn& SHVSubProcessImpl::GetStdErr()
{
	return *StreamStdErr;
}

/*************************************
 * GetStdIn
 *************************************/
SHVStreamOut& SHVSubProcessImpl::GetStdIn()
{
	return *StreamStdIn;
}

/*************************************
 * GetPID
 *************************************/
SHVSubProcess::ProcessID SHVSubProcessImpl::GetPID()
{
#if defined(__SHIVA_WIN32)
	return (SHVSubProcess::ProcessID)Process.dwProcessId;
#elif defined(__SHIVA_LINUX)
	return (SHVSubProcess::ProcessID)Pid;
#else
	return 0;
#endif
}

/*************************************
 * GetMemUsage
 *************************************/
SHVInt64 SHVSubProcessImpl::GetMemUsage()
{
SHVInt64 retVal;
	if (IsRunning())
	{
#if defined(__SHIVA_WIN32) && defined(PSAPI_VERSION)
	PROCESS_MEMORY_COUNTERS pmc;
		if (GetProcessMemoryInfo(Process.hProcess, &pmc, sizeof(pmc)))
		{
			retVal = pmc.WorkingSetSize;
		}
#elif defined(__SHIVA_LINUX)
	unsigned long sz, resident, share, text, lib, data, dt;
	FILE* f;
	
		f = fopen(SHVString8C::Format("/proc/%d/statm",Pid).GetBufferConst(),"r");
	
		if(f)
		{
			if(7 == fscanf(f,"%ld %ld %ld %ld %ld %ld %ld", &sz,&resident,&share,&text,&lib,&data,&dt))
			{
				retVal = resident * ::getpagesize();
			}
			fclose(f);
		}
#else
		SHVASSERT(false); // unimplemented
#endif
	}
	return retVal;
}

/*************************************
 * Start
 *************************************/
SHVBool SHVSubProcessImpl::Start(const SHVStringC program, const SHVStringC args, int streams, bool nonBlocking)
{
SHVFileList argList;
	ParseArgs(argList,args);
	return Start(program,argList,streams,nonBlocking);
}

/*************************************
 * Start
 *************************************/
SHVBool SHVSubProcessImpl::Start(const SHVStringC program, SHVFileList& args, int streams, bool nonBlocking)
{
SHVBool retVal(IsRunning() ? ErrAlreadyRunning : ErrNone);
	
	if (retVal)
	{
#ifdef __SHIVA_POSIX

		if (streams&StdOut)
			pipe(PipeStdOut);
		if (streams&StdErr)
			pipe(PipeStdErr);
		if (streams&StdIn)
			pipe(PipeStdIn);
		
		if ((nonBlocking = NonBlocking))
		{
			fcntl(PipeStdIn[1], F_SETFL, O_NONBLOCK);
		}

		LastError.SetError(ErrNone);
		Pid = vfork(); ///\todo Use clone instead of vfork
		
		if (Pid == 0) // Then we are the sub-process
		{
		char** argList;
		SHVListPos pos = NULL;
		
			if (PipeStdIn[0])
			{
				dup2(PipeStdIn[0], 0); // stdin
				close(PipeStdIn[0]);
			}
			else
			{
				freopen("/dev/null", "r", stdin);
			}
			if (PipeStdOut[1])
			{
				dup2(PipeStdOut[1], 1); // stdout
				close(PipeStdOut[1]);
				setlinebuf(stdout);
			}
			if (PipeStdErr[1])
			{
				dup2(PipeStdErr[1], 2); // stderr
				close(PipeStdErr[1]);
				setlinebuf(stderr);
			}
#ifdef F_CLOSEM
			fcntl(3, F_CLOSEM);
#elif HAVE_CLOSEFROM
			closefrom(3);
#else
		int open_max = sysconf (_SC_OPEN_MAX);
			for (int i = 3; i < open_max; i++)
				close(i);
#endif
			SHVVERIFY(setpgid(0,0) == 0); // make our own process group
			argList = (char**)malloc(sizeof(char*)*(args.GetCount()+2));
			argList[0] = (char*)program.GetSafeBuffer();
			for (int i=1;args.MoveNext(pos);i++)
				argList[i] = (char*)args.GetAt(pos).GetSafeBuffer();
			argList[args.GetCount()+1] = NULL;
			
			execvp(argList[0],argList);
			// If we reach here it means the execute failed
			LastError.SetError(ErrUnknownProgram);
			_exit(-1);
		}
		else if (Pid == -1) // we failed to start the process
		{
			LastError.SetError(ErrGeneric);
			
			SafeCloseFd(PipeStdIn[0]);
			SafeCloseFd(PipeStdIn[1]);
			SafeCloseFd(PipeStdOut[0]);
			SafeCloseFd(PipeStdOut[1]);
			SafeCloseFd(PipeStdErr[0]);
			SafeCloseFd(PipeStdErr[1]);
		}
		else // we are the parent process
		{
			setpgid(Pid,Pid); // Release the sub process into it's own group
			// close the other end of the streams
			SafeCloseFd(PipeStdIn[0]);
			SafeCloseFd(PipeStdOut[1]);
			SafeCloseFd(PipeStdErr[1]);
			
			if (!LastError) // then execution failed of the external program
			{
				WaitForTermination();
			}
		}
#elif defined(__SHIVA_WIN32)
	SHVString cmdLine(program);
	SHVFileListIterator itr(args);
	STARTUPINFOW si;
	SECURITY_ATTRIBUTES sa;

		::memset(&si,0,sizeof(STARTUPINFOW));
		si.cb = sizeof(STARTUPINFOW);

		sa.nLength= sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = TRUE;

		if (streams&StdOut)
		{
		HANDLE hStdOutTmp;
			si.dwFlags = STARTF_USESTDHANDLES;
			::CreatePipe(&hStdOutTmp,&si.hStdOutput,&sa,0);
			::DuplicateHandle(GetCurrentProcess(),hStdOutTmp,GetCurrentProcess(),&hStdOut,0,FALSE,DUPLICATE_SAME_ACCESS);
			::CloseHandle(hStdOutTmp);
		}
		if (streams&StdErr)
		{
		HANDLE hStdErrTmp;
			si.dwFlags = STARTF_USESTDHANDLES;
			::CreatePipe(&hStdErrTmp,&si.hStdError,&sa,0);
			::DuplicateHandle(GetCurrentProcess(),hStdErrTmp,GetCurrentProcess(),&hStdErr,0,FALSE,DUPLICATE_SAME_ACCESS);
			::CloseHandle(hStdErrTmp);
		}
		if (streams&StdIn)
		{
		HANDLE hStdInTmp;
			si.dwFlags = STARTF_USESTDHANDLES;
			::CreatePipe(&si.hStdInput,&hStdInTmp,&sa,0);
			::DuplicateHandle(GetCurrentProcess(),hStdInTmp,GetCurrentProcess(),&hStdIn,0,FALSE,DUPLICATE_SAME_ACCESS);
			::CloseHandle(hStdInTmp);
			SHVASSERT(!nonBlocking);
		}
		SHVUNUSED_PARAM(nonBlocking);


		if (cmdLine.Find(_S(" ")) >= 0)
		{
			cmdLine.Replace(_S("\\"),_S("\\\\"));
			cmdLine.Replace(_S("\""),_S("\\\""));
			cmdLine = SHVStringC::Format(_S("\"%s\""),cmdLine.GetSafeBuffer());
		}

		while (itr.MoveNext())
		{
			if (itr.Get().Find(_S(" ")) >= 0)
			{
			SHVString arg(itr.Get());
				arg.Replace(_S("\\"),_S("\\\\"));
				arg.Replace(_S("\""),_S("\\\""));
				cmdLine += SHVStringC::Format(_S(" \"%s\""),arg.GetSafeBuffer());
			}
			else
			{
				cmdLine += _S(" ") + itr.Get();
			}
		}

# if __SHVSTRINGDEFAULT == 16
		if (::CreateProcessW(NULL,cmdLine.GetBufferWin32(),NULL,NULL,streams ? TRUE : FALSE,0,NULL,NULL,&si,&Process))
# else
	SHVString16 cmdLineW(cmdLine.ToStr16());
		if (::CreateProcessW(NULL,cmdLineW.GetBufferWin32(),NULL,NULL,streams ? TRUE : FALSE,0,NULL,NULL,&si,&Process))
# endif
		{
			LastError.SetError(ErrNone);
		}
		else
		{
			LastError.SetError(ErrGeneric);
		}

		if (streams&StdOut)
			::CloseHandle(si.hStdOutput);
		if (streams&StdErr)
			::CloseHandle(si.hStdError);
		if (streams&StdIn)
			::CloseHandle(si.hStdInput);
#endif
	
		retVal = LastError;
	}
	
	return retVal;
}

/*************************************
 * Shutdown
 *************************************/
void SHVSubProcessImpl::Shutdown()
{
	if (IsRunning())
	{
#ifdef __SHIVA_POSIX
		kill(Pid,SIGTERM);
#elif defined(__SHIVA_WIN32)
		///\todo Find a better way to shut down a windows process
		TerminateProcess(Process.hProcess,-1);
#else
		abort();
#endif
	}
}

/*************************************
 * Kill
 *************************************/
void SHVSubProcessImpl::Kill()
{
	if (IsRunning())
	{
#ifdef __SHIVA_POSIX
		kill(-Pid,SIGKILL);
		///\todo Run through the process tree and properly kill childs that have their own group
#elif defined(__SHIVA_WIN32)
		TerminateProcess(Process.hProcess,-1);
#else
		abort();
#endif
	}
}

/*************************************
 * WaitForTermination
 *************************************/
void SHVSubProcessImpl::WaitForTermination()
{
	if (IsRunning())
	{
#ifdef __SHIVA_POSIX
	int status;
		waitpid(Pid,&status,0);
		Pid = -1;
#elif defined(__SHIVA_WIN32)
	DWORD exitCode;
		WaitForSingleObject(Process.hProcess,INFINITE);
		GetExitCodeProcess(Process.hProcess,&exitCode);
		CloseHandle(Process.hProcess);
		CloseHandle(Process.hThread);
		memset(&Process,0,sizeof(Process));
#endif
		StreamStdOut->Close();
		StreamStdErr->Close();
		StreamStdIn->Close();
	}
}

/*************************************
 * ReadLine8
 *************************************/
SHVBool SHVSubProcessImpl::ReadLine8(SHVString8& line, Streams stream)
{
	switch (stream)
	{
	case StdOut:
		return StreamStdOut->ReadLine8(line);
	case StdErr:
		return StreamStdErr->ReadLine8(line);
	default:
		return ErrInvalidOperation;
	}
	
}

/*************************************
 * ReadLineUTF8
 *************************************/
SHVBool SHVSubProcessImpl::ReadLineUTF8(SHVStringUTF8& line, Streams stream)
{
	switch (stream)
	{
	case StdOut:
		return StreamStdOut->ReadLineUTF8(line);
	case StdErr:
		return StreamStdErr->ReadLineUTF8(line);
	default:
		return ErrInvalidOperation;
	}
	
}

/*************************************
 * ReadLine16
 *************************************/
SHVBool SHVSubProcessImpl::ReadLine16(SHVString16& line, Streams stream)
{
	switch (stream)
	{
	case StdOut:
		return StreamStdOut->ReadLine16(line);
	case StdErr:
		return StreamStdErr->ReadLine16(line);
	default:
		return ErrInvalidOperation;
	}
}

/*************************************
 * WriteLine8
 *************************************/
SHVBool SHVSubProcessImpl::WriteLine8(const SHVString8C line)
{
size_t strLen = line.GetLength();
	if (strLen && line.GetBufferConst()[strLen-1] == '\n')
		return StreamStdIn->WriteString8(line.GetSafeBuffer());
	else
		return StreamStdIn->WriteString8(SHVString8(line + "\n").GetSafeBuffer());
}

/*************************************
 * WriteLineUTF8
 *************************************/
SHVBool SHVSubProcessImpl::WriteLineUTF8(const SHVStringUTF8C line)
{
size_t strLen = line.GetLength();
	if (strLen && line.GetBufferConst()[strLen-1] == '\n')
		return StreamStdIn->WriteStringUTF8(line.GetSafeBuffer());
	else
		return StreamStdIn->WriteStringUTF8(SHVStringUTF8(line + "\n").GetSafeBuffer());
}

/*************************************
 * WriteLine16
 *************************************/
SHVBool SHVSubProcessImpl::WriteLine16(const SHVString16C line)
{
size_t strLen = line.GetLength();
static const SHVWChar newLine[] = { '\n', '\0' };

	if (strLen && line.GetBufferConst()[strLen-1] == '\n')
		return StreamStdIn->WriteString16(line.GetSafeBuffer());
	else
		return StreamStdIn->WriteString16(SHVString16(line + newLine).GetSafeBuffer());
}

/*************************************
 * ParseArgs
 *************************************/
void SHVSubProcessImpl::ParseArgs(SHVFileList& argList, const SHVStringC args)
{
const SHVTChar* c = args.GetSafeBuffer();
long pos;
SHVString val;
	Trim(c);
	while (*c)
	{
		if (*c == '"')
		{
		SHVStringC cStr(++c);
		bool escaping = false;
		bool running = true;

			val = SHVStringC(NULL);
			for (pos=0; c[pos] && running; pos++)
			{
				if (escaping)
				{
					escaping = false;
					switch (c[pos])
					{
					case '\\': val += _S("\\"); break;
					case 'n':  val += _S("\n"); break;
					case '"':  val += _S("\""); break;
					case 't':  val += _S("\t"); break;
					default:
						SHVASSERT(false);
						val += SHVStringC::Format(_S("\\%c"),c[pos]);
						break;
					}
				}
				else if (c[pos] == '\\')
				{
					escaping = true;
				}
				else if (c[pos] == '"')
				{
					running = false;
				}
				else
				{
					val.AddChars(c+pos,1);
				}
			}

			c += pos;
		}
		else
		{
		SHVStringC cStr(c);
			pos = cStr.Find(_S(" "));
			if (pos < 0)
			{
				val = cStr;
				c += val.GetLength();
			}
			else
			{
				val = cStr.Mid(0,(size_t)pos);
				c += val.GetLength();
			}
		}
		argList.AddTail(val);
		Trim(c);
	}
}

/*************************************
 * Trim
 *************************************/
void SHVSubProcessImpl::Trim(const SHVTChar*& ch)
{
	while (*ch)
	{
		switch (*ch)
		{
		case ' ':
		case '\t':
			ch++;
			break;
		default:
			return;
		}
	}
}


///\cond INTERNAL
#ifdef __SHIVA_POSIX
/*************************************
 * SafeCloseFd
 *************************************/
void SHVSubProcessImpl::SafeCloseFd(int& fd)
{
	if (fd)
	{
		close(fd);
		fd = 0;
	}
}
#endif

//=========================================================================================================
// SHVSubProcessStreamIn class
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
#ifdef __SHIVA_POSIX
SHVSubProcessStreamIn::SHVSubProcessStreamIn(int& fd) : Fd(fd)
#elif defined(__SHIVA_WIN32)
SHVSubProcessStreamIn::SHVSubProcessStreamIn(HANDLE& fd) : Fd(fd)
#endif
{
	EofFlag = false;
	ExcessDataPos = 0;
}

/*************************************
 * Eof
 *************************************/
bool SHVSubProcessStreamIn::Eof() const
{
bool retVal = EofFlag;
	((SHVSubProcessStreamIn*)this)->EofFlag = false;
	return (retVal || !IsOk());
}

/*************************************
 * IsOk
 *************************************/
SHVBool SHVSubProcessStreamIn::IsOk() const
{
	return (Fd ? SHVSubProcess::ErrNone : SHVSubProcess::ErrEof);
}

/*************************************
 * ReadBuffer
 *************************************/
SHVBool SHVSubProcessStreamIn::ReadBuffer(void* buffer, size_t bufferSize, size_t& actualLength)
{
SHVBool retVal(IsOk());

	actualLength = 0;
	
	if (retVal && bufferSize > 0)
	{
		if (ExcessDataLeft())
		{
		size_t bytesToRead = (bufferSize > ExcessDataLeft() ? ExcessDataLeft() : bufferSize);
		
			SHVVERIFY(ExcessData.ReadBytes((SHVByte*)buffer, bytesToRead, ExcessDataPos));
			actualLength = bytesToRead;
			ExcessData.Truncate(ExcessDataPos);
			
			EofFlag = EofFlag && !ExcessDataLeft();
		}
		else
		{
#ifdef __SHIVA_POSIX
		int bytesRead;
			bytesRead = (int)read(Fd,buffer,bufferSize);
			EofFlag = (bytesRead == 0);
#elif defined(__SHIVA_WIN32)
		DWORD bytesRead;
			EofFlag = ReadFile(Fd,buffer,(DWORD)bufferSize,&bytesRead,NULL) != TRUE;
			if (EofFlag && bytesRead < 0)
				bytesRead = 0;
#endif
			if (bytesRead < 0) // test for error
			{
				if (errno == EAGAIN || errno == EWOULDBLOCK)
				{
					EofFlag = true;
				}
				else
				{
					Close();
				}
				retVal.SetError(SHVSubProcess::ErrEof);
			}
			else if (bytesRead > 0)
			{
				actualLength = (size_t)bytesRead;
			}
			else
			{
				retVal.SetError(SHVSubProcess::ErrEof);
			}
		}
	}

	return retVal;
}

/*************************************
 * ReadString16
 *************************************/
SHVBool SHVSubProcessStreamIn::ReadString16(SHVWChar* buffer, size_t maxlen)
{
SHVBool retVal(SHVSubProcess::ErrNone);

	if (maxlen > 1)
	{
	size_t readBytes = 0;
	size_t bytesToRead = (maxlen-1)*sizeof(SHVWChar);
	
		if (ExcessDataLeft() < sizeof(SHVWChar))
			retVal = ReadIntoBuffer(bytesToRead - ExcessDataLeft());
	
		if (retVal && ExcessDataLeft() >= sizeof(SHVWChar))
		{
			readBytes = (bytesToRead > ExcessDataLeft() ? ExcessDataLeft()-(ExcessDataLeft()%sizeof(SHVWChar)) : bytesToRead);
		
			SHVVERIFY(ExcessData.ReadBytes((SHVByte*)buffer, readBytes, ExcessDataPos));
			ExcessData.Truncate(ExcessDataPos);
			
			EofFlag = EofFlag && !ExcessDataLeft();
		}
		
		buffer[readBytes/sizeof(SHVWChar)] = '\0';
	}
	else if (maxlen == 1)
	{
		buffer[0] = '\0';
	}
	
	return retVal;
}

/*************************************
 * ReadChar16
 *************************************/
SHVWChar SHVSubProcessStreamIn::ReadChar16()
{
SHVWChar retVal[2];
	retVal[0] = '\0';
	ReadString16(retVal,2);
	return retVal[0];
}

/*************************************
 * ReadString8
 *************************************/
SHVBool SHVSubProcessStreamIn::ReadString8(SHVChar* buffer, size_t maxlen)
{
SHVBool retVal(SHVSubProcess::ErrNone);

	if (maxlen > 1)
	{
	size_t readBytes = 0;
	size_t bytesToRead = (maxlen-1)*sizeof(SHVChar);
	
		if (ExcessDataLeft() < sizeof(SHVChar))
			retVal = ReadIntoBuffer(bytesToRead - ExcessDataLeft());
	
		if (retVal && ExcessDataLeft() >= sizeof(SHVChar))
		{
			readBytes = (bytesToRead > ExcessDataLeft() ? ExcessDataLeft()-(ExcessDataLeft()%sizeof(SHVChar)) : bytesToRead);
		
			SHVVERIFY(ExcessData.ReadBytes((SHVByte*)buffer, readBytes, ExcessDataPos));
			ExcessData.Truncate(ExcessDataPos);
			
			EofFlag = EofFlag && !ExcessDataLeft();
		}
		
		buffer[readBytes/sizeof(SHVChar)] = '\0';
	}
	else if (maxlen == 1)
	{
		buffer[0] = '\0';
	}
	
	return retVal;
}

/*************************************
 * ReadChar8
 *************************************/
SHVChar SHVSubProcessStreamIn::ReadChar8()
{
SHVChar retVal[2];
	retVal[0] = '\0';
	ReadString8(retVal,2);
	return retVal[0];
}

/*************************************
 * ReadStringUTF88
 *************************************/
SHVBool SHVSubProcessStreamIn::ReadStringUTF8(SHVChar* buffer, size_t maxlen)
{
	return ReadString8(buffer,maxlen);
}

/*************************************
 * ReadCharUTF8
 *************************************/
SHVChar SHVSubProcessStreamIn::ReadCharUTF8()
{
SHVChar retVal[2];
	retVal[0] = '\0';
	ReadString8(retVal,2);
	return retVal[0];
}

/*************************************
 * Close
 *************************************/
void SHVSubProcessStreamIn::Close()
{
	if (IsOk())
	{
#ifdef __SHIVA_POSIX
		close(Fd);
		Fd = 0;
#elif defined(__SHIVA_WIN32)
		CloseHandle(Fd);
		Fd = INVALID_HANDLE_VALUE;
#endif
	}
}

/*************************************
 * ReadLine8
 *************************************/
SHVBool SHVSubProcessStreamIn::ReadLine8(SHVString8& line)
{
SHVBool retVal(SHVSubProcess::ErrNone);
size_t newlinePos = SIZE_T_MAX;

	while (retVal && (newlinePos = ExcessData.SeekByte('\n',ExcessDataPos)) >= ExcessData.GetSize())
	{
		retVal = ReadIntoBuffer();
	}
	
	if (retVal && newlinePos < ExcessData.GetSize())
	{
	size_t newSize = newlinePos - ExcessDataPos;
		line.SetBufferSize((newSize+2)*sizeof(SHVChar));
		
		SHVVERIFY(ExcessData.ReadBytes((SHVByte*)line.GetBuffer(), (newSize+1)*sizeof(SHVChar), ExcessDataPos));
		line[newSize/sizeof(SHVChar)] = '\0';
		line.TrimRight("\r");
		ExcessData.Truncate(ExcessDataPos);
		
		EofFlag = EofFlag && !ExcessDataLeft();
	}
	
	return retVal;
}

/*************************************
 * ReadLineUTF8
 *************************************/
SHVBool SHVSubProcessStreamIn::ReadLineUTF8(SHVStringUTF8& line)
{
SHVBool retVal(SHVSubProcess::ErrNone);
size_t newlinePos = SIZE_T_MAX;

	while (retVal && (newlinePos = ExcessData.SeekByte('\n',ExcessDataPos)) >= ExcessData.GetSize())
	{
		retVal = ReadIntoBuffer();
	}
	
	if (retVal && newlinePos < ExcessData.GetSize())
	{
	size_t newSize = newlinePos - ExcessDataPos;
		line.SetBufferSize((newSize+2)*sizeof(SHVChar));
		
		SHVVERIFY(ExcessData.ReadBytes((SHVByte*)line.GetBuffer(), (newSize+1)*sizeof(SHVChar), ExcessDataPos));
		line.GetBuffer()[newSize/sizeof(SHVChar)] = '\0';
		line.TrimRight("\r");
		ExcessData.Truncate(ExcessDataPos);
		
		EofFlag = EofFlag && !ExcessDataLeft();
	}
	
	return retVal;
}

/*************************************
 * ReadLine16
 *************************************/
SHVBool SHVSubProcessStreamIn::ReadLine16(SHVString16& line)
{
SHVBool retVal(SHVSubProcess::ErrNone);
size_t newlinePos = SIZE_T_MAX;
static const SHVWChar carReturn[] = { '\r', '\0' };

	while (retVal && (newlinePos = ExcessData.SeekShort('\n',ExcessDataPos)) >= ExcessData.GetSize())
	{
		retVal = ReadIntoBuffer();
	}
	
	if (retVal && newlinePos < ExcessData.GetSize())
	{
	size_t newSize = newlinePos - ExcessDataPos;
		line.SetBufferSize((newSize+2)*sizeof(SHVWChar));
		
		SHVVERIFY(ExcessData.ReadBytes((SHVByte*)line.GetBuffer(), (newSize+1)*sizeof(SHVWChar), ExcessDataPos));
		line[newSize/sizeof(SHVWChar)] = '\0';
		line.TrimRight(carReturn);
		ExcessData.Truncate(ExcessDataPos);
		
		EofFlag = EofFlag && !ExcessDataLeft();
	}
	
	return retVal;
}

/*************************************
 * ReadIntoBuffer
 *************************************/
SHVBool SHVSubProcessStreamIn::ReadIntoBuffer(size_t sz)
{
SHVBool retVal(IsOk());
	
	if (retVal)
	{
	SHVBufferPtrRef buffer = new SHVBufferPtr();
#ifdef __SHIVA_POSIX
	int bytesRead;

		buffer->SetBufferSize(sz);

		bytesRead = (int)read(Fd,buffer->GetBuffer(),sz);
		EofFlag = (bytesRead == 0);
#elif defined(__SHIVA_WIN32)
	DWORD bytesRead;

		buffer->SetBufferSize(sz);

		EofFlag = ReadFile(Fd,buffer->GetBuffer(),(DWORD)sz,&bytesRead,NULL) != TRUE;
		if (EofFlag && bytesRead < 0)
			bytesRead = 0;
#endif
		if (bytesRead < 0) // test for error
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				EofFlag = true;
			}
			else
			{
				Close();
			}
			retVal.SetError(SHVSubProcess::ErrEof);
		}
		else if (bytesRead > 0)
		{
			if ((size_t)bytesRead < sz)
				buffer->SetBufferSize((size_t)bytesRead,(size_t)bytesRead);
			ExcessData.AddBuffer(buffer);
		}
		else
		{
			retVal.SetError(SHVSubProcess::ErrEof);
		}
	}
	
	return retVal;
}

//=========================================================================================================
// SHVSubProcessStreamOut class
//=========================================================================================================
/*************************************
 * Constructor
 *************************************/
#ifdef __SHIVA_POSIX
SHVSubProcessStreamOut::SHVSubProcessStreamOut(int& fd) : Fd(fd)
#elif defined(__SHIVA_WIN32)
SHVSubProcessStreamOut::SHVSubProcessStreamOut(HANDLE& fd) : Fd(fd)
#endif
{}

/*************************************
 * IsOk
 *************************************/
SHVBool SHVSubProcessStreamOut::IsOk() const
{
	return (Fd ? SHVSubProcess::ErrNone : SHVSubProcess::ErrEof);
}

/*************************************
 * WriteBuffer
 *************************************/
SHVBool SHVSubProcessStreamOut::WriteBuffer(const void* buffer, size_t len)
{
SHVBool retVal(IsOk());

	if (IsOk())
	{
#ifdef __SHIVA_POSIX
		write(Fd,buffer,len);
#else
	DWORD dwLen((DWORD)len);
	const char* chBuf((const char*)buffer);
		while (dwLen)
		{
		DWORD bytesWrote;
			if (!::WriteFile(Fd,chBuf,dwLen,&bytesWrote,NULL) && bytesWrote <= 0)
			{
				retVal = SHVBool::False;
				break;
			}
			dwLen -= bytesWrote;
			chBuf += bytesWrote;
		}
#endif
	}

	return retVal;
}

/*************************************
 * WriteString16
 *************************************/
SHVBool SHVSubProcessStreamOut::WriteString16(const SHVWChar* buffer, size_t maxlen)
{
	return (WriteBuffer(buffer,(maxlen == SIZE_T_MAX ? SHVString16C::StrLen(buffer) : maxlen)*sizeof(SHVWChar)));
}

/*************************************
 * WriteChar16
 *************************************/
SHVBool SHVSubProcessStreamOut::WriteChar16(SHVWChar ch)
{
	return (WriteBuffer(&ch,sizeof(SHVWChar)));
}

/*************************************
 * WriteString8
 *************************************/
SHVBool SHVSubProcessStreamOut::WriteString8(const SHVChar* buffer, size_t maxlen)
{
	return (WriteBuffer(buffer,(maxlen == SIZE_T_MAX ? SHVString8C::StrLen(buffer) : maxlen)*sizeof(SHVChar)));
}

/*************************************
 * WriteChar8
 *************************************/
SHVBool SHVSubProcessStreamOut::WriteChar8(const SHVChar ch)
{
	return (WriteBuffer(&ch,sizeof(SHVChar)));
}

/*************************************
 * WriteStringUTF8
 *************************************/
SHVBool SHVSubProcessStreamOut::WriteStringUTF8(const SHVChar* buffer, size_t maxlen)
{
	return (WriteBuffer(buffer,(maxlen == SIZE_T_MAX ? SHVStringUTF8C::StrLen(buffer) : maxlen)*sizeof(SHVChar)));
}

/*************************************
 * WriteCharUTF8
 *************************************/
SHVBool SHVSubProcessStreamOut::WriteCharUTF8(const SHVChar ch)
{
	return (WriteBuffer(&ch,sizeof(SHVChar)));
}

/*************************************
 * Close
 *************************************/
void SHVSubProcessStreamOut::Close()
{
	if (IsOk())
	{
#ifdef __SHIVA_POSIX
		close(Fd);
		Fd = 0;
#elif defined(__SHIVA_WIN32)
		CloseHandle(Fd);
		Fd = INVALID_HANDLE_VALUE;
#endif
	}
}
///\endcond
