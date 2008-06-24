#ifndef __SHIVA_SOCKETSERVERIMPL_SOCKET_H
#define __SHIVA_SOCKETSERVERIMPL_SOCKET_H

#include "../../../include/modules/socketserver/shvsocketserver.h"



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


	// properties
	virtual Types GetType();
	
	
	// Operations
	virtual SHVBool BindAndListen(SHVIPv4Port port);
	virtual SHVBool Close();
	virtual SHVBool Connect(SHVIPv4Addr ip, SHVIPv4Port port);
	virtual SHVBool Connect(const SHVStringC ipv4Addr, SHVIPv4Port port);
	
	virtual SHVBool Send(const SHVBufferC& buf);

	virtual SHVBool SetReceiveBufferSize(size_t sz);
	virtual size_t GetReceiveBufferSize();
	virtual SHVBuffer* PopReceiveBuffer(size_t& bytesRead);
	virtual SHVBool HasReceivedData();


protected:
friend class SHVSocketServerImpl;
friend class SHVSocketServerThread;

	///\cond INTERNAL
	SHVSocketImpl(SHVEventSubscriberBase* subs, SHVSocketServerImpl* socketServer, SHVSocket::Types type);
	SHVSocketImpl(SHVEventSubscriberBase* subs, SHVSocketServerImpl* socketServer, SHVSocket::Types type, int sock);
	virtual ~SHVSocketImpl();
	
	enum { InvalidSocket = -1 };
	
	void PerformEvent();
	SHVBool SetError(SHVBool err = ErrGeneric);
	int RetreiveSocketError();

	SHVEventSubscriberBaseRef EventSubscriber;
	SHVSocketServerImpl* SocketServer;
	SHVSocket::Types Type;
	
	struct BufferInstance
	{
		SHVBufferRef Buffer;
		size_t BytesRead;

		inline BufferInstance(SHVBuffer* buffer, size_t bytesRead) : Buffer(buffer), BytesRead(bytesRead)  {}
	};
	
	SHVList<BufferInstance> ReceiveBuffers;
	size_t BufferSize;
	size_t BytesRead;
	bool RecvPending;

#ifdef __SHIVA_WIN32
#error NOT IMPLEMENTED
#elif defined(__SHIVA_EPOC)
#error NOT IMPLEMENTED
#else
	int Socket;
#endif
	///\endcond
};
typedef SHVRefObjectContainer<SHVSocketImpl> SHVSocketImplRef;

#endif
