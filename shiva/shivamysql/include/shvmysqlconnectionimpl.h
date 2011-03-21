#ifndef __SHIVA_MYSQL_CONNECTION_IMPL_H
#define __SHIVA_MYSQL_CONNECTION_IMPL_H

#include "../../include/mysql/shvmysqlconnection.h"
#include "../../include/mysql/shvmysqlresultset.h"
#include <mysql.h>

class SHVMySQLConnectionImpl: public SHVMySQLConnection
{
public:
	SHVMySQLConnectionImpl();
	virtual ~SHVMySQLConnectionImpl();

	virtual const SHVMySQLConnectionParms* GetParms() const;
	virtual SHVBool Connect(const SHVMySQLConnectionParms* parms);
	virtual SHVBool IsOk();

	virtual SHVMySQLResultSet* Query(const SHVStringUTF8 query);
	virtual SHVBool Execute(const SHVStringUTF8 sql);
	virtual SHVStringBuffer GetError();
protected:
	virtual void* GetProvider();

private:
  MYSQL* Connection;
  SHVMySQLConnectionParmsRef Parms;
};

#endif
