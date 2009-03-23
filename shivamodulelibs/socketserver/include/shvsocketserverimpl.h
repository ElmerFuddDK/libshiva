#ifndef __SHIVA_SOCKETSERVERIMPL_SOCKETSERVER_H
#define __SHIVA_SOCKETSERVERIMPL_SOCKETSERVER_H

#include "shvsocketserverthread.h"


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


private:
friend class SHVSocketServerThread;
friend class SHVSocketImpl;

	///\cond INTERNAL
	void AddToList(SHVSocketImpl* sock);
	void RemoveFromList(SHVSocketImpl* sock);
	inline SHVModuleList& GetModules() { return Modules; }
	
#ifdef __SHIVA_WINCE
	SHVMutexBase ThreadSignal; // for signalling the socket thread
#endif
	SHVMutex SocketServerLock; // hit and run lock
	SHVList<SHVSocketImplRef,SHVSocketImpl*> SocketList;

	SHVSocketServerThread SocketServerThread;
	///\endcond
};

#endif
