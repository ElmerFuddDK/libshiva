#ifndef __SHIVA_SOCKETSERVERIMPL_SOCKET_H
#define __SHIVA_SOCKETSERVERIMPL_SOCKET_H

#include "../../../include/modules/socketserver/shvsocketserver.h"
#ifdef __SHIVA_WIN32
# define __SHIVASOCKETS_NOSELECTMODE
# include <winsock2.h>
# define SHVSOCKTYPE SOCKET
#else
# define SHVSOCKTYPE int
#endif

#ifdef __SHIVASOCKETS_NOSELECTMODE
# include "../../../include/threadutils/shvthread.h"
# include "../../../include/threadutils/shvmutexbase.h"
#endif
#include "shvsslsocket.h"

class SHVSocketServerImpl;
class SHVSocketServerThread;

//-=========================================================================================================
/// SHVSocketImpl class - Implementation of sockets interface
//-=========================================================================================================
/**
 * This class is used for listening, streaming and datagram socket traffic.
 \note It functions as an interface to a platform specific socket implementation class.
 */

class SHVSocketImpl : public SHVSocket
{
public:
	enum SSLStates {
		SSLStateNone = -1,
		SSLStateAccepting = 1001,
		SSLStateConnecting = 1002
	};

	// properties
	virtual Types GetType();
	
	
	// Operations
	virtual SHVBool BindAndListen(SHVIPv4Port port);
	virtual SHVBool BindAndListenUnix(const SHVStringC fileName);
	virtual SHVBool Close();
	virtual SHVBool Shutdown();
	virtual SHVBool ConnectAny(SHVIPv4Port port);
	virtual SHVBool Connect(SHVIPv4Addr ip, SHVIPv4Port port);
	virtual SHVBool Connect(const SHVStringC ipv4Addr, SHVIPv4Port port);
	virtual SHVBool ConnectUnix(const SHVStringC fileName);
	
	virtual SHVBool Send(const SHVBufferC& buf);
	virtual SHVBool SendTo(const SHVBufferC& buf, SHVIPv4Addr ip, SHVIPv4Port port);

	virtual SHVBool SetReceiveBufferSize(size_t sz);
	virtual size_t GetReceiveBufferSize();
	virtual SHVBuffer* PopReceiveBuffer(size_t& bytesRead);
	virtual SHVBuffer* PopReceiveBuffer(size_t& bytesRead, SHVIPv4Addr &fromIP, SHVIPv4Port &fromPort);
	virtual SHVBool HasReceivedData();

	virtual SHVBool SetSocketOption(SocketOptions option, int val1, int val2 = 0);
	virtual SHVBool GetSocketOption(SocketOptions option, int& val1, int& val2);

	virtual SHVBool SetServerCertificate(const SHVStringC keyfile, const SHVStringC certFile);
	virtual SHVBool StartTLS(); // Converts a connected TCP socket to SSL

protected:
friend class SHVSocketServerImpl;
friend class SHVSocketServerThread;
friend class SHVSSLSocket;

	///\cond INTERNAL
	SHVSocketImpl(SHVEventSubscriberBase* subs, SHVSocketServerImpl* socketServer, SHVSocket::Types type);
	SHVSocketImpl(SHVEventSubscriberBase* subs, SHVSocketServerImpl* socketServer, SHVSocket::Types type, SHVSOCKTYPE sock);
	virtual ~SHVSocketImpl();
	
	enum { InvalidSocket = -1 };
	
	void PerformEvent();
	SHVBool SetError(SHVBool err = ErrGeneric);
	int RetreiveSocketError();
	SHVBool SSLAccept(const char* keyFile, const char* certFile);
	bool SSLConnect();

	SHVEventSubscriberBaseRef EventSubscriber;
	SHVSocketServerImpl* SocketServer;
	SHVSocket::Types Type;
	
	struct BufferInstance
	{
		SHVBufferRef Buffer;
		size_t BytesRead;
		SHVIPv4Addr FromIP;
		SHVIPv4Port FromPort;

		inline BufferInstance(SHVBuffer* buffer, size_t bytesRead, SHVIPv4Addr fromIP, SHVIPv4Port fromPort)
			: Buffer(buffer), BytesRead(bytesRead), FromIP(fromIP), FromPort(fromPort)  {}
	};
	
	SHVList<BufferInstance> ReceiveBuffers;
	size_t BufferSize;
	size_t BytesRead;
	bool RecvPending;
	SHVSSLSocketRef SSL;
	SSLStates SSLState;
	SHVString8 KeyFile;
	SHVString8 CertFile;

#ifdef __SHIVASOCKETS_NOSELECTMODE
	bool IsPending;
	SHVThread<SHVSocketImpl> ReadThread;
	SHVMutexBase ReadThreadSignal;

	void StartReadThread();
	void ReadThreadFunc();
#endif
#if defined(__SHIVA_EPOC)
#error NOT IMPLEMENTED
#endif
	SHVSOCKTYPE Socket;
	///\endcond
};
typedef SHVRefObjectContainer<SHVSocketImpl> SHVSocketImplRef;

#endif
