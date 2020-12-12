#include "stdafx.h"
#include "../../../include/platformspc.h"
#include "../include/shvsocketimpl.h"
// #include "shvsslsocket.h"

//=========================================================================================================
// SHVSSLSocketFactory class - Factory for creating SSL socket abstraction object
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVSSLSocketFactory::SHVSSLSocketFactory(): Supported(false)
{
#if defined(_WIN64)
	Supported = OpenSSL.Load(OpenSSL.CreateLibFileName(_S("ssleay64")));
	if (!Supported)
		Supported = OpenSSL.Load(OpenSSL.CreateLibFileName(_S("libssl64")));
#elif defined(_WIN32)
	Supported = OpenSSL.Load(OpenSSL.CreateLibFileName(_S("ssleay32")));
	if (!Supported)
		Supported = OpenSSL.Load(OpenSSL.CreateLibFileName(_S("libssl32")));
#elif defined(__SHIVA_POSIX)
	if (!Supported)
		Supported = OpenSSL.Load(_S("libssl.so.1.0.0"));
	if (!Supported)
		Supported = OpenSSL.Load(_S("libssl.so.1.0"));
#endif
	if (!Supported)
		Supported = OpenSSL.Load(OpenSSL.CreateLibFileName(_S("ssl")));
	Supported = Supported && OpenSSL.Resolve((void**) &_ssl_load_error_strings, _S("SSL_load_error_strings"));
	Supported = Supported && OpenSSL.Resolve((void**) &_ssl_library_init, _S("SSL_library_init"));
	Supported = Supported && OpenSSL.Resolve((void**) &_ssl_new, _S("SSL_new"));
	Supported = Supported && OpenSSL.Resolve((void**) &_ssl_ctx_new, _S("SSL_CTX_new"));
	Supported = Supported && OpenSSL.Resolve((void**) &_ssl_ctx_ctrl, _S("SSL_CTX_ctrl"));
	Supported = Supported && OpenSSL.Resolve((void**) &_sslv23_client_method, _S("SSLv23_client_method"));
	Supported = Supported && OpenSSL.Resolve((void**) &_sslv23_server_method, _S("SSLv23_server_method"));
	Supported = Supported && OpenSSL.Resolve((void**) &_ssl_set_fd, _S("SSL_set_fd"));
	Supported = Supported && OpenSSL.Resolve((void**) &_ssl_connect, _S("SSL_connect"));
	Supported = Supported && OpenSSL.Resolve((void**) &_ssl_accept, _S("SSL_accept"));
	Supported = Supported && OpenSSL.Resolve((void**) &_ssl_shutdown, _S("SSL_shutdown"));
	Supported = Supported && OpenSSL.Resolve((void**) &_ssl_free, _S("SSL_free"));
	Supported = Supported && OpenSSL.Resolve((void**) &_ssl_ctx_free, _S("SSL_CTX_free"));
	Supported = Supported && OpenSSL.Resolve((void**) &_ssl_read, _S("SSL_read"));
	Supported = Supported && OpenSSL.Resolve((void**) &_ssl_write, _S("SSL_write"));
	Supported = Supported && OpenSSL.Resolve((void**) &_ssl_get_error, _S("SSL_get_error"));
	Supported = Supported && OpenSSL.Resolve((void**) &_ssl_use_PrivateKey_file, _S("SSL_use_PrivateKey"));
	Supported = Supported && OpenSSL.Resolve((void**) &_ssl_use_RSAPrivateKey_file, _S("SSL_use_RSAPrivateKey_file"));
	Supported = Supported && OpenSSL.Resolve((void**) &_ssl_use_certificate_file, _S("SSL_use_certificate_file"));
	if (Supported)
	{
		(*_ssl_load_error_strings)();
		(*_ssl_library_init)();
	}
}

/*************************************
 * CreateSSLSocket
 *************************************/
SHVSSLSocket * SHVSSLSocketFactory::CreateSSLSocket()
{
	if (Supported)
		return new SHVSSLSocket(this);
	else
		return NULL;
}

//=========================================================================================================
// SHVSSLSocket class - SSL socket abstraction class
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVSSLSocket::SHVSSLSocket(SHVSSLSocketFactory *factory): Factory(factory), Initialized(false)
{
}

/*************************************
 * Destructor
 *************************************/
SHVSSLSocket::~SHVSSLSocket()
{	
	Disconnect();
	Factory = NULL;
}

/*************************************
 * Connect
 *************************************/
SHVBool SHVSSLSocket::Connect(SHVSocket *socket, bool &retry)
{
SHVSocketImpl* sock = (SHVSocketImpl*)	socket;
SHVBool retVal(false);
	retry = false;
	if (!AlreadyInitialized())
		SSLCTX = (*Factory->_ssl_ctx_new)((*Factory->_sslv23_client_method)());
	if (SSLCTX)
	{
		(*Factory->_ssl_ctx_ctrl)(SSLCTX,SSL_CTRL_MODE,SSL_MODE_ENABLE_PARTIAL_WRITE,NULL);

		if (!AlreadyInitialized())
			SSLHandle = (*Factory->_ssl_new)(SSLCTX);
		if (SSLHandle)
		{
		int ret;
			if (!AlreadyInitialized())
				(*Factory->_ssl_set_fd)(SSLHandle, sock->Socket);
			Initialized = true;
			ret = (*Factory->_ssl_connect)(SSLHandle);
			if (ret != 1)
			{
				retVal.SetError((*Factory->_ssl_get_error)(SSLHandle,ret));
				if (retVal.GetError() == SSL_ERROR_WANT_READ || retVal.GetError() == SSL_ERROR_WANT_WRITE)
				{
					retry = true;
				}
				else
				{
					printf("SSL Connect err %d\n", retVal.GetError());
					Disconnect();
					Initialized = false;
				}
			}
			else
			{
				retVal = true;
			}
		}
	}
	return retVal;
}

/*************************************
 * Accept
 *************************************/
SHVBool SHVSSLSocket::Accept(SHVSocket *socket, const char *keyFile, const char *certFile, bool &retry)
{
SHVSocketImpl* sock = (SHVSocketImpl*)	socket;
SHVBool retVal(false);
	retry = false;
	if (!AlreadyInitialized())
		SSLCTX = (*Factory->_ssl_ctx_new)((*Factory->_sslv23_server_method)());
	if (SSLCTX)
	{
		if (!AlreadyInitialized())
			SSLHandle = (*Factory->_ssl_new)(SSLCTX);
		if (SSLHandle)
		{
		int ret = 1;
			if (!AlreadyInitialized())
			{
				ret = (*Factory->_ssl_use_RSAPrivateKey_file)(SSLHandle, keyFile, SSL_FILETYPE_PEM);
				if (ret == 1)
				{
					ret = (*Factory->_ssl_use_certificate_file)(SSLHandle, certFile, SSL_FILETYPE_PEM);
				}
				else
				{
					Disconnect();
				}
			}
			if (ret == 1)
			{
				if (!AlreadyInitialized())
					(*Factory->_ssl_set_fd)(SSLHandle, sock->Socket);
				Initialized = true;
				ret = (*Factory->_ssl_accept)(SSLHandle);
			}
			else
			{
				Disconnect();
			}
			if (ret != 1)
			{
				retVal.SetError((*Factory->_ssl_get_error)(SSLHandle,ret));
				if (retVal.GetError() == SSL_ERROR_WANT_READ || retVal.GetError() == SSL_ERROR_WANT_WRITE)
				{
					retry = true;
				}
				else
				{
					Disconnect();
				}
			}
			else
			{
				retVal = true;
			}
		}
	}
	return retVal;
}

/*************************************
 * Disconnect
 *************************************/
void SHVSSLSocket::Disconnect()
{
	Initialized = false;
	if (SSLHandle)
	{
		(*Factory->_ssl_shutdown)(SSLHandle);
		(*Factory->_ssl_free)(SSLHandle);
		SSLHandle = NULL;
	}
	if (SSLCTX)
	{
		(*Factory->_ssl_ctx_free)(SSLCTX);
		SSLCTX = NULL;
	}
}

/*************************************
 * send
 *************************************/
int SHVSSLSocket::send(const void *buffer, int n, bool &retry)
{
int retVal = -1;
	retry = false;
	if (!SSLHandle)
		return -1;
	retVal = (*Factory->_ssl_write)(SSLHandle, buffer, n);
	if (retVal < 0)
	{
	int sslerror = (*Factory->_ssl_get_error)(SSLHandle, retVal);
		if (sslerror == SSL_ERROR_WANT_READ || sslerror == SSL_ERROR_WANT_WRITE)
			retry = true;
	}
	return retVal;
}

/*************************************
 * recv
 *************************************/
int SHVSSLSocket::recv(void *buffer, int n, bool &retry)
{
int retVal = -1;
	retry = false;
	if (!SSLHandle)
	return -1;
	retVal = (*Factory->_ssl_read)(SSLHandle, buffer, n);
	if (retVal < 0)
	{
		retVal = (*Factory->_ssl_get_error)(SSLHandle, retVal);
		if (retVal == SSL_ERROR_WANT_READ || retVal == SSL_ERROR_WANT_WRITE)
			retry = true;
		else
			retVal *= -1;
	}
	return retVal;
}
