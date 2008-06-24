#include "../../../include/platformspc.h"

#include "../include/shvmysqlresultsetimpl.h"

SHVMySQLConnectionImpl::SHVMySQLConnectionImpl()
{
}
 
SHVMySQLConnectionImpl::~SHVMySQLConnectionImpl()
{
	if (Connection)
	{
		mysql_close(Connection);
		Connection = NULL;
	}		
}

const SHVMySQLConnectionParms* SHVMySQLConnectionImpl::GetParms() const
{
	return Parms.AsConst();
}
 
SHVBool SHVMySQLConnectionImpl::Connect(const SHVMySQLConnectionParms* parms)
{
	Connection = mysql_init(NULL);
	Parms = (SHVMySQLConnectionParms*) parms;
	if(!mysql_real_connect(Connection, 
		 					parms->GetHost().GetBufferConst(), 
							parms->GetUser().GetBufferConst(),
							parms->GetPasswd().GetBufferConst(),
							parms->GetDb().GetBufferConst(),
							parms->GetPort(),
							parms->GetUnix_Socket().GetBufferConst(),
							parms->GetClient_Flag()))
	{
		mysql_close(Connection);
		Connection = NULL;
	}
	return Connection != NULL;
}

SHVBool SHVMySQLConnectionImpl::IsOk()
{	
	return Connection != NULL;
}

SHVMySQLResultSet* SHVMySQLConnectionImpl::Query(const SHVStringUTF8 query)
{
	if (IsOk())
		return new SHVMySQLResultSetImpl(this, query);
	else
		return NULL;
}

SHVBool SHVMySQLConnectionImpl::Execute(const SHVStringUTF8 sql)
{
SHVBool retVal;
	if (IsOk())
		retVal.SetError(mysql_query(Connection, sql.GetSafeBuffer()));
	else
		retVal = SHVBool::False;
	return retVal;
}

void* SHVMySQLConnectionImpl::GetProvider()
{
	return Connection;
}

SHVStringBuffer SHVMySQLConnectionImpl::GetError()
{
	if (Connection)
		return SHVStringUTF8(mysql_error(Connection)).ToStrT();
	else
		return SHVString(_T("Not connected")).ReleaseBuffer();
}