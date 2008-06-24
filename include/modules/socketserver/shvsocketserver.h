#ifndef __SHIVA_SOCKETSERVER_SOCKETSERVER_H
#define __SHIVA_SOCKETSERVER_SOCKETSERVER_H

#include "../../framework/shvmodule.h"
#include "shvsocket.h"

#define __SOCKETSERVER_VERSION_MAJOR   0
#define __SOCKETSERVER_VERSION_MINOR   1
#define __SOCKETSERVER_VERSION_RELEASE 0


//-=========================================================================================================
/// SHVSocketServer class - TCP and UDP socket server for async sockets
//-=========================================================================================================
/**
 * This class is the server for socket operations - it will handle the distribution of socket events
 * via the subscriber for each socket.\n
 * The sockets themselves are identified by a Tag, wich is distributed on the event as SubID. You can
 * use this Tag to obtain a reference to the socket from the server, or compare the socket reference
 * you already have to identify the socket in question.\n
 */

class SHVSocketServer : public SHVModule
{
public:


	// events
	enum Events {
		EventSockStatus,		///< Distributed on the socket subscriber when status is changed
		EventSockDataRead,		///< Distributed on the socket subscriber when data has been read
		EventSockIncomingConn	///< Distributed on the socket subscriber when an incoming connection is
									 /// accepted - event object will be the new socket
	};

	// methods
	virtual SHVEventQueue* GetQueue() = 0;
	virtual SHVSocket* CreateSocket(SHVEventSubscriberBase* subs, SHVSocket::Types type = SHVSocket::TypeTCP, size_t bufferSize = SHVSocket::DefaultBufferSize) = 0;
	virtual SHVSocket* GetSocket(SHVInt tag) = 0;
	
	virtual SHVIPv4Addr Inetv4Addr(const SHVStringC strIp) = 0;


	// Obtain socket from a socket server event
	inline static SHVSocket* SocketFromEvent(SHVEvent* event);
	inline static SHVSocket* NewSocketFromEvent(SHVEvent* event);

protected:
	///\cond INTERNAL
	inline SHVSocketServer(SHVModuleList& modules) : SHVModule(modules,"SocketServer") {}
	///\endcond
};



// ============================================= implementation ============================================= //

/*************************************
 * SocketFromEvent
 *************************************/
SHVSocket* SHVSocketServer::SocketFromEvent(SHVEvent* event)
{
	return (SHVSocket*)event->GetObject();
}

/*************************************
 * NewSocketFromEvent
 *************************************/
SHVSocket* SHVSocketServer::NewSocketFromEvent(SHVEvent* event)
{
	return (SHVSocket*)event->GetData();
}

#endif
