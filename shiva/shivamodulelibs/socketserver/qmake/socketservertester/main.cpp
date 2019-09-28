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

#include "../../../../include/platformspc.h"
#include "../../../../include/framework/shvmodule.h"
#include "../../../../include/framework/shvmodulefactory.h"
#include "../../../../include/frameworkimpl/shvmoduleloaderimpl.h"
#include "../../../../include/frameworkimpl/shvmainthreadeventdispatchergeneric.h"
#include "../../../../include/shvversion.h"
#include "../../../../include/utils/shvdll.h"
#include "../../../../include/utils/shvdir.h"
#include "../../../../include/modules/shvmodulefactories.h"
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
		SHVConsole::PrintfUTF8("In register\n");
		
		if (!SHVModuleResolver<SHVSocketServer>(Modules,SocketServer,"SocketServer"))
			return false;
		
		Modules.EventSubscribe(SHVModuleList::EventEndRegister, new SHVEventSubscriber(this));
		Modules.EventSubscribe(SHVModuleList::EventClosing, new SHVEventSubscriber(this));
		Modules.EventSubscribe(__EVENT_GLOBAL_STDIN, new SHVEventSubscriber(this));
		
		UdpIP = 0;
		UdpPort = 0;

		SHVConsole::Printf(_S("Ipv6 supported : %s\n"), SocketServer->IPv6Supported() ? _S("yes") : _S("no"));

		return SHVModule::Register();
	}
	
	void PostRegister()
	{
	}
	
	void OnSocketEvent(SHVEvent* event)
	{
		if (SHVEvent::Equals(event,SHVSocketServer::EventSockStatus))
		{
			switch (event->GetSubID())
			{
			default:
			case SHVSocket::StateError:
				if (SHVSocketServer::SocketFromEvent(event) == Socket)
				{
					SHVConsole::PrintfUTF8("Main socket Error\n");
					ClientSockets.RemoveAll();
					Socket = NULL;
				}
				else
				{
				SHVListPos pos = ClientSockets.Find(SHVSocketServer::SocketFromEvent(event));
				
					SHVConsole::PrintfUTF8("SocketError\n");
					
					if (pos)
					{
						SHVConsole::PrintfUTF8("Removing client socket\n");
						ClientSockets.RemoveAt(pos);
					}
				}
				break;
			case SHVSocket::StateDisconnected:
				if (SHVSocketServer::SocketFromEvent(event) == Socket)
				{
					SHVConsole::PrintfUTF8("Main socket disconnected\n");
					ClientSockets.RemoveAll();
					Socket = NULL;
				}
				else
				{
				SHVListPos pos = ClientSockets.Find(SHVSocketServer::SocketFromEvent(event));
				
					SHVConsole::PrintfUTF8("Socket disconnected\n");
					
					if (pos)
					{
						SHVConsole::PrintfUTF8("Removing client socket\n");
						ClientSockets.RemoveAt(pos);
					}
				}
				break;
			case SHVSocket::StateConnecting:
				SHVConsole::PrintfUTF8("Connecting ...\n");
				break;
			case SHVSocket::StateConnected:
				SHVConsole::PrintfUTF8("Connected\n");
				break;
			case SHVSocket::StateListening:
				SHVConsole::PrintfUTF8("Listening\n");
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
			SHVStringUTF8 str;
				str.AddChars(buffer->GetBufferConst(),bytesRead);
				if (fromIP)
					SHVConsole::Printf(_S("Received from (%s,%d): %s\n"), SocketServer->Inetv4ToAddr(fromIP).GetSafeBuffer(), fromPort, str.ToStrT().GetSafeBuffer());
				else
					SHVConsole::Printf(_S("Received : %s\n"), str.ToStrT().GetSafeBuffer());
			}
		}
		else if (SHVEvent::Equals(event,SHVSocketServer::EventSockIncomingConn))
		{
			SHVConsole::PrintfUTF8("Incomming connection ...\n");
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
		SHVStringUTF8 str(SHVEventStdin::StdinFromEventUTF8(event));
		
			if (str == SHVStringUTF8C("/quit"))
			{
				Modules.CloseApp();
			}
			else if (str == SHVStringUTF8C("/close"))
			{
				if (!Socket.IsNull())
				{
					SHVConsole::PrintfUTF8("Closing socket\n");
					Socket->Close();
					Socket = NULL;
				}
			}
			else if (str.GetSafeBuffer()[0] == '/')
			{
				if (str.Left(8) == SHVStringUTF8C("/server "))
				{
				SHVChar* c;
				SHVIPv4Port port = SHVStringUTF8C::StrToL(str.GetSafeBuffer() + 8, &c);
				
					if (!Socket.IsNull())
					{
						Socket->Close();
						Socket = NULL;
					}
					
					Socket = SocketServer->CreateSocket(SocketSubscriber);
					SHVConsole::PrintfUTF8("Attempting to start server at %d\n", port);
					SHVVERIFY(Socket->BindAndListen(port ? port : 1234));
				}
				else if (str.Left(11) == SHVStringUTF8C("/serverssl "))
				{
				SHVChar* c;
				SHVIPv4Port port = SHVStringUTF8C::StrToL(str.GetSafeBuffer() + 11, &c);

					if (!Socket.IsNull())
					{
						Socket->Close();
						Socket = NULL;
					}

					Socket = SocketServer->CreateSocket(SocketSubscriber, SHVSocket::TypeSSL);
					if (Socket.IsNull())
					{
						SHVConsole::PrintfUTF8("SSL not supported\n");
					}
					else
					{
						if (!Socket->SetServerCertificate(Modules.GetConfig().Find(SHVModuleList::DefaultCfgAppPath)->ToString() + SHVDir::Delimiter() + _S("server.key"),
														 Modules.GetConfig().Find(SHVModuleList::DefaultCfgAppPath)->ToString() + SHVDir::Delimiter() + _S("server.crt")))
						{
							Socket = NULL;
							SHVConsole::PrintfUTF8("Error setting up certificate\n");
						}
						else
						{
							SHVConsole::PrintfUTF8("Attempting to start server at %d\n", port);
							SHVVERIFY(Socket->BindAndListen(port ? port : 1234));
						}
					}
				}
				else if (str.Left(12) == SHVStringUTF8C("/serverunix "))
				{
				SHVStringUTF8 filepath(str.Mid(12));

					if (!Socket.IsNull())
					{
						Socket->Close();
						Socket = NULL;
					}

					Socket = SocketServer->CreateSocket(SocketSubscriber, SHVSocket::TypeUnix);
					if (Socket.IsNull())
					{
						SHVConsole::PrintfUTF8("Unix socket not supported\n");
					}
					else
					{
						Socket->SetSocketOption(SHVSocket::SockOptReuseAddr,1);
						SHVConsole::PrintfUTF8("Attempting to start server at \"%s\"\n", filepath.GetSafeBuffer());
						SHVVERIFY(Socket->BindAndListenUnix(filepath.ToStrT()));
					}
				}
				else if (str.Left(9) == SHVStringUTF8C("/connect "))
				{
				SHVStringUTF8 ip(str.GetSafeBuffer()+9);
				SHVIPv4Port port = 0;
				int space;
				
					if ( (space = ip.ReverseFind(_S(" "))) > 0)
					{
					SHVChar* c;
						port = SHVStringUTF8C::StrToL(ip.GetSafeBuffer() + space, &c);
						ip.GetBuffer()[space] = 0;
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
						SHVConsole::PrintfUTF8("Attempting connect to \"%s\" port %d\n", ip.GetSafeBuffer(), port);
						SHVVERIFY(Socket->Connect(SocketServer->InetResolveHost(ip.ToStrT()),port ? port : 1234));
					}
				}
				else if (str.Left(12) == SHVStringUTF8C("/sslconnect "))
				{
				SHVStringUTF8 ip(str.GetSafeBuffer()+12);
				SHVIPv4Port port = 0;
				int space;

					if ( (space = ip.ReverseFind(_S(" "))) > 0)
					{
					SHVChar* c;
						port = SHVStringUTF8C::StrToL(ip.GetSafeBuffer() + space, &c);
						ip.GetBuffer()[space] = 0;
					}

					if (!Socket.IsNull())
					{
						Socket->Close();
						Socket = NULL;
					}

					Socket = SocketServer->CreateSocket(SocketSubscriber, SHVSocket::TypeSSL);
					if (Socket.IsNull())
					{
						SHVConsole::PrintfUTF8("SSL not supported\n");
					}
					else
					{
						SHVConsole::PrintfUTF8("Attempting secure connect to \"%s\" port %d\n", ip.GetSafeBuffer(), port);
						SHVVERIFY(Socket->Connect(SocketServer->InetResolveHost(ip.ToStrT()),port ? port : 1234));
					}
				}
				else if (str.Left(13) == SHVStringUTF8C("/unixconnect "))
				{
				SHVStringUTF8 filepath(str.Mid(13));

					if (!Socket.IsNull())
					{
						Socket->Close();
						Socket = NULL;
					}

					Socket = SocketServer->CreateSocket(SocketSubscriber, SHVSocket::TypeUnix);
					if (Socket.IsNull())
					{
						SHVConsole::PrintfUTF8("Unix socket not supported\n");
					}
					else
					{
						SHVConsole::PrintfUTF8("Attempting connect to \"%s\"\n", filepath.GetSafeBuffer());
						SHVVERIFY(Socket->ConnectUnix(filepath.ToStrT()));
					}
				}
				else if (str == SHVStringUTF8C("/udp"))
				{
					UdpIP = 0;
					UdpPort = 0;
					
					if (!Socket.IsNull())
					{
						Socket->Close();
						Socket = NULL;
					}
					
					Socket = SocketServer->CreateSocket(SocketSubscriber,SHVSocket::TypeUDP);
					SHVConsole::PrintfUTF8("Created a UDP client socket\n");
				}
				else if (str.Left(5) == SHVStringUTF8C("/udp "))
				{
				SHVChar* c;
				SHVIPv4Port port = SHVStringUTF8C::StrToL(str.GetSafeBuffer() + 5, &c);
				
					UdpIP = 0;
					UdpPort = 0;
					
					if (!Socket.IsNull())
					{
						Socket->Close();
						Socket = NULL;
					}
					
					Socket = SocketServer->CreateSocket(SocketSubscriber,SHVSocket::TypeUDP);
					SHVConsole::PrintfUTF8("Attempting to start UDP at %d\n", port);
					SHVVERIFY(Socket->BindAndListen(port ? port : 1234));
				}
				else if (str.Left(12) == SHVStringUTF8C("/udpconnect "))
				{
				SHVStringUTF8 ip(str.GetSafeBuffer()+12);
				SHVIPv4Port port = 0;
				int space;
				
					if ( (space = ip.ReverseFind(_S(" "))) > 0)
					{
						SHVChar* c;
						port = SHVStringUTF8C::StrToL(ip.GetSafeBuffer() + space, &c);
						ip.GetBuffer()[space] = 0;
					}
					
					if (!Socket.IsNull())
					{
						Socket->Close();
						Socket = NULL;
					}
					
					Socket = SocketServer->CreateSocket(SocketSubscriber,SHVSocket::TypeUDP);
					SHVConsole::PrintfUTF8("Attempting connect to \"%s\" port %d\n", ip.GetSafeBuffer(), port);
					SHVVERIFY(Socket->Connect(SocketServer->InetResolveHost(ip.ToStrT()),port ? port : 1234));
				}
				else if (str.Left(9) == SHVStringUTF8C("/resolve "))
				{
				SHVStringUTF8 host(str.GetSafeBuffer()+9);
				SHVString ip(SocketServer->InetResolveHost(host.ToStrT()));
					if (ip.IsEmpty())
						SHVConsole::PrintfUTF8("Failed resolving host %s\n", host.GetSafeBuffer());
					else
						SHVConsole::PrintfUTF8("Resolved host %s to %s\n", host.GetSafeBuffer(), ip.AsStrUTF8C().GetSafeBuffer());
				}
				else if (str == SHVStringUTF8C("/help"))
				{
					SHVConsole::PrintfUTF8("Commands available:\n"
						   " /server <port>            Will start a server at a given port\n"
						   " /serverssl <port>         Will start a secure server at a given port\n"
						   " /udp                      Will set udp mode\n"
						   " /udp <port>               Will start an udp socket - set udp mode\n"
						   " /connect <ip> <port>      Will connect to an ip/port as udp if in udp mode\n"
						   " /sslconnect <ip> <port>   Will connect to an ip/port as tcp/ssl\n"
						   " /udpconnect <ip> <port>   Will connect to an ip/port as udp\n"
						   " /resolve <host>           Resolves host and prints the result\n"
						   " /quit                     Will quit ...\n"
						   "\n");
				}
				else
				{
					SHVConsole::PrintfUTF8("Unknown command\n");
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
		SHVConsole::PrintfUTF8("In unregister\n");
		Socket = NULL;
		ClientSockets.RemoveAll();
		SHVModule::Unregister();
	}
};


CONSOLEMAIN()
{
SHVDll socketlib;

	if (!SHVModuleList::CheckVersion(__SHIVA_VERSION_MAJOR, __SHIVA_VERSION_MINOR, __SHIVA_VERSION_RELEASE))
	{
		SHVConsole::ErrPrintf8("WRONG SHIVA VERSION\n");
	}
	else
	{
	SHVMainThreadEventQueue mainqueue(new SHVMainThreadEventDispatcherConsole());
	SHVModuleLoaderImpl loader(mainqueue.GetModuleList());
#ifdef SHIVASTATICMODULELIB
		loader.AddModuleFactory(SHVModuleFactory_SocketServerNew(&mainqueue.GetModuleList()));
#else
		loader.AddModuleLibs(mainqueue.GetModuleList().GetConfig().Find(SHVModuleList::DefaultCfgAppPath)->ToString() + SHVDir::Delimiter() + _S("modules"));
#endif

		CONSOLEPARSEARGS(mainqueue.GetModuleList().GetConfig());

		mainqueue.GetModuleList().AddModule(new SHVTest(mainqueue.GetModuleList()));
		loader.AddSymbol(__MODULESYMBOL_DEFAULTS);
		loader.LoadModules();

		return mainqueue.Run().GetError();
	}
	
	return -1;
}
