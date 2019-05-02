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
#ifndef __SHIVA_WIN32
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>
#endif

#if defined(__SHIVA_WIN32) && !defined(__SHIVASOCKETS_IPV6DISABLED)
# include <ws2tcpip.h>
# if !defined(IPV6_V6ONLY)
#  define IPV6_V6ONLY 27
# endif
#endif



//=========================================================================================================
// SHVSocketServerImpl class - Implementation of sockets interface
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVSocketServerImpl::SHVSocketServerImpl(SHVModuleList& modules) : SHVSocketServer(modules), SocketServerThread(this)
{
#if defined(__SHIVASOCKETS_IPV6DISABLED)
	Ipv6Enabled = false;
#elif defined(__SHIVA_WIN32)
WSADATA wsaData;
int osVerMajor = (DWORD)(LOBYTE(LOWORD(GetVersion())));
	WSAInitialized =(WSAStartup(MAKEWORD( 2, 2 ), &wsaData) == 0);
	Ipv6Enabled = osVerMajor > 5; // Newer than xp/2k3
#else
	Ipv6Enabled = true;
#endif
#ifndef __SHIVASOCKETS_IPV6DISABLED
	if (Ipv6Enabled)
	{
	SHVSOCKTYPE testSocket = ::socket( PF_INET6, SOCK_STREAM, 0 );
	int ipv6only = 0;

		// If it fails to enable ipv4 mixed mode then the OS doesn't support ipv6 after all
		Ipv6Enabled = (setsockopt( testSocket, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&ipv6only, sizeof(ipv6only) ) ? false : true);
# ifdef __SHIVA_WIN32
		::closesocket(testSocket);
# else
		::close(testSocket);
# endif
	}
#endif
}

/*************************************
 * Destructor
 *************************************/
SHVSocketServerImpl::~SHVSocketServerImpl()
{
#ifdef __SHIVA_WIN32
	if (WSAInitialized)
		WSACleanup();
#endif
}

/*************************************
 * Register
 *************************************/
SHVBool SHVSocketServerImpl::Register()
{
#ifdef __SHIVA_WIN32
	if (!WSAInitialized)
	{
		Modules.AddStartupError(_S("Failed initializing socket system"));
	}
#endif
	if (!SocketServerThread.StartThread(Modules))
		return Modules.AddStartupError(_S("Failed starting socket server thread"));
	SSLFactory = new SHVSSLSocketFactory();

	return SHVSocketServer::Register();
}

/*************************************
 * PostRegister
 *************************************/
void SHVSocketServerImpl::PostRegister()
{
}

/*************************************
 * Unregister
 *************************************/
void SHVSocketServerImpl::Unregister()
{
SHVList<SHVSocketImplRef,SHVSocketImpl*> tmpSocketList;
SHVListIterator<SHVSocketImplRef,SHVSocketImpl*> sockItr(SocketList);
	SocketServerThread.StopThread();
	
	// clear the list, since it will contain allocations from other modules
	SocketServerLock.Lock();
	while (sockItr.MoveNext())
		tmpSocketList.AddTail(sockItr.Get());
	SocketList.RemoveAll();
	SocketServerLock.Unlock();

	// this will delete the objects, handy when in noselect mode (prevents deadlocks)
	tmpSocketList.RemoveAll();	
	SSLFactory = NULL;
}

/*************************************
 * GetInternalEventThread
 *************************************/
SHVEventQueue* SHVSocketServerImpl::GetQueue()
{
	return (SHVEventQueue*)&SocketServerThread;
}

/*************************************
 * CreateSocket
 *************************************/
SHVSocket* SHVSocketServerImpl::CreateSocket(SHVEventSubscriberBase* subs, SHVSocket::Types type, size_t bufferSize)
{
SHVSocketImplRef sock = NULL;

	if (SocketTypeSupported(type))
	{
		sock = new SHVSocketImpl(subs,this,type);
		sock->SetReceiveBufferSize(bufferSize);
		AddToList(sock);
	}

	return sock;
}

/*************************************
 * GetSocket
 *************************************/
SHVSocket* SHVSocketServerImpl::GetSocket(SHVInt tag)
{
SHVListIterator<SHVSocketImplRef,SHVSocketImpl*> SocketListItr(SocketList);
SHVSocket* retVal = NULL;

	SocketServerLock.Lock();
	while(!retVal && SocketListItr.MoveNext())
	{
		if ( ((SHVSocket*)SocketListItr.Get())->GetTag() == tag )
		{
			retVal = (SHVSocket*)SocketListItr.Get();
		}
	}
	SocketServerLock.Unlock();
	
	return retVal;
}

/*************************************
 * Inetv4Addr
 *************************************/
SHVIPv4Addr SHVSocketServerImpl::Inetv4Addr(const SHVStringC strIp)
{
SHVIPv4Addr retVal = (SHVIPv4Addr)::inet_addr(strIp.ToStr8().GetSafeBuffer());

	if (retVal == INADDR_NONE)
		retVal = SHVSocket::InvalidIPv4;

	return retVal;
}

/*************************************
 * Inetv4ToAddr
 *************************************/
SHVStringBuffer SHVSocketServerImpl::Inetv4ToAddr(SHVIPv4Addr ip)
{
SHVString retVal;
#ifdef __SHIVA_WIN32
in_addr addr;
	addr.S_un.S_addr = ip;
#else
in_addr addr = { ip };
#endif
#if __SHVSTRINGDEFAULT == 16
SHVString8 tmpBuf = SHVString8C(::inet_ntoa(addr));
	retVal = tmpBuf.ToStrT();
#else
	retVal = ::inet_ntoa(addr);
#endif
	return retVal.ReleaseBuffer();
}

/*************************************
 * Inetv4ResolveHost
 *************************************/
SHVIPv4Addr SHVSocketServerImpl::Inetv4ResolveHost(const SHVStringC host)
{
SHVIPv4Addr retVal = Inetv4Addr(host);

	if (retVal == SHVSocket::InvalidIPv4)
	{
	hostent* h = gethostbyname(host.ToStr8().GetSafeBuffer());
		retVal = (h ? ((struct in_addr *)*h->h_addr_list)->s_addr : (SHVIPv4Addr)SHVSocket::InvalidIPv4);
	}
	
	return retVal;
}


/*************************************
 * Inetv6Addr
 *************************************/
SHVIPv6Addr SHVSocketServerImpl::Inetv6Addr(const SHVStringC strIp)
{
SHVIPv6Addr retVal;
#ifdef __SHIVASOCKETS_IPV6DISABLED
	::memset(&retVal,0,sizeof(retVal));
	SHVUNUSED_PARAM(strIp);
#else
SHVString8 strIp8;
	if (strIp.Find(_S(":")) < 0)
		strIp8.Format("::FFFF:%s", strIp.ToStr8().GetSafeBuffer());
	else
		strIp8 = strIp.ToStr8();
# ifdef __SHIVA_WIN32
sockaddr_in6 host;
INT sockAddrLen = sizeof(host);
	if (WSAStringToAddressA((LPSTR)strIp8.GetSafeBuffer(),AF_INET6,NULL,(LPSOCKADDR)&host,&sockAddrLen) == 0)
	{
		retVal = *(SHVIPv6Addr*)&host.sin6_addr;
	}
	else
# else
	if (inet_pton(AF_INET6,strIp8.GetSafeBuffer(),&retVal) <= 0)
# endif
	{
		::memset(&retVal,0,sizeof(retVal));
	}
#endif
	return retVal;
}

/*************************************
 * Inetv6ToAddr
 *************************************/
SHVStringBuffer SHVSocketServerImpl::Inetv6ToAddr(SHVIPv6Addr ip)
{
SHVString retVal;
#ifdef __SHIVASOCKETS_IPV6DISABLED
	SHVUNUSED_PARAM(ip);
#else
char strIp8[128];
char* strIp8Ptr = NULL;
# ifdef __SHIVA_WIN32
sockaddr_in6 sockAddr;
int sockAddrLen = sizeof(sockAddr);
DWORD strIp8Len = 127;

	::memset(&sockAddr, 0, sizeof(sockaddr_in6));

	sockAddr.sin6_family = AF_INET6;
	sockAddr.sin6_addr = *(in6_addr*)&ip;
	
	if (WSAAddressToStringA((LPSOCKADDR)&sockAddr, sockAddrLen, NULL, strIp8, &strIp8Len) == 0)
	{
		strIp8Ptr = strIp8;
		if (*strIp8 == '[' && strIp8Len > 1)
		{
			strIp8Ptr++;
			strIp8[strIp8Len-1] = '\0';
		}
	}
# else
	if (inet_ntop(AF_INET6,&ip,strIp8,128))
	{
		strIp8Ptr = strIp8;
	}
# endif
	
	// Strip the ::FFFF: part if the IP was really an ipv4 address
	if (strIp8Ptr)
	{
		if (strIp8Ptr[0] == ':' && strIp8Ptr[1] == ':'
		        && (strIp8Ptr[2] == 'f' || strIp8Ptr[2] == 'F')
		        && strIp8Ptr[3] == strIp8Ptr[2]
		        && strIp8Ptr[4] == strIp8Ptr[2]
		        && strIp8Ptr[5] == strIp8Ptr[2]
		        && strIp8Ptr[6] == ':')
		{
			strIp8Ptr += 7;
		}

		retVal = SHVString8C(strIp8Ptr).ToStrT();
	}
#endif
	return retVal.ReleaseBuffer();
}

/*************************************
 * Inetv6ResolveHost
 *************************************/
SHVIPv6Addr SHVSocketServerImpl::Inetv6ResolveHost(const SHVStringC host)
{
SHVIPv6Addr retVal = Inetv6Addr(host);

#ifndef __SHIVASOCKETS_IPV6DISABLED
	if (::memcmp(&retVal,&in6addr_any,sizeof(retVal)) == 0)
	{
	struct addrinfo hints;
	struct addrinfo *result = NULL;
	SHVString8 host8(host.ToStr8());
	
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		
		if (getaddrinfo(host8.GetSafeBuffer(), NULL, &hints, &result) == 0 && result)
		{
			if (result->ai_family == AF_INET6)
			{
				retVal = *(SHVIPv6Addr*)&((sockaddr_in6*)result->ai_addr)->sin6_addr;
			}
			else if (result->ai_family == AF_INET)
			{
				retVal = IPv4ToIPv6(((sockaddr_in*)result->ai_addr)->sin_addr.s_addr);
			}
			else
			{
				SHVASSERT(false);
			}
			freeaddrinfo(result);
		}
	}
#endif
	return retVal;
}

/*************************************
 * SocketTypeSupported
 *************************************/
bool SHVSocketServerImpl::SocketTypeSupported(SHVSocket::Types type)
{
	switch (type)
	{
	case SHVSocket::TypeSSL:
		return SSLFactory->IsSupported();
	case SHVSocket::TypeUnix:
#ifdef __SHIVA_POSIX
		return true;
#else
		return false;
#endif
	default:
		return true;
	}
}

/*************************************
 * IPv6Supported
 *************************************/
bool SHVSocketServerImpl::IPv6Supported()
{
	return Ipv6Enabled;
}


///\cond INTERNAL
/*************************************
 * AddToList
 *************************************/
void SHVSocketServerImpl::AddToList(SHVSocketImpl* sock)
{
	if (sock->GetState() != SHVSocket::StateError)
	{
		SocketServerLock.Lock();
		SocketList.AddTail(sock);
		SocketServerLock.Unlock();
	}
}

/*************************************
 * RemoveFromList
 *************************************/
void SHVSocketServerImpl::RemoveFromList(SHVSocketImpl* sock)
{
SHVListIterator<SHVSocketImplRef,SHVSocketImpl*> SocketListItr(SocketList);
bool running = true;

	SocketServerLock.Lock();
	while(running && SocketListItr.MoveNext())
	{
		if ((SHVSocketImpl*)SocketListItr.Get() == sock)
		{
			running = false;
			SocketList.RemoveAt(SocketListItr.Pos());
			SocketListItr.Pos() = NULL; // reset the invalid position for safety - this is not necessary due to the running var and conditional branching and whatnot
		}
	}
	SocketServerLock.Unlock();
}

/*************************************
 * IPv4ToIPv6
 *************************************/
SHVIPv6Addr SHVSocketServerImpl::IPv4ToIPv6(SHVIPv4Addr ipAddr)
{
SHVIPv6Addr retVal;
const unsigned char ipv4header[] = { 0x00, 0x00, 0x00, 0x00, 0x00,
									 0x00, 0x00, 0x00, 0x00, 0x00,
									 0xFF, 0xFF };
char* ipv4offset = ((char*)&retVal)+12;

	::memcpy(&retVal,ipv4header,12);
	::memcpy(ipv4offset,&ipAddr,4);

	return retVal;
}

///\endcond
