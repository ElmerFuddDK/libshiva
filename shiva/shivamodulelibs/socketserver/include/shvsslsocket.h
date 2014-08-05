#ifndef __SHIVA_SSLSOCKET_H
#define __SHIVA_SSLSOCKET_H

#include "../../../include/utils/shvdllbase.h"
#include "../../../include/utils/shvrefobject.h"

//=========================================================================================================
// openssl types and defines
//=========================================================================================================

typedef void* SSL;
typedef void* SSL_CTX;
typedef void* SSL_METHOD;

#define SSL_ERROR_NONE			0
#define SSL_ERROR_SSL			1
#define SSL_ERROR_WANT_READ		2
#define SSL_ERROR_WANT_WRITE		3
#define SSL_ERROR_WANT_X509_LOOKUP	4
#define SSL_ERROR_SYSCALL		5 /* look at error stack/return value/errno */
#define SSL_ERROR_ZERO_RETURN		6
#define SSL_ERROR_WANT_CONNECT		7
#define SSL_ERROR_WANT_ACCEPT		8

#define X509_FILETYPE_PEM	1
#define X509_FILETYPE_ASN1	2
#define X509_FILETYPE_DEFAULT	3

#define SSL_FILETYPE_ASN1	X509_FILETYPE_ASN1
#define SSL_FILETYPE_PEM	X509_FILETYPE_PEM

#define SSL_CTRL_MODE  33
#define SSL_MODE_ENABLE_PARTIAL_WRITE 0x00000001L

//-=========================================================================================================
/// SHVSSLSocketFactory class - Creates a SSLSocket object
//-=========================================================================================================
/**
 * This class handles instantiation secure socket connections
 * Only supports openssl at this time (maybe always)
 */

class SHVSocket;
class SHVSSLSocket;

class SHVSSLSocketFactory: public SHVRefObject
{
public:
	SHVSSLSocketFactory();

	SHVSSLSocket* CreateSSLSocket();
	inline bool IsSupported();

private:
friend class SHVSSLSocket;
	void (*_ssl_load_error_strings)(void );
	int (*_ssl_library_init)(void );
	SSL* (*_ssl_new)(SSL_CTX* ctx);
	long (*_ssl_ctx_ctrl)(SSL_CTX* ctx,int cmd,long larg,void *parg);
	SSL_CTX* (*_ssl_ctx_new)(SSL_METHOD *meth);
	SSL_METHOD* (*_sslv23_client_method)(void);
	SSL_METHOD* (*_sslv23_server_method)(void);
	int	(*_ssl_set_fd)(SSL *s, int fd);
	int (*_ssl_connect)(SSL *ssl);
	int (*_ssl_accept)(SSL *ssl);
	int (*_ssl_shutdown)(SSL *s);
	void (*_ssl_free)(SSL *ssl);
	void (*_ssl_ctx_free)(SSL_CTX *);
	int (*_ssl_read)(SSL *ssl,void *buf,int num);
	int (*_ssl_write)(SSL *ssl,const void *buf,int num);
	int	(*_ssl_get_error)(const SSL *s,int ret_code);
	int	(*_ssl_use_PrivateKey_file)(SSL *ssl, const char *file, int type);
	int	(*_ssl_use_RSAPrivateKey_file)(SSL *ssl, const char *file, int type);
	int	(*_ssl_use_certificate_file)(SSL *ssl, const char *file, int type);
	SHVDllBase OpenSSL;
	bool Supported;
};
typedef SHVRefObjectContainer<SHVSSLSocketFactory> SHVSSLSocketFactoryRef;

//-=========================================================================================================
/// SHVSSLSocket class - Secure socket using openssl
//-=========================================================================================================
/**
 * Secure socket class
 */
class SHVSSLSocket : public SHVRefObject
{
public:
	virtual ~SHVSSLSocket();

	SHVBool Connect(SHVSocket* socket, bool& retry);
	SHVBool Accept(SHVSocket* socket, const char* keyFile, const char* certFile, bool& retry);
	void Disconnect();

	int send(const void* buffer, int n, bool& retry);
	int recv(void* buffer, int n, bool& retry);

	inline bool AlreadyInitialized();

private:
friend class SHVSSLSocketFactory;

	SHVSSLSocket(SHVSSLSocketFactory* factory);
	SHVSSLSocketFactoryRef Factory;
	SSL* SSLHandle;
	SSL_CTX* SSLCTX;
	bool Initialized;
};
typedef SHVRefObjectContainer<SHVSSLSocket> SHVSSLSocketRef;

// ============================================= implementation ============================================= //

bool SHVSSLSocketFactory::IsSupported()
{
	return Supported;
}

bool SHVSSLSocket::AlreadyInitialized()
{
	return Initialized;
}

#endif
