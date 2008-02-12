
#ifndef __SQLITESTATEMENT_H
#define __SQLITESTATEMENT_H

class RFTSQLiteStatement
{
public:
	virtual ~RFTSQLiteStatement() {}

	virtual short GetValue(long& val, int columnIdx) = 0;
	virtual short GetValue(double& val, int columnIdx) = 0;
	virtual short GetValue(const void*& blob, int& len, int columnIdx) = 0;
	virtual short GetValue(const char*& text, int& len, int columnIdx) = 0;
	virtual short GetColumnName(const char*& name, int columnIdx) = 0;
	virtual short GetColumnType(short& type, int columnIdx) = 0;
	virtual int GetColumnCount() = 0;
	virtual short SetParameter(const char* name, long val) = 0;
	virtual short SetParameter(const char* name, double val) = 0;
	virtual short SetParameter(const char* name, const char* val) = 0;
	virtual short SetParameterNull(const char* name) = 0;

	virtual short NextResult() = 0;
	virtual short Reset() = 0;	
};

#endif