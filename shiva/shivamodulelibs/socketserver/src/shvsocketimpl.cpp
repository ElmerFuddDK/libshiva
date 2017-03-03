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
#include "../../../include/utils/shvdir.h"

#include "../include/shvsocketserverimpl.h"

#ifdef __MINGW32__
# ifndef SIO_KEEPALIVE_VALS
struct tcp_keepalive { u_long  onoff; u_long  keepalivetime; u_long  keepaliveinterval; };
#  define SIO_KEEPALIVE_VALS _WSAIOW(IOC_VENDOR,4)
# endif
# define MSG_DONTWAIT 0
# define MSG_NOSIGNAL 0
# include <sys/types.h>
# include <fcntl.h>
# include <errno.h>
# if !defined(__SHIVASOCKETS_IPV6DISABLED)
#  include <ws2tcpip.h>
# endif
#elif defined(__SHIVA_WIN32)
# define MSG_DONTWAIT 0
# define MSG_NOSIGNAL 0
# ifndef __SHIVA_WINCE
#  include <fcntl.h>
#  include <errno.h>
# endif
# include <mstcpip.h>
# if !defined(__SHIVASOCKETS_IPV6DISABLED)
#  include <ws2tcpip.h>
# endif
#else
# include <netinet/in.h>
# include <netinet/tcp.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <sys/ioctl.h>
# include <fcntl.h>
# include <errno.h>
#endif
#ifdef __SHIVA_POSIX
# include <sys/un.h>
#endif

#if defined(__SHIVA_WIN32) && !defined(IPV6_V6ONLY)
# define IPV6_V6ONLY 27
#endif

#define __SHVSOCKET_SENDTIMEOUT 30

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
///\endcond


//=========================================================================================================
// SHVSocketImpl class - Implementation of sockets interface
//=========================================================================================================

///\cond INTERNAL
/*************************************
 * Constructor
 *************************************/
SHVSocketImpl::SHVSocketImpl(SHVEventSubscriberBase* subs, SHVSocketServerImpl* socketServer, SHVSocket::Types type, SHVSOCKTYPE sock)
	: SHVSocket(socketServer->GetModules().CreateTag(),SHVSocket::StateConnected), EventSubscriber(subs), Type(type), SSLState(SHVSocketImpl::SSLStateNone)
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
	: SHVSocket(socketServer->GetModules().CreateTag(),SHVSocket::StateNone), EventSubscriber(subs), Type(type), SSLState(SHVSocketImpl::SSLStateNone)
{
#ifdef __SHIVASOCKETS_IPV6DISABLED
int ipAddrFamily = PF_INET;
#else
int ipAddrFamily = (SHVSocketServerImpl::IPv6SupportedInternal() ? PF_INET6 : PF_INET);
#endif
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

	switch (Type)
	{
	case SHVSocket::TypeTCP:
	case SHVSocket::TypeSSL:
		Socket = ::socket( ipAddrFamily, SOCK_STREAM, 0 );
#ifndef __SHIVASOCKETS_IPV6DISABLED
		if (ipAddrFamily == PF_INET6)
		{ int ipv6only = 0; ::setsockopt( Socket, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&ipv6only, sizeof(ipv6only) ); }
#endif
		break;
	case SHVSocket::TypeUDP:
		Socket = ::socket( ipAddrFamily, SOCK_DGRAM, 0 );
#ifndef __SHIVASOCKETS_IPV6DISABLED
		if (ipAddrFamily == PF_INET6)
		{ int ipv6only = 0; ::setsockopt( Socket, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&ipv6only, sizeof(ipv6only) ); }
#endif
		break;
#ifdef __SHIVA_POSIX
	case SHVSocket::TypeUnix:
		Socket = ::socket( AF_UNIX, SOCK_STREAM, 0 );
		break;
#endif
	default:
		Socket = InvalidSocket;
		break;
	}

	if (Socket != InvalidSocket)
	{
#ifdef __SHIVA_WIN32
		unsigned long nonblock = 1;
		::ioctlsocket(Socket, FIONBIO, &nonblock);
#elif defined __SHIVA_POSIX
		::fcntl(Socket,F_SETFL,O_NONBLOCK);
#endif
	}
}


/*************************************
 * Destructor
 *************************************/
SHVSocketImpl::~SHVSocketImpl()
{
	SSL = NULL;
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

	if (Type == SHVSocket::TypeUnix)
		return SHVBool(SHVSocket::ErrInvalidOperation);

	SocketServer->SocketServerLock.Lock();
	if (State == SHVSocket::StateNone)
	{
	int status;
#ifndef __SHIVASOCKETS_IPV6DISABLED
		if (!SHVSocketServerImpl::IPv6SupportedInternal())
#endif
		{
#ifdef __SHIVA_WIN32
		SOCKADDR_IN sockAddr;
#else
		sockaddr_in sockAddr;
#endif
			
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
		}
#ifndef __SHIVASOCKETS_IPV6DISABLED
		else
		{
		sockaddr_in6 sockAddr;
			
			retVal = SHVSocket::ErrNone;
			
			::memset(&sockAddr, 0, sizeof(sockaddr_in6));
			
			sockAddr.sin6_port   = htons(port);
			sockAddr.sin6_family = AF_INET6;
			status = ::bind(Socket, (sockaddr*) &sockAddr, sizeof(sockaddr_in6));
		}
#endif
		
		if (Type == SHVSocket::TypeTCP || Type == SHVSocket::TypeSSL)
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
 * BindAndListenUnix
 *************************************/
SHVBool SHVSocketImpl::BindAndListenUnix(const SHVStringC fileName)
{
#ifdef __SHIVA_POSIX
SHVBool retVal;

	if (Type != SHVSocket::TypeUnix)
		return SHVBool(SHVSocket::ErrInvalidOperation);

	SocketServer->SocketServerLock.Lock();
	if (State == SHVSocket::StateNone)
	{
	sockaddr_un sockAddr;
	int status;
	int val1, val2;

		// If "reuseaddr" is set then fake the behavior by deleting the old file
		if (GetSocketOption(SHVSocket::SockOptReuseAddr,val1,val2) && val1)
			SHVDir::DeleteFile(fileName);
	
		retVal = SHVSocket::ErrNone;

		memset(&sockAddr, 0, sizeof(sockAddr));
		sockAddr.sun_family = AF_UNIX;
		strncpy(sockAddr.sun_path, fileName.GetSafeBuffer(), sizeof(sockAddr.sun_path)-1);
		status = ::bind(Socket, (struct sockaddr*)&sockAddr, sizeof(sockAddr));

		if (!status)
		{
			status = ::listen(Socket, 5);

			if (!status)
			{
				State = SHVSocket::StateListening;
# ifdef __SHIVASOCKETS_NOSELECTMODE
				StartReadThread();
# endif
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
#else
	return SHVBool(SHVSocket::ErrInvalidOperation);
#endif
}

/*************************************
 * Send
 *************************************/
SHVBool SHVSocketImpl::Send(const SHVBufferC& buf)
{
SHVBool retVal(SHVSocket::ErrInvalidOperation);
bool retry = false;
	
	SocketServer->SocketServerLock.Lock();
	if (State == SHVSocket::StateConnected)
	{
	int err = 0;
	const SHVByte* bufPtr = buf.GetBufferConst();
	int bufLen = (int)buf.GetSize();
	int i = 0;

		while (i<__SHVSOCKET_SENDTIMEOUT*10 && err == 0 && bufLen > 0)
		{
			retry = false;
			if (!SSL.IsNull())
			{
				err = SSL->send(bufPtr,bufLen, retry);
			}
			else
			{
				err = ::send(Socket,bufPtr,bufLen,MSG_NOSIGNAL);
			}
			if (err < 0)
			{
				 i++;
#ifdef __SHIVA_WIN32
				if (SSL.IsNull() || err == SSL_ERROR_SYSCALL)
					retry = WSAGetLastError() == WSAEWOULDBLOCK;
#else
				if (SSL.IsNull() || err == SSL_ERROR_SYSCALL)
					retry = (err = errno) == EAGAIN || err == EWOULDBLOCK;
#endif
				if(retry)
				{
					err = 0;
					SHVThreadBase::Sleep(100);
				}
				else
				{
					err = -1;
				}
			}
			else if (err == 0)
			{
				err = -1;
			}
			else
			{
				i=0;
				SHVASSERT(err <= bufLen);
				bufLen -= err;
				bufPtr += err;
				err = 0;
			}
		}
	
		if (err || i == __SHVSOCKET_SENDTIMEOUT*10)
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

		host.sin_family=AF_INET;
		host.sin_addr.s_addr = ip;
		host.sin_port = htons(port);
		
		retVal = SendToInternal(buf,&host,sizeof(host));
	}
	SocketServer->SocketServerLock.Unlock();
	
	return retVal;
}

/*************************************
 * SendTo6
 *************************************/
SHVBool SHVSocketImpl::SendTo6(const SHVBufferC& buf, SHVIPv6Addr ip, SHVIPv4Port port)
{
SHVBool retVal(SHVSocket::ErrInvalidOperation);

#ifdef __SHIVASOCKETS_IPV6DISABLED
	SHVUNUSED_PARAM(buf);
	SHVUNUSED_PARAM(ip);
	SHVUNUSED_PARAM(port);
#else
	SocketServer->SocketServerLock.Lock();
	if (Type == TypeUDP && (State == SHVSocket::StateConnected || State == SHVSocket::StateNone) && SHVSocketServerImpl::IPv6SupportedInternal())
	{
	sockaddr_in6 host;

		::memset(&host,0,sizeof(host));
		host.sin6_family=AF_INET6;
		host.sin6_addr = *(in6_addr*)&ip;
		host.sin6_port = htons(port);
		
		retVal = SendToInternal(buf,&host,sizeof(host));
	}
	SocketServer->SocketServerLock.Unlock();
#endif
	
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
		fromIP = (ReceiveBuffers.GetFirst().IPv6Mode ? (SHVIPv4Addr)InvalidIPv4 : ReceiveBuffers.GetFirst().FromIP4);
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
SHVBuffer *SHVSocketImpl::PopReceiveBuffer6(size_t &bytesRead, SHVIPv6Addr &fromIP, SHVIPv4Port &fromPort)
{
SHVBuffer* retVal = NULL;

	SocketServer->SocketServerLock.Lock();
	if (ReceiveBuffers.GetCount())
	{
		retVal = ReceiveBuffers.GetFirst().Buffer.ReleaseReference();
		bytesRead = ReceiveBuffers.GetFirst().BytesRead;
		if (ReceiveBuffers.GetFirst().IPv6Mode)
			fromIP = ReceiveBuffers.GetFirst().FromIP6;
		else
#ifdef __SHIVASOCKETS_IPV6DISABLED
			::memset(&fromIP, 0, sizeof(SHVIPv6Addr));
#else
			fromIP = *(SHVIPv6Addr*)&in6addr_any;
#endif
		fromPort = ReceiveBuffers.GetFirst().FromPort;
		
		ReceiveBuffers.RemoveHead();
		BytesRead -= bytesRead;
	}
	else
	{
#ifdef __SHIVASOCKETS_IPV6DISABLED
		::memset(&fromIP, 0, sizeof(SHVIPv6Addr));
#else
		fromIP = *(SHVIPv6Addr*)&in6addr_any;
#endif
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
#elif defined(__SHIVA_POSIX_FREEBSD)
	case SHVSocket::SockOptKeepaliveIdle:
		///\todo Figure out a way to implement SHVSocket::SockOptKeepaliveIdle for FreeBSD
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
#elif defined __SHIVA_POSIX
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
#ifdef __SHIVA_POSIX
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
#elif defined(__SHIVA_POSIX_FREEBSD)
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
 * SetServerCertificate
 *************************************/
SHVBool SHVSocketImpl::SetServerCertificate(const SHVStringC keyfile, const SHVStringC certFile)
{
SHVBool retVal(true);
	if (Type == SHVSocket::TypeSSL)
	{
		if (SHVDir::FileExist(keyfile))
		{
			KeyFile = keyfile.ToStr8();
		}
		else
		{
			retVal = false;
		}
		if (SHVDir::FileExist(certFile))
		{
			CertFile = certFile.ToStr8();
		}
		else
		{
			retVal = false;
		}
	}
	else
	{
		retVal = false;
	}
	return retVal;
}

/*************************************
 * StartTLS
 *************************************/
///\ Converts a connected TCP socket to SSL
SHVBool SHVSocketImpl::StartTLS()
{
SHVBool retVal(SHVSocket::ErrInvalidOperation);
bool sendEvent = false;
	SocketServer->SocketServerLock.Lock();
	if (Type == SHVSocket::TypeTCP || State == SHVSocket::StateConnected)
	{
		Type = SHVSocket::TypeSSL;
		State = SHVSocket::StateConnecting;
		retVal.SetError(SSLConnect() ? SHVBool::True : Error.GetError());
		if (retVal && State == SHVSocket::StateConnected)
			sendEvent = true;
	}
	SocketServer->SocketServerLock.Unlock();

	if (sendEvent)
		EventSubscriber->EmitNow(SocketServer->GetModules(), new SHVEventSocket(SocketServer,SHVSocketServer::EventSockStatus,SHVSocket::StateConnected,this,NULL));

	return retVal;
}


/*************************************
 * Close
 *************************************/
SHVBool SHVSocketImpl::Close()
{
	SocketServer->SocketServerLock.Lock();
	SSL = NULL;
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
 * Shutdown
 *************************************/
SHVBool SHVSocketImpl::Shutdown()
{
	SocketServer->SocketServerLock.Lock();
	if (Socket != InvalidSocket)
	{
		if (State == StateConnected)
			::shutdown(Socket, 2); // close both recv and send
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
	{
#ifndef __SHIVASOCKETS_IPV6DISABLED
		if (SHVSocketServerImpl::IPv6SupportedInternal())
			retVal = Connect6(*(SHVIPv6Addr*)&in6addr_any,port);
		else
#endif
			retVal = Connect(INADDR_ANY,port);
	}

	return retVal;
}

/*************************************
 * Connect
 *************************************/
SHVBool SHVSocketImpl::Connect(SHVIPv4Addr ip, SHVIPv4Port port)
{
SHVBool retVal(SHVSocket::ErrInvalidOperation);

	SocketServer->SocketServerLock.Lock();
	if (State == SHVSocket::StateNone && Type != SHVSocket::TypeUnix)
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
SHVBool SHVSocketImpl::Connect6(SHVIPv6Addr ip, SHVIPv4Port port)
{
SHVBool retVal(SHVSocket::ErrInvalidOperation);

#ifdef __SHIVASOCKETS_IPV6DISABLED
	SHVUNUSED_PARAM(ip);
	SHVUNUSED_PARAM(port);
#else
	SocketServer->SocketServerLock.Lock();
	if (State == SHVSocket::StateNone && Type != SHVSocket::TypeUnix && SHVSocketServerImpl::IPv6SupportedInternal())
	{
	sockaddr_in6 host;
	int result;

		::memset(&host,0,sizeof(host));
		host.sin6_family=AF_INET6;
		host.sin6_addr = *(in6_addr*)&ip;
		host.sin6_port = htons(port);
		
		result = ::connect(Socket, (sockaddr*)&host, sizeof(sockaddr_in6));
# ifdef __SHIVA_WIN32
		if (result == -1 && WSAGetLastError() != WSAEWOULDBLOCK) // failure
# else
		if (result == -1 && errno != EINPROGRESS) // failure
# endif
		{
# ifndef __SHIVA_WIN32
			printf("SOCKET CONNECT Err: %d\n", errno);
# endif
			Close();
			retVal = SetError(SHVSocket::ErrGeneric);
		}
		else
		{
			State = SHVSocket::StateConnecting;
			retVal.SetError(SHVSocket::ErrNone);
# ifdef __SHIVASOCKETS_NOSELECTMODE
			StartReadThread();
# endif
		}
	}
	SocketServer->SocketServerLock.Unlock();
	
	if (retVal == SHVBool::True)
	{
		SocketServer->SocketServerThread.SignalDispatcher();
	}
#endif
	return retVal;
}
SHVBool SHVSocketImpl::Connect(const SHVStringC ipAddr, SHVIPv4Port port)
{
#ifdef __SHIVASOCKETS_IPV6DISABLED
	return Connect(SocketServer->Inetv4Addr(ipAddr),port);
#else
	return (SHVSocketServerImpl::IPv6SupportedInternal() ? Connect6(SocketServer->Inetv6Addr(ipAddr),port) : Connect(SocketServer->Inetv4Addr(ipAddr),port));
#endif
}

/*************************************
 * ConnectUnix
 *************************************/
SHVBool SHVSocketImpl::ConnectUnix(const SHVStringC fileName)
{
SHVBool retVal(SHVSocket::ErrInvalidOperation);

#ifdef __SHIVA_POSIX
	SocketServer->SocketServerLock.Lock();
	if (State == SHVSocket::StateNone && Type == SHVSocket::TypeUnix)
	{
	sockaddr_un host;
	int result;

		memset(&host, 0, sizeof(host));
		host.sun_family = AF_UNIX;
		strncpy(host.sun_path, fileName.GetSafeBuffer(), sizeof(host.sun_path)-1);
		
		result = ::connect(Socket, (sockaddr*)&host, sizeof(sockaddr_un));
		if (result == -1 && errno != EINPROGRESS) // failure
		{
			Close();
			retVal = SetError(SHVSocket::ErrGeneric);
		}
		else
		{
			State = SHVSocket::StateConnecting;
			retVal.SetError(SHVSocket::ErrNone);
# ifdef __SHIVASOCKETS_NOSELECTMODE
			StartReadThread();
# endif
		}
	}
	SocketServer->SocketServerLock.Unlock();
	
	if (retVal == SHVBool::True)
	{
		SocketServer->SocketServerThread.SignalDispatcher();
	}
#endif
	
	return retVal;
}



///\cond INTERNAL
/*************************************
 * SendToInternal
 *************************************/
SHVBool SHVSocketImpl::SendToInternal(const SHVBufferC& buf, void* host, size_t hostLen)
{
SHVBool retVal;
int err = 0;

	for (int i=0; i<__SHVSOCKET_SENDTIMEOUT*10 && err == 0; i++)
	{
		err = ::sendto(Socket,buf.GetBufferConst(),(int)buf.GetSize(),MSG_NOSIGNAL,(sockaddr*)host,hostLen);
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
	
	return retVal;
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
SHVIPv4Addr fromip4 = InvalidIPv4;
SHVIPv4Port fromport = 0;
#ifndef __SHIVASOCKETS_IPV6DISABLED
SHVIPv6Addr fromip6 = *(SHVIPv6Addr*)&in6addr_any;
bool ipv6Mode = SHVSocketServerImpl::IPv6SupportedInternal();
#endif
int state = (SSLState != SHVSocketImpl::SSLStateNone ? (int) SSLState : (int) State);
bool retry;

	// After we got this lock DO NOT PERFORM ANYTHING BLOCKING OR SOMETHING THAT INVOLVES OTHER LOCKS!
	SocketServer->SocketServerLock.Lock();
	
	// Handle the event, collecting data for the emission after unlocking
	switch  (state)
	{
	case SHVSocket::StateListening:
		sockObj = new SHVSocketImpl(EventSubscriber,SocketServer,Type,::accept(Socket, NULL, NULL));
		if (Type == SHVSocket::TypeSSL)
		{
			if (!sockObj->SSLAccept(KeyFile.GetBufferConst(), CertFile.GetBufferConst()))
			{
				if (sockObj->SSLState == SHVSocketImpl::SSLStateAccepting)
					doEvent = false;
				else
					sockObj->Socket = InvalidSocket;
			}
		}
		sockObj->SetReceiveBufferSize(BufferSize);
		if (sockObj->Socket < 0 && sockObj->GetError() == SHVSocket::ErrNone)
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
			if (Type == SHVSocket::TypeSSL)
			{
				SSLConnect();
				if (SSLState == SHVSocketImpl::SSLStateConnecting)
					doEvent = false;
				else if (State == SHVSocket::StateConnected)
					subID = State;
			}
			else
			{
				State = SHVSocket::StateConnected;
				subID = State;
			}
		}
		break;
	case SHVSocket::StateConnected: // received something
		bufPtr = new SHVBufferPtr();
		bufPtr->SetBufferSize(BufferSize);
		retry = false;

		if (Type == TypeUDP)
		{
#ifndef __SHIVASOCKETS_IPV6DISABLED
			if (!ipv6Mode)
#endif
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
				fromip4 = host.sin_addr.s_addr;
				fromport = ntohs(host.sin_port);
			}
#ifndef __SHIVASOCKETS_IPV6DISABLED
			else
			{
			sockaddr_in6 host;
			
				::memset(&host,0,sizeof(host));
				host.sin6_family=AF_INET6;
				host.sin6_addr = in6addr_any;
				host.sin6_port = 0;
				
#ifdef __SHIVA_WIN32
			int len = sizeof(sockaddr_in6);
				sz = ::recvfrom(Socket,(char*)bufPtr->GetBufferAsVoid(),(int)BufferSize,MSG_DONTWAIT, (sockaddr*) &host, &len);
#else
			socklen_t len = sizeof(sockaddr_in6);
				sz = ::recvfrom(Socket,bufPtr->GetBufferAsVoid(),BufferSize,MSG_DONTWAIT, (sockaddr*) &host, &len);
#endif
				fromip6 = *(SHVIPv6Addr*)&host.sin6_addr;
				fromport = ntohs(host.sin6_port);
			}
#endif
		}
		else
		{
			if (!SSL.IsNull())
			{
				sz = SSL->recv((char*)bufPtr->GetBufferAsVoid(),(int)BufferSize, retry);
			}
			else
			{
#ifdef __SHIVA_WIN32
				sz = ::recv(Socket,(char*)bufPtr->GetBufferAsVoid(),(int)BufferSize,MSG_DONTWAIT);
#else
				sz = ::recv(Socket,bufPtr->GetBufferAsVoid(),BufferSize,MSG_DONTWAIT);
#endif
			}
		}
		
		if (!retry)
		{
			if (sz<=0) // error
			{
				delete bufPtr;
				Close();
				SetError(SHVSocket::ErrGeneric);
			}
			else
			{
#ifdef __SHIVASOCKETS_IPV6DISABLED
				ReceiveBuffers.AddTail(BufferInstance(bufPtr,(size_t)sz,fromip4,fromport));
#else
				ReceiveBuffers.AddTail(ipv6Mode ? BufferInstance(bufPtr,(size_t)sz,fromip6,fromport) : BufferInstance(bufPtr,(size_t)sz,fromip4,fromport));
#endif
				BytesRead += (size_t)sz;
				evCode = SHVSocketServer::EventSockDataRead;
			}
			subID = State;
		}
		else
			doEvent = false;
		break;
	case SHVSocketImpl::SSLStateAccepting:
		SSLState = SHVSocketImpl::SSLStateNone;
		if (!SSLAccept(KeyFile.GetBufferConst(), CertFile.GetBufferConst()))
		{
			if (SSLState == SHVSocketImpl::SSLStateAccepting)
				doEvent = false;
			else
				Socket = InvalidSocket;
		}
		else
		{
			retValObj = this;
			evCode = SHVSocketServer::EventSockIncomingConn;
		}
		break;
	case SHVSocketImpl::SSLStateConnecting:
		if (SSL.IsNull())
		{
			SetError(SHVSocket::ErrSSLConnection);
		}
		else if (!SSL->Connect(this, retry))
		{
			if (retry)
			{
				SSLState = SHVSocketImpl::SSLStateConnecting;
				doEvent = false;
			}
			else
			{
				SetError(SHVSocket::ErrSSLConnection);
			}
		}
		else
		{
			SSLState = SHVSocketImpl::SSLStateNone;
			State = SHVSocket::StateConnected;
			subID = State;
		}
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

/*************************************
 * SSLAccept
 *************************************/
SHVBool SHVSocketImpl::SSLAccept(const char *keyFile, const char *certFile)
{
SHVBool retVal(true);
bool retry;

	if (Socket != InvalidSocket)
	{
		if (!SSL)
			SSL = SocketServer->SSLFactory->CreateSSLSocket();
		retVal = SSL->Accept(this, keyFile, certFile, retry);
		if (!retVal)
		{
			if (retry)
			{
				SSLState = SHVSocketImpl::SSLStateAccepting;
			}
			else
			{
				retVal = SetError(SHVSocket::ErrListeningSSL);
			}
		}
		else
		{
			State = SHVSocket::StateConnected;
			SSLState = SHVSocketImpl::SSLStateNone;
		}
	}
	else
		retVal = SetError(SHVSocket::ErrListening);

	return retVal;
}

/*************************************
 * SSLConnect
 *************************************/
bool SHVSocketImpl::SSLConnect()
{
bool retry = false;
bool retVal = true;
	SSL = SocketServer->SSLFactory->CreateSSLSocket();
	if (!SSL->Connect(this, retry))
	{					
		if (retry)
		{
			SSLState = SHVSocketImpl::SSLStateConnecting;
		}
		else
		{
			SetError(SHVSocket::ErrSSLConnection);
			retVal = false;
		}
	}
	else
	{
		State = SHVSocket::StateConnected;
	}
	return retVal;
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
SHVSocketImplRef tmpRefToSelf;

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
			tmpRefToSelf = this;
			Close();
			SetError();
			IsPending = true;
			SocketServer->SocketServerThread.SignalDispatcher(); // signal the socket server thread
		}
		else if (FD_ISSET(Socket,&rfds) || FD_ISSET(Socket,&wfds) || FD_ISSET(Socket,&efds))
		{
			ReadThreadSignal.Lock(); // request signal
			IsPending = true;
			SocketServer->SocketServerThread.SignalDispatcher(); // signal the socket server thread

			ReadThreadSignal.Lock();
			ReadThreadSignal.Unlock();
		}
	}
}
#endif
///\endcond
