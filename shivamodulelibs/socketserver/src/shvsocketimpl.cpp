/*
 *   Copyright (C) 2008 by Lars Eriksen
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
#include "../../../include/platformspc.h"


#include "../include/shvsocketserverimpl.h"

#ifdef __SHIVA_WIN32
# define MSG_DONTWAIT 0
# define MSG_NOSIGNAL 0
# ifndef __SHIVA_WINCE
#  include <fcntl.h>
#  include <errno.h>
# endif
#else
# include <netinet/in.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <sys/ioctl.h>
# include <fcntl.h>
# include <errno.h>
#endif


//=========================================================================================================
// SHVSocketImpl class - Implementation of sockets interface
//=========================================================================================================

///\cond INTERNAL
/*************************************
 * Constructor
 *************************************/
SHVSocketImpl::SHVSocketImpl(SHVEventSubscriberBase* subs, SHVSocketServerImpl* socketServer, SHVSocket::Types type, SHVSOCKTYPE sock)
	: SHVSocket(socketServer->GetModules().CreateTag(),SHVSocket::StateConnected), EventSubscriber(subs), Type(type)
{
	SocketServer = socketServer;
	Socket = sock;
	RecvPending = false;
	BytesRead = 0;
	
	BufferSize = DefaultBufferSize;

#ifdef __SHIVA_WINCE
	IsPending = false;
	StartReadThread();
#endif
}
SHVSocketImpl::SHVSocketImpl(SHVEventSubscriberBase* subs, SHVSocketServerImpl* socketServer, SHVSocket::Types type)
	: SHVSocket(socketServer->GetModules().CreateTag(),SHVSocket::StateNone), EventSubscriber(subs), Type(type)
{
	SocketServer = socketServer;
	RecvPending = false;
	BytesRead = 0;
	
	BufferSize = DefaultBufferSize;
	
#ifdef __SHIVA_WINCE
	IsPending = false;
#endif

	if (!subs)
	{
		Socket = InvalidSocket;
		State = StateError;
		Error.SetError(SHVSocket::ErrInvalidSubscriber);
		return;
	}

#ifdef __SHIVA_WIN32
LINGER linger;
BOOL noDelay;
#elif defined __SHIVA_LINUX
linger _linger;
#endif

	Socket = ::socket( PF_INET, (Type == SHVSocket::TypeTCP ? SOCK_STREAM : SOCK_DGRAM), 0 );

	if (Type == SHVSocket::TypeTCP)
	{
#if defined __SHIVA_WIN32
		linger.l_linger = 1;
		linger.l_onoff  = true;
	
		if (Socket != InvalidSocket)
		{
		unsigned long nonblock = 1;
			::setsockopt(Socket, SOL_SOCKET, SO_LINGER, (char*)&linger, sizeof(LINGER));
			noDelay = true;
			
			::setsockopt(Socket, IPPROTO_TCP, TCP_NODELAY, (char*)&noDelay, sizeof(BOOL));
			::ioctlsocket(Socket, FIONBIO, &nonblock);
		}
#elif defined __SHIVA_LINUX
		_linger.l_linger = 1;
		_linger.l_onoff  = true;
	
		if (Socket != InvalidSocket)
		{
			::setsockopt(Socket, SOL_SOCKET, SO_LINGER, (char*)&_linger, sizeof(linger));
			::fcntl(Socket,F_SETFL,O_NONBLOCK);
		}
#endif
	}
}


/*************************************
 * Destructor
 *************************************/
SHVSocketImpl::~SHVSocketImpl()
{
	if (Socket != InvalidSocket)
	{
		if (State == StateConnected)
			::shutdown(Socket, 2); // close both recv and send
#ifdef __SHIVA_WIN32
		::closesocket(Socket);
#else
		::close(Socket);
#endif
	}

#ifdef __SHIVA_WINCE
	Socket = InvalidSocket;
	while (ReadThread.IsRunning())
	{
		ReadThreadSignal.Unlock();
		SHVThreadBase::Sleep(10);
	}
#endif
}
///\endcond

/*************************************
 * GetType
 *************************************/
/// Returns socket type
SHVSocket::Types SHVSocketImpl::GetType()
{
	return Type;
}

/*************************************
 * BindAndListen
 *************************************/
SHVBool SHVSocketImpl::BindAndListen(SHVIPv4Port port)
{
SHVBool retVal;

	SocketServer->SocketServerLock.Lock();
	if (Type == SHVSocket::TypeTCP && State == SHVSocket::StateNone)
	{
#ifdef __SHIVA_WIN32
	SOCKADDR_IN sockAddr;
#else
	sockaddr_in sockAddr;
#endif
	int status;
		
		retVal = SHVSocket::ErrNone;
		
		sockAddr.sin_port   = htons(port);
		sockAddr.sin_family = AF_INET;
#ifdef __SHIVA_WIN32
		sockAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
		status = ::bind(Socket, (LPSOCKADDR) &sockAddr, sizeof(SOCKADDR_IN));
#else
		sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		status = ::bind(Socket, (sockaddr*) &sockAddr, sizeof(sockaddr_in));
#endif
	
		if (!status)
		{
			status = ::listen(Socket, 5);
			
			if (!status)
			{
				State = SHVSocket::StateListening;
#ifdef __SHIVA_WINCE
				StartReadThread();
#endif
				retVal = SHVBool::True;
			}
			else
			{
				retVal = SetError(SHVSocket::ErrListening);
			}
		}
		else
		{
			retVal = SetError(SHVSocket::ErrBinding);
		}
	}
	else
	{
		retVal = SHVSocket::ErrInvalidOperation;
	}
	SocketServer->SocketServerLock.Unlock();
	
	if (retVal == SHVBool::True)
	{
		SocketServer->SocketServerThread.SignalDispatcher();
	}
	
	return retVal;
}

/*************************************
 * Send
 *************************************/
SHVBool SHVSocketImpl::Send(const SHVBufferC& buf)
{
SHVBool retVal(SHVSocket::ErrInvalidOperation);
	
	SocketServer->SocketServerLock.Lock();
	if (State == SHVSocket::StateConnected)
	{
		if (::send(Socket,buf.GetBufferConst(),(int)buf.GetSize(),MSG_NOSIGNAL) <= 0)
		{
			Close();
			retVal = SetError(SHVSocket::ErrGeneric);
		}
		else
		{
			retVal = SHVSocket::ErrNone;
		}
	}
	SocketServer->SocketServerLock.Unlock();
	
	return retVal;
}

/*************************************
 * SetReceiveBufferSize
 *************************************/
SHVBool SHVSocketImpl::SetReceiveBufferSize(size_t sz)
{
	BufferSize = sz;
	return SHVBool::True;
}

/*************************************
 * GetReceiveBufferSize
 *************************************/
size_t SHVSocketImpl::GetReceiveBufferSize()
{
	return BufferSize;
}

/*************************************
 * GetReceiveBuffer
 *************************************/
SHVBuffer* SHVSocketImpl::PopReceiveBuffer(size_t& bytesRead)
{
SHVBuffer* retVal = NULL;

	SocketServer->SocketServerLock.Lock();
	if (ReceiveBuffers.GetCount())
	{
		retVal = ReceiveBuffers.GetFirst().Buffer.ReleaseReference();
		bytesRead = ReceiveBuffers.GetFirst().BytesRead;
		
		ReceiveBuffers.RemoveHead();
		BytesRead -= bytesRead;
	}
	else
	{
		bytesRead = 0;
	}
	SocketServer->SocketServerLock.Unlock();
	
	return retVal;
}

/*************************************
 * HasReceivedData
 *************************************/
SHVBool SHVSocketImpl::HasReceivedData()
{
	return (ReceiveBuffers.GetCount() ? SHVBool::True : SHVBool::False);
}

/*************************************
 * Close
 *************************************/
SHVBool SHVSocketImpl::Close()
{
	SocketServer->SocketServerLock.Lock();
	if (Socket != InvalidSocket)
	{
	int tempUnlocks;
	SHVSOCKTYPE sock = Socket;
	States oldState = State;

		Socket = InvalidSocket;
		SetError(SHVSocket::ErrClosed);
		tempUnlocks = SocketServer->SocketServerLock.UnlockAll(); // totally free the lock since this close might block
		
		if (oldState == StateConnected)
			::shutdown(sock, 2); // close both recv and send
#ifdef __SHIVA_WIN32
		::closesocket(sock);
#else
		::close(sock);
#endif
		
		SocketServer->SocketServerLock.LockMultiple(tempUnlocks);
	}
	SocketServer->SocketServerLock.Unlock();
	
	return SHVBool::True;
}

/*************************************
 * Connect
 *************************************/
SHVBool SHVSocketImpl::Connect(SHVIPv4Addr ip, SHVIPv4Port port)
{
SHVBool retVal(SHVSocket::ErrInvalidOperation);

	SocketServer->SocketServerLock.Lock();
	if (State == SHVSocket::StateNone)
	{
	sockaddr_in host;
	int result;

		host.sin_family=AF_INET;
		host.sin_addr.s_addr = ip;
		host.sin_port = htons(port);
		
		result = ::connect(Socket, (sockaddr*)&host, sizeof(sockaddr_in));
#ifdef __SHIVA_WIN32
		if (result == -1 && WSAGetLastError() != WSAEWOULDBLOCK) // failure
#else
		if (result == -1 && errno != EINPROGRESS) // failure
#endif
		{
#ifndef __SHIVA_WIN32
			printf("SOCKET CONNECT Err: %d\n", errno);
#endif
			Close();
			retVal = SetError(SHVSocket::ErrGeneric);
		}
		else
		{
			State = SHVSocket::StateConnecting;
			retVal.SetError(SHVSocket::ErrNone);
#ifdef __SHIVA_WINCE
			StartReadThread();
#endif
		}
	}
	SocketServer->SocketServerLock.Unlock();
	
	if (retVal == SHVBool::True)
	{
		SocketServer->SocketServerThread.SignalDispatcher();
	}
	
	return retVal;
}
SHVBool SHVSocketImpl::Connect(const SHVStringC ipv4Addr, SHVIPv4Port port)
{
	return Connect(SocketServer->Inetv4Addr(ipv4Addr),port);
}



///\cond INTERNAL
class SHVEventSocket : public SHVEvent
{
public:
	
	SHVEventSocket(SHVEventTarget* caller, SHVInt id, SHVInt subid, SHVRefObject* obj, SHVSocket* newSock);
	virtual void* GetData();
	
private:
	SHVSocketRef NewSocket;
};

SHVEventSocket::SHVEventSocket(SHVEventTarget* caller, SHVInt id, SHVInt subid, SHVRefObject* obj, SHVSocket* newSock)
	: SHVEvent(caller,id,subid,obj), NewSocket(newSock)
{
}

void* SHVEventSocket::GetData()
{
	return (SHVSocket*)NewSocket;
}


/*************************************
 * PerformEvent
 *************************************/
void SHVSocketImpl::PerformEvent()
{
SHVEventQueue* queue = NULL;
SHVSocketImplRef sockObj;
SHVBuffer* bufPtr;
SHVSocketRef retValObj;
int evCode = SHVSocketServer::EventSockStatus;
bool doEvent = true;
int sockerr;
int sz;

	// After we got this lock DO NOT PERFORM ANYTHING BLOCKING OR SOMETHING THAT INVOLVES OTHER LOCKS!
	SocketServer->SocketServerLock.Lock();
	
	// Handle the event, collecting data for the emission after unlocking
	switch  (State)
	{
	case SHVSocket::StateListening:
		sockObj = new SHVSocketImpl(EventSubscriber,SocketServer,Type,::accept(Socket, NULL, NULL));
		sockObj->SetReceiveBufferSize(BufferSize);
		if (sockObj->Socket < 0)
		{
			sockObj->Socket = InvalidSocket;
			sockObj->SetError(SHVSocket::ErrGeneric);
		}
		else
		{
			evCode = SHVSocketServer::EventSockIncomingConn;
			SocketServer->AddToList(sockObj);
		}
		retValObj = sockObj;
		break;
	case SHVSocket::StateConnecting:
		sockerr = RetreiveSocketError();
		if (sockerr)
		{
			Close();
			SetError(SHVSocket::ErrGeneric);
		}
		else
		{
			State = SHVSocket::StateConnected;
		}
		break;
	case SHVSocket::StateConnected: // received something
		bufPtr = new SHVBufferPtr();
		bufPtr->SetBufferSize(BufferSize);

#ifdef __SHIVA_WIN32
		sz = ::recv(Socket,(char*)bufPtr->GetBufferAsVoid(),(int)BufferSize,MSG_DONTWAIT);
#else
		sz = ::recv(Socket,bufPtr->GetBufferAsVoid(),BufferSize,MSG_DONTWAIT);
#endif
		
		if (sz<=0) // error
		{
			delete bufPtr;
			Close();
			SetError(SHVSocket::ErrGeneric);
		}
		else
		{
			ReceiveBuffers.AddTail(BufferInstance(bufPtr,(size_t)sz));
			BytesRead += (size_t)sz;
			evCode = SHVSocketServer::EventSockDataRead;
		}
		break;
	default:
		Close();
		SetError(SHVSocket::ErrGeneric);
		break;
	}
	
	SocketServer->SocketServerLock.Unlock();
	
	if (doEvent)
	{
		queue = EventSubscriber->Emit(SocketServer->GetModules(), new SHVEventSocket(SocketServer,evCode,Tag,this,retValObj));
	}
	if (queue)
	{
		queue->SignalDispatcher();
	}

#ifdef __SHIVA_WINCE
	IsPending = false;
	ReadThreadSignal.Unlock();
#endif
}

/*************************************
 * SetError
 *************************************/
SHVBool SHVSocketImpl::SetError(SHVBool err)
{
	State = StateError;
	SocketServer->RemoveFromList(this);
	return Error = err;
}

/*************************************
 * RetreiveSocketError
 *************************************/
int SHVSocketImpl::RetreiveSocketError()
{
int sockerr;
#ifdef _WIN32
int len = sizeof(sockerr);
	getsockopt(Socket, SOL_SOCKET, SO_ERROR, (char *)&sockerr, &len);
#else
socklen_t len = sizeof(sockerr);
	getsockopt(Socket, SOL_SOCKET, SO_ERROR, &sockerr, &len);
#endif
	return sockerr;
}


#ifdef __SHIVA_WINCE
void SHVSocketImpl::StartReadThread()
{
	ReadThread.Start(this,&SHVSocketImpl::ReadThreadFunc);
}

void SHVSocketImpl::ReadThreadFunc()
{
fd_set rfds;
fd_set wfds;
fd_set* fds;
int nextFD, retVal;

	while (GetState() != SHVSocket::StateError && Socket != SHVSocketImpl::InvalidSocket)
	{
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);
		fds = (GetState() == SHVSocket::StateConnecting ? &wfds : &rfds);
		FD_SET(Socket, fds);
		nextFD = Socket+1;

		retVal = select(nextFD, &rfds, &wfds, NULL, NULL);

		if (retVal == -1) // ERROR!
		{
			Close();
			SetError();
			IsPending = true;
		}
		else if (FD_ISSET(Socket,fds))
		{
			IsPending = true;
			ReadThreadSignal.Lock(); // request signal
			SocketServer->SocketServerThread.SignalDispatcher(); // signal the socket server thread

			ReadThreadSignal.Lock();
			ReadThreadSignal.Unlock();
		}
	}
}
#endif
///\endcond
