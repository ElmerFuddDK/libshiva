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
#include "shiva/include/platformspc.h"

#include "shvsubprocessimpl.h"
#include "shiva/include/utils/shvbuffer.h"

#ifdef __SHIVA_POSIX
# include <sys/types.h>
# include <unistd.h>
# include <signal.h>
# include <sys/wait.h>
# include <errno.h>
# include <sys/ioctl.h>
# include <fcntl.h>
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
	StreamStdOut = new SHVSubProcessStreamIn(PipeStdOut[0]);
	StreamStdErr = new SHVSubProcessStreamIn(PipeStdErr[0]);
	StreamStdIn = new SHVSubProcessStreamOut(PipeStdIn[1]);
	PipeStdOut[0] = PipeStdOut[1] = PipeStdErr[0] = PipeStdErr[1] = PipeStdIn[0] = PipeStdIn[1] = 0;
	NonBlocking = false;
#ifdef __SHIVA_POSIX
	Pid = -1;
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
 * Start
 *************************************/
SHVBool SHVSubProcessImpl::Start(const SHVStringC program, const SHVStringC args, int streams, bool nonBlocking)
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
		SHVString argStr(args);
		SHVString script;
		
			if (PipeStdIn[0])
				dup2(PipeStdIn[0], 0); // stdin
			if (PipeStdOut[1])
				dup2(PipeStdOut[1], 1); // stdout
			if (PipeStdErr[1])
				dup2(PipeStdErr[1], 2); // stderr
			
			argStr.Replace("\"","\\\"");
			script.Format("echo \"%s\" | xargs \"%s\"", argStr.GetSafeBuffer(), program.GetSafeBuffer());
			
			argList = (char**)malloc(sizeof(char*)*4);
			argList[0] = (char*)"sh";
			argList[1] = (char*)"-c";
			argList[2] = (char*)script.GetSafeBuffer();
			argList[3] = NULL;
			
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
			// close the other end of the streams
			SafeCloseFd(PipeStdIn[0]);
			SafeCloseFd(PipeStdOut[1]);
			SafeCloseFd(PipeStdErr[1]);
			
			if (LastError) // then execution failed of the external program
			{
				WaitForTermination();
			}
		}
		
#endif
	
		retVal = LastError = IsRunning();
	}
	
	return retVal;
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
				dup2(PipeStdIn[0], 0); // stdin
			if (PipeStdOut[1])
				dup2(PipeStdOut[1], 1); // stdout
			if (PipeStdErr[1])
				dup2(PipeStdErr[1], 2); // stderr
			
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
			// close the other end of the streams
			SafeCloseFd(PipeStdIn[0]);
			SafeCloseFd(PipeStdOut[1]);
			SafeCloseFd(PipeStdErr[1]);
			
			if (!LastError) // then execution failed of the external program
			{
				WaitForTermination();
			}
		}
		
#endif
	
		retVal = LastError = IsRunning();
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
		kill(Pid,SIGKILL);
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
SHVSubProcessStreamIn::SHVSubProcessStreamIn(int& fd) : Fd(fd)
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
		int bytesRead;
			bytesRead = (int)read(Fd,buffer,bufferSize);
			EofFlag = (bytesRead == 0);
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
 * Close
 *************************************/
void SHVSubProcessStreamIn::Close()
{
	if (IsOk())
	{
		close(Fd);
		Fd = 0;
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
	int bytesRead;

		buffer->SetBufferSize(sz);

		bytesRead = (int)read(Fd,buffer->GetBuffer(),sz);
		EofFlag = (bytesRead == 0);
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
SHVSubProcessStreamOut::SHVSubProcessStreamOut(int& fd) : Fd(fd)
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
		write(Fd,buffer,len);
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
 * Close
 *************************************/
void SHVSubProcessStreamOut::Close()
{
	if (IsOk())
	{
		close(Fd);
		Fd = 0;
	}
}
///\endcond
