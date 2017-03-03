#ifndef __SHIVA_SOCKETSERVERIMPL_SOCKETSERVER_H
#define __SHIVA_SOCKETSERVERIMPL_SOCKETSERVER_H

#include "shvsocketserverthread.h"
#include "shvsslsocket.h"


//-=========================================================================================================
/// SHVSocketServerImpl class - TCP and UDP socket server for async sockets
//-=========================================================================================================
/**
 * This class is the server for socket operations - it will handle the distribution of socket events
 * via the subscriber for each socket.
 */

class SHVSocketServerImpl : public SHVSocketServer
{
public:


	// constructor
	SHVSocketServerImpl(SHVModuleList& modules);
	~SHVSocketServerImpl();


	// module functions
	virtual SHVBool Register();
	virtual void PostRegister();
	virtual void Unregister();


	// from SHVSocketServer
	virtual SHVEventQueue* GetQueue();
	virtual SHVSocket* CreateSocket(SHVEventSubscriberBase* subs, SHVSocket::Types type = SHVSocket::TypeTCP, size_t bufferSize = SHVSocket::DefaultBufferSize);
	virtual SHVSocket* GetSocket(SHVInt tag);
	
	virtual SHVIPv4Addr Inetv4Addr(const SHVStringC strIp);
	virtual SHVStringBuffer Inetv4ToAddr(SHVIPv4Addr ip);
	virtual SHVIPv4Addr Inetv4ResolveHost(const SHVStringC host);

	virtual SHVIPv6Addr Inetv6Addr(const SHVStringC strIp);
	virtual SHVStringBuffer Inetv6ToAddr(SHVIPv6Addr ip);
	virtual SHVIPv6Addr Inetv6ResolveHost(const SHVStringC host);

	virtual bool SocketTypeSupported(SHVSocket::Types type);
	virtual bool IPv6Supported();

private:
friend class SHVSocketServerThread;
friend class SHVSocketImpl;

	///\cond INTERNAL
	void AddToList(SHVSocketImpl* sock);
	void RemoveFromList(SHVSocketImpl* sock);
	static bool IPv6SupportedInternal();
	SHVIPv6Addr IPv4ToIPv6(SHVIPv4Addr ipAddr);
	inline SHVModuleList& GetModules() { return Modules; }
	
#ifdef __SHIVA_WIN32
	bool WSAInitialized;
#endif
#ifdef __SHIVASOCKETS_NOSELECTMODE
	SHVMutexBase ThreadSignal; // for signalling the socket thread
#endif
	SHVMutex SocketServerLock; // hit and run lock
	SHVList<SHVSocketImplRef,SHVSocketImpl*> SocketList;

	SHVSocketServerThread SocketServerThread;
	SHVSSLSocketFactoryRef SSLFactory;
	///\endcond
};

#endif
