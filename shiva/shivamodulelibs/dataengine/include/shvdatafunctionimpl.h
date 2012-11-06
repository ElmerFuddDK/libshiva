#ifndef __SHIVA_DATAFUNCTIONIMPL_H
#define __SHIVA_DATAFUNCTIONIMPL_H

#include "../../../include/modules/dataengine/shvdatafunction.h"
#include "shvdatarowlistcsqlite.h"

class SHVDataFunctionImpl : public SHVDataFunction
{
public:
	SHVDataFunctionImpl(SHVDataRowListCSQLite* rowList);
	SHVDataFunctionImpl(SHVDataSession* session, const SHVStringUTF8C& query, const SHVDataRowKey* sortKey);

	virtual SHVBool SetParameterLongUTF8(const SHVStringUTF8C& name, long val);
	virtual SHVBool SetParameterInt64UTF8(const SHVStringUTF8C& name, SHVInt64Val val);
	virtual SHVBool SetParameterDoubleUTF8(const SHVStringUTF8C& name, double val);
	virtual SHVBool SetParameterStringUTF8(const SHVStringUTF8C& name, const SHVStringUTF8C& val);
	virtual SHVBool SetParameterNullUTF8(const SHVStringUTF8C& name);

	virtual SHVDataRowListC* Exec();
	virtual SHVBool GetLastError();

	virtual bool GetInUse();

private:
	inline SHVBool SetError(SHVBool err);

	SHVDataRowListCSQLiteRef RowList;
	SHVBool LastError;
};


SHVBool SHVDataFunctionImpl::SetError(SHVBool err)
{
	LastError = err;
	return err;
}

#endif // __SHIVA_DATAFUNCTIONIMPL_H
