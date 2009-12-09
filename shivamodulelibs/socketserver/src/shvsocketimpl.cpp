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
# include <mstcpip.h>
#else
# include <netinet/in.h>
# include <netinet/tcp.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <sys/ioctl.h>
# include <fcntl.h>
# include <errno.h>
#endif

#define __SHVSOCKET_SENDTIMEOUT 10


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

#ifdef __SHIVASOCKETS_NOSELECTMODE
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
	
#ifdef __SHIVASOCKETS_NOSELECTMODE
	IsPending = false;
#endif

	if (!subs)
	{
		Socket = InvalidSocket;
		State = StateError;
		Error.SetError(SHVSocket::ErrInvalidSubscriber);
		return;
	}

	Socket = ::socket( PF_INET, (Type == SHVSocket::TypeTCP ? SOCK_STREAM : SOCK_DGRAM), 0 );

	if (Socket != InvalidSocket)
	{
#ifdef __SHIVA_WIN32
		unsigned long nonblock = 1;
		::ioctlsocket(Socket, FIONBIO, &nonblock);
#elif defined __SHIVA_LINUX
		::fcntl(Socket,F_SETFL,O_NONBLOCK);
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

#ifdef __SHIVASOCKETS_NOSELECTMODE
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
	if (State == SHVSocket::StateNone)
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
	
		if (Type == SHVSocket::TypeTCP)
		{
			if (!status)
			{
				status = ::listen(Socket, 5);
				
				if (!status)
				{
					State = SHVSocket::StateListening;
#ifdef __SHIVASOCKETS_NOSELECTMODE
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
			if (!status)
			{
				State = SHVSocket::StateConnected;
#ifdef __SHIVASOCKETS_NOSELECTMODE
					StartReadThread();
#endif
				retVal = SHVBool::True;
			}
			else
			{
				retVal = SetError(SHVSocket::ErrBinding);
			}
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
	int err = 0;

		for (int i=0; i<__SHVSOCKET_SENDTIMEOUT*10 && err == 0; i++)
		{
			err = ::send(Socket,buf.GetBufferConst(),(int)buf.GetSize(),MSG_NOSIGNAL);
			if (err < 0)
			{
#ifdef __SHIVA_WIN32
				err = WSAGetLastError();
				if (err == WSAEWOULDBLOCK)
#else
				err = errno;
				if (err == EAGAIN || err == EWOULDBLOCK)
#endif
				{
					SHVThreadBase::Sleep(100);
					err = 0;
				}
				else
				{
					err = -1;
				}
			}
			else
			{
				SHVASSERT(err == (int)buf.GetSize());
				err = 0;
				break;
			}
		}
	
		if (err)
		{
			Close();
			retVal = SetError(SHVSocket::ErrGeneric);
		}
		else
		{
			retVal.SetError(SHVSocket::ErrNone);
		}
	}
	SocketServer->SocketServerLock.Unlock();
	
	return retVal;
}

/*************************************
 * SendTo
 *************************************/
SHVBool SHVSocketImpl::SendTo(const SHVBufferC& buf, SHVIPv4Addr ip, SHVIPv4Port port)
{
SHVBool retVal(SHVSocket::ErrInvalidOperation);
	
	SocketServer->SocketServerLock.Lock();
	if (Type == TypeUDP && (State == SHVSocket::StateConnected || State == SHVSocket::StateNone))
	{
	sockaddr_in host;
	int err = 0;

		host.sin_family=AF_INET;
		host.sin_addr.s_addr = ip;
		host.sin_port = htons(port);
		
		for (int i=0; i<__SHVSOCKET_SENDTIMEOUT*10 && err == 0; i++)
		{
			err = ::sendto(Socket,buf.GetBufferConst(),(int)buf.GetSize(),MSG_NOSIGNAL,(sockaddr*) &host,sizeof(sockaddr_in));
			if (err < 0)
			{
#ifdef __SHIVA_WIN32
				err = WSAGetLastError();
				if (err == WSAEWOULDBLOCK)
#else
				err = errno;
				if (err == EAGAIN || err == EWOULDBLOCK)
#endif
				{
					SHVThreadBase::Sleep(100);
					err = 0;
				}
				else
				{
					err = -1;
				}
			}
			else
			{
				SHVASSERT(err == (int)buf.GetSize());
				err = 0;
				break;
			}
		}
		if (err)
		{
			Close();
			retVal = SetError(SHVSocket::ErrGeneric);
		}
		else
		{
			retVal.SetError(SHVSocket::ErrNone);
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
SHVIPv4Addr fromIP;
SHVIPv4Port fromPort;
	return PopReceiveBuffer(bytesRead,fromIP,fromPort);
}
SHVBuffer* SHVSocketImpl::PopReceiveBuffer(size_t& bytesRead, SHVIPv4Addr &fromIP, SHVIPv4Port &fromPort)
{
SHVBuffer* retVal = NULL;

	SocketServer->SocketServerLock.Lock();
	if (ReceiveBuffers.GetCount())
	{
		retVal = ReceiveBuffers.GetFirst().Buffer.ReleaseReference();
		bytesRead = ReceiveBuffers.GetFirst().BytesRead;
		fromIP = ReceiveBuffers.GetFirst().FromIP;
		fromPort = ReceiveBuffers.GetFirst().FromPort;
		
		ReceiveBuffers.RemoveHead();
		BytesRead -= bytesRead;
	}
	else
	{
		fromIP = InvalidIPv4;
		fromPort = 0;
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
 * SetSocketOption
 *************************************/
SHVBool SHVSocketImpl::SetSocketOption(SocketOptions option, int val1, int val2)
{
int level, optname;
SHVBool retVal(SHVSocket::ErrGeneric);

	switch (option)
	{
	case SHVSocket::SockOptKeepalive:
		level = SOL_SOCKET;
		optname = SO_KEEPALIVE;
		break;
#ifdef __SHIVA_WIN32
	case SHVSocket::SockOptKeepaliveIdle:
		{
		DWORD bytesReturned;
		tcp_keepalive data = { 1, 0, 1000 };
			data.keepalivetime = val1*1000;
			if (val2 > 0)
				data.keepaliveinterval = val2*1000;

			retVal = SHVBool(WSAIoctl(Socket,SIO_KEEPALIVE_VALS,&data,sizeof(tcp_keepalive),NULL,0,&bytesReturned,NULL,NULL) == 0 ? SHVBool::True : SHVSocket::ErrGeneric);
		}
		return retVal;
#else
	case SHVSocket::SockOptKeepaliveIdle:
		level = IPPROTO_TCP;
		optname = TCP_KEEPIDLE;
		if (val2 > 0)
		{
			retVal = SHVBool(::setsockopt(Socket, IPPROTO_TCP, TCP_KEEPINTVL, (char*)&val2, sizeof(int)) == 0 ? SHVSocket::ErrNone : SHVSocket::ErrGeneric);
			if (!retVal)
				return retVal;
		}
		break;
#endif
	case SHVSocket::SockOptLinger:
		{
#ifdef __SHIVA_WIN32
		LINGER ling;
#elif defined __SHIVA_LINUX
		linger ling;
#endif
			ling.l_linger = val1;
			ling.l_onoff  = val2;
			return (::setsockopt(Socket, SOL_SOCKET, SO_LINGER, (char*)&ling, sizeof(ling)) == 0 ? SHVSocket::ErrNone : SHVSocket::ErrGeneric);
		}
		break;
	case SHVSocket::SockOptTcpNodelay:
		if (GetType() != SHVSocket::TypeTCP)
			return SHVSocket::ErrInvalidOperation;
		level = IPPROTO_TCP;
		optname = TCP_NODELAY;
		break;
	case SHVSocket::SockOptReuseAddr:
		level = SOL_SOCKET;
		optname = SO_REUSEADDR;
		break;
	default:
		return retVal;
	}

	retVal = SHVBool(::setsockopt(Socket, level, optname, (char*)&val1, sizeof(int)) == 0 ? SHVSocket::ErrNone : SHVSocket::ErrGeneric);

	return retVal;
}

/*************************************
 * GetSocketOption
 *************************************/
SHVBool SHVSocketImpl::GetSocketOption(SocketOptions option, int& val1, int& val2)
{
int level, optname, val;
SHVBool retVal(SHVSocket::ErrGeneric);
#ifdef __SHIVA_LINUX
socklen_t len;
#else
int len;
#endif

	val1 = val2 = 0;

	switch (option)
	{
	case SHVSocket::SockOptKeepalive:
		level = SOL_SOCKET;
		optname = SO_KEEPALIVE;
		break;
#ifdef __SHIVA_WIN32
	case SHVSocket::SockOptKeepaliveIdle:
		return retVal;
#else
	case SHVSocket::SockOptKeepaliveIdle:
		level = IPPROTO_TCP;
		optname = TCP_KEEPIDLE;
		break;
#endif
	case SHVSocket::SockOptLinger:
		{
		linger ling;
			len = sizeof(linger);
			if (::getsockopt(Socket, SOL_SOCKET, SO_LINGER, (char*)&ling, &len) == 0)
			{
				val1 = ling.l_linger;
				val2 = ling.l_onoff;
				retVal = SHVBool::True;
			}

			return retVal;
		}
		break;
	case SHVSocket::SockOptTcpNodelay:
		level = IPPROTO_TCP;
		optname = TCP_NODELAY;
		break;
	case SHVSocket::SockOptReuseAddr:
		level = SOL_SOCKET;
		optname = SO_REUSEADDR;
		break;
	default:
		return retVal;
	}

	len = sizeof(int);
	if (::getsockopt(Socket, level, optname, (char*)&val, &len) == 0)
	{
		val1 = val;
		retVal = SHVBool::True;
	}

	return retVal;
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
SHVBool SHVSocketImpl::ConnectAny(SHVIPv4Port port)
{
SHVBool retVal(SHVSocket::ErrInvalidOperation);

	if (Type == SHVSocket::TypeUDP)
		retVal = Connect(INADDR_ANY,port);

	return retVal;
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
#ifdef __SHIVASOCKETS_NOSELECTMODE
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
SHVInt subID;
int evCode = SHVSocketServer::EventSockStatus;
bool doEvent = true;
int sockerr;
int sz;
SHVIPv4Addr fromip = InvalidIPv4;
SHVIPv4Port fromport = 0;

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
		subID = State;
		break;
	case SHVSocket::StateConnected: // received something
		bufPtr = new SHVBufferPtr();
		bufPtr->SetBufferSize(BufferSize);

		if (Type == TypeUDP)
		{
		sockaddr_in host;
		
			host.sin_family=AF_INET;
			host.sin_addr.s_addr = 0;
			host.sin_port = 0;
			
#ifdef __SHIVA_WIN32
		int len = sizeof(sockaddr_in);
			sz = ::recvfrom(Socket,(char*)bufPtr->GetBufferAsVoid(),(int)BufferSize,MSG_DONTWAIT, (sockaddr*) &host, &len);
#else
		socklen_t len = sizeof(sockaddr_in);
			sz = ::recvfrom(Socket,bufPtr->GetBufferAsVoid(),BufferSize,MSG_DONTWAIT, (sockaddr*) &host, &len);
#endif
			fromip = host.sin_addr.s_addr;
			fromport = ntohs(host.sin_port);
		}
		else
		{
#ifdef __SHIVA_WIN32
			sz = ::recv(Socket,(char*)bufPtr->GetBufferAsVoid(),(int)BufferSize,MSG_DONTWAIT);
#else
			sz = ::recv(Socket,bufPtr->GetBufferAsVoid(),BufferSize,MSG_DONTWAIT);
#endif
		}
		
		if (sz<=0) // error
		{
			delete bufPtr;
			Close();
			SetError(SHVSocket::ErrGeneric);
		}
		else
		{
			ReceiveBuffers.AddTail(BufferInstance(bufPtr,(size_t)sz,fromip,fromport));
			BytesRead += (size_t)sz;
			evCode = SHVSocketServer::EventSockDataRead;
		}
		subID = State;
		break;
	default:
		Close();
		SetError(SHVSocket::ErrGeneric);
		subID = State;
		break;
	}
	
	SocketServer->SocketServerLock.Unlock();
	
	if (doEvent)
	{
		queue = EventSubscriber->Emit(SocketServer->GetModules(), new SHVEventSocket(SocketServer,evCode,subID,this,retValObj));
	}
	if (queue)
	{
		queue->SignalDispatcher();
	}

#ifdef __SHIVASOCKETS_NOSELECTMODE
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


#ifdef __SHIVASOCKETS_NOSELECTMODE
void SHVSocketImpl::StartReadThread()
{
	ReadThread.Start(this,&SHVSocketImpl::ReadThreadFunc);
}

void SHVSocketImpl::ReadThreadFunc()
{
fd_set rfds;
fd_set wfds;
fd_set efds;
int nextFD, retVal;

	while (GetState() != SHVSocket::StateError && Socket != SHVSocketImpl::InvalidSocket)
	{
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);
		FD_ZERO(&efds);
		FD_SET(Socket, &rfds);
		if (GetState() == SHVSocket::StateConnecting)
		{
			FD_SET(Socket, &wfds);
			FD_SET(Socket, &efds);
		}
		nextFD = int(Socket)+1;

		retVal = select(nextFD, &rfds, &wfds, &efds, NULL);

		if (retVal == -1) // ERROR!
		{
			Close();
			SetError();
			IsPending = true;
		}
		else if (FD_ISSET(Socket,&rfds) || FD_ISSET(Socket,&wfds) || FD_ISSET(Socket,&efds))
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
