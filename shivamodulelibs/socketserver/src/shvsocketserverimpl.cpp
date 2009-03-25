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
# include <arpa/inet.h>
# include <netdb.h>
#endif



//=========================================================================================================
// SHVSocketServerImpl class - Implementation of sockets interface
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVSocketServerImpl::SHVSocketServerImpl(SHVModuleList& modules) : SHVSocketServer(modules), SocketServerThread(this)
{
#ifdef __SHIVA_WIN32
WSADATA wsaData;
	WSAInitialized =(WSAStartup(MAKEWORD( 2, 2 ), &wsaData) == 0);
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
		Modules.AddStartupError(_T("Failed initializing socket system"));
	}
#endif
	if (!SocketServerThread.StartThread(Modules))
		return Modules.AddStartupError(_T("Failed starting socket server thread"));

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
SHVSocketImplRef sock = new SHVSocketImpl(subs,this,type);

	sock->SetReceiveBufferSize(bufferSize);
	AddToList(sock);

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
#ifdef UNICODE
SHVString8 tmpBuf = ::inet_ntoa(addr);
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
		retVal = ((struct in_addr *)*h->h_addr_list)->s_addr;
	}
	
	return retVal;
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
///\endcond
