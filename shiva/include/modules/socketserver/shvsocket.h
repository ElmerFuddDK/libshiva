#ifndef __SHIVA_SOCKETSERVER_SOCKET_H
#define __SHIVA_SOCKETSERVER_SOCKET_H

#include "../../utils/shvbuffer.h"


// typedef for an IPv4 address and port
typedef unsigned int SHVIPv4Addr;
typedef unsigned short SHVIPv4Port;
struct SHVIPv6Addr { unsigned char bytes[16]; };
#define SHVIPv6Port SHVIPv4Port


//-=========================================================================================================
/// SHVSocket class - TCP and UDP socket handling
//-=========================================================================================================
/**
 * This class is used for listening, streaming and datagram socket traffic.
 \note It functions as an interface to a platform specific socket implementation class.
 */

class SHVSocket : public SHVRefObject
{
public:

	// defines/constants
	enum { InvalidIPv4 = 0 };
	enum { DefaultBufferSize = 512 };
	enum States {
		StateNone = 0,
		StateListening,
		StateConnecting,
		StateConnected,
		StateDisconnected,
		StateError = 255
	};

	enum Errors {
		ErrGeneric = SHVBool::False,
		ErrNone = SHVBool::True,
		ErrInvalidOperation = 1,		///< Operation performed is not valid for current state and or type
		ErrInvalidSubscriber = 2,		///< The provided subscriber on creation was NULL
		ErrClosed = 3,					///< Closed and not active
		ErrBinding = 4,					///< Could not bind socket
		ErrListening = 5,				///< Could not listen on bound socket
		ErrListeningSSL = 6,            ///< Could not negotiate SSL connection on bound socket
		ErrSSLConnection = 7			///< Could not create client ssl connection (Negotiation failed)
	};

	enum SocketOptions {
		SockOptKeepalive,
		SockOptKeepaliveIdle,
		SockOptReuseAddr,
		SockOptLinger,
		SockOptTcpNodelay
	};
	
	enum Types {
		TypeTCP,
		TypeUDP,
		TypeSSL,
		TypeUnix
	};


	// Properties
	inline States GetState();
	inline SHVBool GetError();
	inline SHVInt GetTag();

	virtual Types GetType() = 0;
	
	
	// Operations
	virtual SHVBool BindAndListen(SHVIPv4Port port) = 0;
	virtual SHVBool BindAndListenUnix(const SHVStringC fileName) = 0;
	virtual SHVBool Close() = 0;
	virtual SHVBool Shutdown() = 0;
	virtual SHVBool ConnectAny(SHVIPv4Port port) = 0;
	virtual SHVBool Connect(SHVIPv4Addr ip, SHVIPv4Port port) = 0;
	virtual SHVBool Connect6(SHVIPv6Addr ip, SHVIPv6Port port) = 0;
	virtual SHVBool Connect(const SHVStringC ipAddr, SHVIPv4Port port) = 0;
	virtual SHVBool ConnectUnix(const SHVStringC fileName) = 0;
	
	virtual SHVBool Send(const SHVBufferC& buf) = 0;
	virtual SHVBool SendTo(const SHVBufferC& buf, SHVIPv4Addr ip, SHVIPv4Port port) = 0;
	virtual SHVBool SendTo6(const SHVBufferC& buf, SHVIPv6Addr ip, SHVIPv6Port port) = 0;

	virtual SHVBool SetReceiveBufferSize(size_t sz) = 0;
	virtual size_t GetReceiveBufferSize() = 0;
	virtual SHVBuffer* PopReceiveBuffer(size_t& bytesRead) = 0;
	virtual SHVBuffer* PopReceiveBuffer(size_t& bytesRead, SHVIPv4Addr& fromIP, SHVIPv4Port& fromPort) = 0;
	virtual SHVBuffer* PopReceiveBuffer6(size_t& bytesRead, SHVIPv6Addr& fromIP, SHVIPv6Port& fromPort) = 0;
	virtual SHVBool HasReceivedData() = 0;

	virtual SHVBool SetSocketOption(SocketOptions option, int val1, int val2 = 0) = 0;
	virtual SHVBool GetSocketOption(SocketOptions option, int& val1, int& val2) = 0;

	virtual SHVBool SetServerCertificate(const SHVStringC keyfile, const SHVStringC certFile) = 0;
	virtual SHVBool StartTLS() = 0; // Converts a connected TCP socket to SSL

protected:
	///\cond INTERNAL
	int Tag;
	States State;
	SHVBool Error;
	inline SHVSocket(SHVInt tag, States state = StateNone) : Tag((int)tag), State(state) {Error = ErrNone;}
	///\endcond
};
typedef SHVRefObjectContainer<SHVSocket> SHVSocketRef;



// ============================================= implementation ============================================= //


// BindAndListen
/** \fn SHVBool SHVSocket::BindAndListen(SHVIPv4Port port)
 \brief Binds a port and starts listening
 \param port The port to listen to
 *
 * This method only works on a socket with state none. It will block
 */

// Close
/** \fn SHVBool SHVSocket::Close()
 \brief Closes an open port
 */

// Connect
/** \fn SHVBool SHVSocket::ConnectAny(SHVIPv4Port port)
 \brief Connects a UDP socket to all addresses and ports
 \param port IP port to connect to
 *
 * This allows a UDP socket to send to any IP address as well as
 * Receive data back on a random port.
 */
/** \fn SHVBool SHVSocket::Connect(SHVIPv4Addr ip, SHVIPv4Port port)
 \brief Connects a socket
 \param ip IP address to connect to
 \param port IP port to connect to
 */
/** \fn SHVBool SHVSocket::Connect(const SHVStringC ipv4Addr, SHVIPv4Port port)
 \brief Connects a socket
 \param ipv4Addr IPv4 address to connect to
 \param port IP port to connect to
 */

// Set ReceiveBufferSize
/** \fn SHVBool SHVSocket::SetReceiveBufferSize(size_t sz)
 \brief Sets the receive buffer size - indicates max packet size for UDP
 \param sz new size for receive buffer
 *
 \note The new receive buffer size only affects new data received - already received data will have the old size
 */

// GetReceiveBufferSize
/** \fn size_t SHVSocket::GetReceiveBufferSize()
 \brief Obtains the receive buffer size
 */

// PopReceiveBuffer
/** \fn SHVBuffer* SHVSocket::PopReceiveBuffer(size_t& bytesRead)
 \brief Returns a buffer with data received, or null if none
 */


/*************************************
 * GetState
 *************************************/
SHVSocket::States SHVSocket::GetState()
{
	return State;
}

/*************************************
* GetError
*************************************/
SHVBool SHVSocket::GetError()
{
	return Error;
}

/*************************************
* GetTag
*************************************/
SHVInt SHVSocket::GetTag()
{
	return Tag;
}

#endif
