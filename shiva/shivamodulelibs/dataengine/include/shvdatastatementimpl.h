#ifndef __SHIVA_DATASTATEMENTIMPL_H
#define __SHIVA_DATASTATEMENTIMPL_H

#include "../../../include/modules/dataengine/shvdatastatement.h"
#include "../../../include/sqlite/sqlitestatement.h"
#include "../../../include/utils/shvvectorref.h"
class SHVSQLiteWrapper;


class SHVDataStatementImpl : public SHVDataStatement
{
public:

	SHVDataStatementImpl(SHVSQLiteWrapper* sqlite, const SHVStringC query);

	// IsOk
	virtual SHVBool IsOk() const;

	// GetValue
	virtual SHVBool GetLong(long& val, int columnIdx) const;
	virtual SHVBool GetInt64(SHVInt64Val& val, int columnIdx) const;
	virtual SHVBool GetDouble(double& val, int columnIdx) const;
	virtual SHVBool GetStringUTF8(SHVStringUTF8C& text, int& len, int columnIdx) const;

	// GetColumnName
	virtual SHVBool GetColumnNameUTF8(SHVStringUTF8C& name, int columnIdx) const;
	virtual SHVBool GetColumnName8(SHVString8& name, int columnIdx) const;
	virtual SHVBool GetColumnName16(SHVString16& name, int columnIdx) const;

	// GetColumnType
	virtual SHVBool GetColumnAffinity(short& type, int columnIdx) const;
	virtual SHVBool GetColumnTypeUTF8(SHVStringUTF8C& colType, int columnIdx) const;
	virtual SHVBool GetColumnType8(SHVString8& colType, int columnIdx) const;
	virtual SHVBool GetColumnType16(SHVString16& colType, int columnIdx) const;

	// GetColumnCount
	virtual int GetColumnCount() const;

	// SetParameter
	virtual void ClearAllParameters();

	virtual SHVBool SetParameterLongUTF8(const SHVStringUTF8C& name, long val);
	virtual SHVBool SetParameterInt64UTF8(const SHVStringUTF8C& name, SHVInt64Val val);
	virtual SHVBool SetParameterDoubleUTF8(const SHVStringUTF8C& name, double val);
	virtual SHVBool SetParameterStringUTF8(const SHVStringUTF8C& name, const SHVStringUTF8C& val);
	virtual SHVBool SetParameterNullUTF8(const SHVStringUTF8C& name);

	// Iteration methods
	virtual SHVBool NextResult();
	virtual SHVBool NextRow();
	virtual SHVBool Reset();

private:

	SHVSQLiteStatement* GetCurrentStatement() const;

	SHVVectorRef<SHVSQLiteStatement> Statements;
	size_t CurrentStatement;
	SHVBool Ok;
	
};
typedef SHVRefObjectContainer<SHVDataStatementImpl> SHVDataStatementImplRef;

#endif