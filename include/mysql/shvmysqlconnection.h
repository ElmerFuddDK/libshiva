#ifndef __SHIVA_MYSQL_CONNECTION_H
#define __SHIVA_MYSQL_CONNECTION_H

#include "../../include/utils/shvstring.h"
#include "../../include/utils/shvstringutf8.h"
#include "../../include/utils/shvrefobject.h"

// forward declares
class SHVMySQLResultSet;


//-=========================================================================================================
/// SHVMySQLConnectionParms class - Parameters for setting up a MySQL connection
/**
 * This class stores the information needed to connect to MySQL database server.
 * The sole purpose is to be able to conviently centralize the connection information.
 */

class SHVMySQLConnectionParms: public SHVRefObject
{
public:
  inline SHVMySQLConnectionParms(
    const SHVStringUTF8C host, 
    const SHVStringUTF8C user, 
    const SHVStringUTF8C passwd, 
    const SHVStringUTF8C db, 
    unsigned int port, 
    SHVStringUTF8C Unix_Socket = SHVStringUTF8C(NULL), 
    unsigned long Client_Flag = 0
  );
  inline const SHVStringUTF8C& GetHost() const;
  inline const SHVStringUTF8C& GetUser() const;
  inline const SHVStringUTF8C& GetPasswd() const;
  inline const SHVStringUTF8C& GetDb() const;
  inline unsigned int GetPort() const;
  inline const SHVStringUTF8C& GetUnix_Socket() const;
  inline unsigned long GetClient_Flag() const;
private:
  SHVStringUTF8 Host;
  SHVStringUTF8 User;
  SHVStringUTF8 Passwd;
  SHVStringUTF8 Db;
  unsigned int Port;
  SHVStringUTF8 Unix_Socket;
  unsigned long Client_Flag;
};
typedef SHVRefObjectContainer<SHVMySQLConnectionParms> SHVMySQLConnectionParmsRef;

//-=========================================================================================================
/// SHVMySQLConnection class - Class that holds the MySQL connection structure
/**
 * This class is used to connect to the MySQL database server. It also serves as a factory
 * for SHVMySQLResultSet which is the result of queries performed on the server.
 */
class SHVMySQLConnection: public SHVRefObject
{
public:
  virtual const SHVMySQLConnectionParms* GetParms() const = 0;
  virtual SHVBool Connect(const SHVMySQLConnectionParms* parms) = 0;
  virtual SHVBool IsOk() = 0;

  virtual SHVMySQLResultSet* Query(const SHVStringUTF8 query) = 0;
  virtual SHVBool Execute(const SHVStringUTF8 sql) = 0;
  virtual SHVStringBuffer GetError() = 0;
protected:


  friend class SHVMySQLResultSet;

  virtual void* GetProvider() = 0;

};
typedef SHVRefObjectContainer<SHVMySQLConnection> SHVMySQLConnectionRef;

// ============================================= implementation ============================================= //
/*************************************
 * Constructor
 *************************************/
SHVMySQLConnectionParms::SHVMySQLConnectionParms(
  const SHVStringUTF8C host, 
  const SHVStringUTF8C user, 
  const SHVStringUTF8C passwd, 
  const SHVStringUTF8C db, 
  unsigned int port, 
  SHVStringUTF8C unix_socket, 
  unsigned long client_flag
) : Host(host), User(user), Passwd(passwd), Db(db), Port(port), Unix_Socket(unix_socket), Client_Flag(client_flag)
{
}

/*************************************
 * GetHost
 *************************************/
const SHVStringUTF8C& SHVMySQLConnectionParms::GetHost() const
{
  return Host;
}

/*************************************
 * GetUser
 *************************************/
const SHVStringUTF8C& SHVMySQLConnectionParms::GetUser() const
{
  return User;
}

/*************************************
 * GetPasswd
 *************************************/
const SHVStringUTF8C& SHVMySQLConnectionParms::GetPasswd() const
{
  return Passwd;
}

/*************************************
 * GetDb
 *************************************/
const SHVStringUTF8C& SHVMySQLConnectionParms::GetDb() const
{
  return Db;
}

/*************************************
 * GetPort
 *************************************/
unsigned int SHVMySQLConnectionParms::GetPort() const
{
  return Port;
}

/*************************************
 * GetUnix_Socket
 *************************************/
const SHVStringUTF8C& SHVMySQLConnectionParms::GetUnix_Socket() const
{
  return Unix_Socket;
}

/*************************************
 * GetClient_Flag
 *************************************/
unsigned long SHVMySQLConnectionParms::GetClient_Flag() const
{
  return Client_Flag;
}

#endif
