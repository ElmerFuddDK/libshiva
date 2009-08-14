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
#include "../../../../include/platformspc.h"
#include "../../../../include/framework/shvmodule.h"
#include "../../../../include/framework/shvmodulefactory.h"
#include "../../../../include/frameworkimpl/shvmainthreadeventdispatchergeneric.h"
#include "../../../../include/shvversion.h"
#include "../../../../include/utils/shvdll.h"
#include "../../../../include/modules/socketserver/shvsocketserver.h"

#include "../../../../include/frameworkimpl/shvmainthreadeventdispatcherconsole.h"


class SHVTest : public SHVModule
{
	SHVSocketServer* SocketServer;
	SHVSocketRef Socket;
	SHVList<SHVSocketRef,SHVSocket*> ClientSockets;
	SHVEventSubscriberBaseRef SocketSubscriber;
	SHVIPv4Addr UdpIP;
	SHVIPv4Port UdpPort;
public:

	SHVTest(SHVModuleList& modules) : SHVModule(modules,"Test")
	{
	}
	
	SHVBool Register()
	{
		printf("In register\n");
		
		if (!SHVModuleResolver<SHVSocketServer>(Modules,SocketServer,"SocketServer"))
			return false;
		
		Modules.EventSubscribe(SHVModuleList::EventEndRegister, new SHVEventSubscriber(this));
		Modules.EventSubscribe(SHVModuleList::EventClosing, new SHVEventSubscriber(this));
		Modules.EventSubscribe(__EVENT_GLOBAL_STDIN, new SHVEventSubscriber(this));
		
		UdpIP = 0;
		UdpPort = 0;

		return SHVModule::Register();
	}
	
	void PostRegister()
	{
	}
	
	void OnSocketEvent(SHVEvent* event)
	{
		if (SHVEvent::Equals(event,SHVSocketServer::EventSockStatus))
		{
			switch (SHVSocketServer::SocketFromEvent(event)->GetState())
			{
			default:
			case SHVSocket::StateError:
				if (SHVSocketServer::SocketFromEvent(event) == Socket)
				{
					printf("Main socket Error\n");
					ClientSockets.RemoveAll();
					Socket = NULL;
				}
				else
				{
				SHVListPos pos = ClientSockets.Find(SHVSocketServer::SocketFromEvent(event));
				
					printf("SocketError\n");
					
					if (pos)
					{
						printf("Removing client socket\n");
						ClientSockets.RemoveAt(pos);
					}
				}
				break;
			case SHVSocket::StateDisconnected:
				if (SHVSocketServer::SocketFromEvent(event) == Socket)
				{
					printf("Main socket disconnected\n");
					ClientSockets.RemoveAll();
					Socket = NULL;
				}
				else
				{
				SHVListPos pos = ClientSockets.Find(SHVSocketServer::SocketFromEvent(event));
				
					printf("Socket disconnected\n");
					
					if (pos)
					{
						printf("Removing client socket\n");
						ClientSockets.RemoveAt(pos);
					}
				}
				break;
			case SHVSocket::StateConnecting:
				printf("Connecting ...\n");
				break;
			case SHVSocket::StateConnected:
				printf("Connected\n");
				SHVVERIFY(SHVSocketServer::SocketFromEvent(event)->SetSocketOption(SHVSocket::SockOptKeepalive,10));
				SHVVERIFY(SHVSocketServer::SocketFromEvent(event)->SetSocketOption(SHVSocket::SockOptLinger,1,1));
				break;
			case SHVSocket::StateListening:
				printf("Listening\n");
				break;
			}
		}
		else if (SHVEvent::Equals(event,SHVSocketServer::EventSockDataRead))
		{
		SHVBufferRef buffer;
		size_t bytesRead;
		SHVIPv4Addr fromIP;
		SHVIPv4Port fromPort;
		SHVSocket* socket = SHVSocketServer::SocketFromEvent(event);
			for (buffer = socket->PopReceiveBuffer(bytesRead,fromIP,fromPort); !buffer.IsNull(); buffer = socket->PopReceiveBuffer(bytesRead,fromIP,fromPort))
			{
			SHVString8 str;
				str.AddChars(buffer->GetBufferConst(),bytesRead);
				if (fromIP)
					printf("Received from (%s,%d): %s\n", SocketServer->Inetv4ToAddr(fromIP).GetSafeBuffer(), fromPort, str.GetSafeBuffer());
				else
					printf("Received : %s\n", str.GetSafeBuffer());
			}
		}
		else if (SHVEvent::Equals(event,SHVSocketServer::EventSockIncomingConn))
		{
			printf("Incomming connection ...\n");
			ClientSockets.AddTail(SHVSocketServer::NewSocketFromEvent(event));
		}
	}
	
	void OnEvent(SHVEvent* event)
	{
		if (SHVEvent::Equals(event,SHVModuleList::EventEndRegister))
		{
			SocketSubscriber = new SHVEventSubscriberFunc<SHVTest>(this,&SHVTest::OnSocketEvent,&Modules);
		}
		else if (SHVEventString::Equals(event,__EVENT_GLOBAL_STDIN))
		{
		SHVString8 str(SHVEventStdin::StdinFromEvent(event));
		
			if (str == SHVString8C("/quit"))
			{
				Modules.CloseApp();
			}
			else if (str == SHVString8C("/close"))
			{
				if (!Socket.IsNull())
				{
					printf("Closing socket\n");
					Socket->Close();
					Socket = NULL;
				}
			}
			else if (str.GetSafeBuffer()[0] == '/')
			{
				if (str.Left(8) == SHVString8C("/server "))
				{
				SHVChar* c;
				SHVIPv4Port port = (SHVIPv4Port)SHVString8C::StrToL(str.GetSafeBuffer() + 8, &c);
				
					if (!Socket.IsNull())
					{
						Socket->Close();
						Socket = NULL;
					}
					
					Socket = SocketServer->CreateSocket(SocketSubscriber);
					printf("Attempting to start server at %d\n", port);
					SHVASSERT(Socket->BindAndListen(port ? port : 1234));
				}
				else if (str.Left(9) == SHVString8C("/connect "))
				{
				SHVString8 ip(str.GetSafeBuffer()+9);
				SHVIPv4Port port = 0;
				int space;
				
					if ( (space = ip.ReverseFind(" ")) > 0)
					{
					SHVChar* c;
						port = (SHVIPv4Port)SHVString8C::StrToL(ip.GetSafeBuffer() + space, &c);
						ip[space] = 0;
					}
					
					if (!Socket.IsNull() && Socket->GetType() == SHVSocket::TypeUDP)
					{
						UdpIP = SocketServer->Inetv4ResolveHost(ip.ToStrT());
						UdpPort = port;
						
						if (Socket->GetState() == SHVSocket::StateNone) // not connected
							Socket->ConnectAny(0);
					}
					else
					{
						if (!Socket.IsNull())
						{
							Socket->Close();
							Socket = NULL;
						}
						
						Socket = SocketServer->CreateSocket(SocketSubscriber);
						printf("Attempting connect to \"%s\" port %d\n", ip.GetSafeBuffer(), port);
						SHVASSERT(Socket->Connect(SocketServer->Inetv4ResolveHost(ip.ToStrT()),port ? port : 1234));
					}
				}
				else if (str == SHVString8C("/udp"))
				{
					UdpIP = 0;
					UdpPort = 0;
					
					if (!Socket.IsNull())
					{
						Socket->Close();
						Socket = NULL;
					}
					
					Socket = SocketServer->CreateSocket(SocketSubscriber,SHVSocket::TypeUDP);
					printf("Created a UDP client socket\n");
				}
				else if (str.Left(5) == SHVString8C("/udp "))
				{
				SHVChar* c;
				SHVIPv4Port port = (SHVIPv4Port)SHVString8C::StrToL(str.GetSafeBuffer() + 5, &c);
				
					UdpIP = 0;
					UdpPort = 0;
					
					if (!Socket.IsNull())
					{
						Socket->Close();
						Socket = NULL;
					}
					
					Socket = SocketServer->CreateSocket(SocketSubscriber,SHVSocket::TypeUDP);
					printf("Attempting to start UDP at %d\n", port);
					SHVASSERT(Socket->BindAndListen(port ? port : 1234));
				}
				else if (str == SHVString8C("/disconnect"))
				{
					if (!Socket.IsNull())
					{
						printf("Disconnecting ...\n");
						Socket->Close();
						Socket = NULL;
					}
				}
				else if (str.Left(12) == SHVString8C("/udpconnect "))
				{
				SHVString8 ip(str.GetSafeBuffer()+12);
				SHVIPv4Port port = 0;
				int space;
				
					if ( (space = ip.ReverseFind(" ")) > 0)
					{
						SHVChar* c;
						port = (SHVIPv4Port)SHVString8C::StrToL(ip.GetSafeBuffer() + space, &c);
						ip[space] = 0;
					}
					
					if (!Socket.IsNull())
					{
						Socket->Close();
						Socket = NULL;
					}
					
					Socket = SocketServer->CreateSocket(SocketSubscriber,SHVSocket::TypeUDP);
					printf("Attempting connect to \"%s\" port %d\n", ip.GetSafeBuffer(), port);
					SHVASSERT(Socket->Connect(SocketServer->Inetv4ResolveHost(ip.ToStrT()),port ? port : 1234));
				}
				else if (str == SHVString8C("/help"))
				{
					printf("Commands available:\n"
						   " /server <port>            Will start a server at a given port\n"
						   " /udp                      Will set udp mode\n"
						   " /udp <port>               Will start an udp socket - set udp mode\n"
						   " /connect <ip> <port>      Will connect to an ip/port as udp if in udp mode\n"
						   " /udpconnect <ip> <port>   Will connect to an ip/port as udp\n"
						   " /disconnect               Disconnects current socket\n"
						   " /quit                     Will quit ...\n"
						   "\n");
				}
				else
				{
					printf("Unknown command\n");
				}
			}
			else if (!Socket.IsNull())
			{
				str.AddChars("\n",1);
				
				if (Socket->GetState() == SHVSocket::StateConnected
					|| (Socket->GetState() == SHVSocket::StateNone && Socket->GetType() == SHVSocket::TypeUDP))
				{
					if (Socket->GetType() == SHVSocket::TypeUDP && UdpIP != 0)
						Socket->SendTo(SHVBufferCPtr((SHVByte*)str.GetSafeBuffer(),str.GetLength()),UdpIP,UdpPort);
					else
						Socket->Send(SHVBufferCPtr((SHVByte*)str.GetSafeBuffer(),str.GetLength()));
				}
				else if (Socket->GetState() == SHVSocket::StateListening)
				{
				SHVListIterator<SHVSocketRef,SHVSocket*> itr(ClientSockets);
				
					while (itr.MoveNext())
					{
						itr.Get()->Send(SHVBufferCPtr((SHVByte*)str.GetSafeBuffer(),str.GetLength()));
					}
				}
			}
		}
	}

	void Unregister()
	{
		printf("In unregister\n");
		Socket = NULL;
		ClientSockets.RemoveAll();
		SHVModule::Unregister();
	}
};


int main(int argc, char *argv[])
{
SHVDll socketlib;
	
	SHVUNUSED_PARAM(argc);
	SHVUNUSED_PARAM(argv);

	if (!SHVModuleList::CheckVersion(__SHIVA_VERSION_MAJOR, __SHIVA_VERSION_MINOR, __SHIVA_VERSION_RELEASE))
	{
		fprintf(stderr,"WRONG SHIVA VERSION\n");
	}
	else if (!socketlib.Load(socketlib.CreateLibFileName(_T("socketserver"))))
	{
		fprintf(stderr,"Could not load socket server\n");
	}
	else
	{
	SHVMainThreadEventQueue mainqueue(new SHVMainThreadEventDispatcherConsole());
	SHVModuleFactory* factory = (SHVModuleFactory*)socketlib.CreateObjectInt(&mainqueue.GetModuleList(),SHVDll::ClassTypeModuleFactory);
	SHVString testStr;

		mainqueue.GetModuleList().AddModule(new SHVTest(mainqueue.GetModuleList()));
		factory->ResolveModules(__MODULESYMBOL_DEFAULTS);

		testStr.Format(_T("This is a test %s %d.%d.%d\n"), _T("of SHIVA version"), __SHIVA_VERSION_MAJOR, __SHIVA_VERSION_MINOR, __SHIVA_VERSION_RELEASE);
	
		printf("%s", testStr.GetSafeBuffer());
		
		return mainqueue.Run().GetError();
	}
	
	return -1;
}
