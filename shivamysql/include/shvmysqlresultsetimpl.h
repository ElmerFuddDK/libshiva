#ifndef __SHIVA_MYSQL_RESULTSET_IMPL_H
#define __SHIVA_MYSQL_RESULTSET_IMPL_H


#include "shvmysqlconnectionimpl.h"

class SHVMySQLResultSetImpl: public SHVMySQLResultSet
{
public:
	SHVMySQLResultSetImpl(SHVMySQLConnection* connection, SHVStringUTF8 query);
	virtual ~SHVMySQLResultSetImpl();
	virtual SHVBool GetLong(long& val, int columnIdx) const;
	virtual SHVBool GetInt64(SHVInt64Val& val, int columnIdx) const;
	virtual SHVBool GetDouble(double& val, int columnIdx) const;
	virtual SHVBool GetStringUTF8(SHVStringUTF8& text, int columnIdx) const;
	virtual SHVBool GetDateTime(SHVTime& val, int columnIdx) const;
	virtual SHVBool GetBool(SHVBool& val, int columnIdx) const;
	virtual const char* GetRawData(int columnIdx) const;

	// GetColumnName
	virtual SHVBool GetColumnNameUTF8(SHVStringUTF8& name, int columnIdx) const;
	virtual SHVBool GetColumnName8(SHVString8& name, int columnIdx) const;
	virtual SHVBool GetColumnName16(SHVString16& name, int columnIdx) const;
	virtual int GetColumnCount() const;

	virtual Types GetColumnType(int columnIdx) const;
	virtual int GetNativeType(int columnIdx) const;
	virtual int GetColumnLen(int columnIdx) const;
	virtual SHVBool NextRow();
	virtual void Reset();
	virtual SHVBool IsOk() const;
protected:
	virtual SHVMySQLConnection* GetConnection();
	SHVBool PerformQuery();
	inline MYSQL* GetMYSQL();
private:
	SHVMySQLConnectionRef Connection;
	MYSQL_RES* Result;
	MYSQL_ROW CurrentRow;
	int NumFields;
	SHVStringUTF8 Query;
	SHVBool Ok;
};


// ============================================= implementation ============================================= //

MYSQL* SHVMySQLResultSetImpl::GetMYSQL()
{
	return (MYSQL*) GetProvider();
}

#endif
