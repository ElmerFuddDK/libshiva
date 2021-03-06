#ifndef __SHIVA_DATAENGINE_DATAROWLISTC_SQLITE_H
#define __SHIVA_DATAENGINE_DATAROWLISTC_SQLITE_H

#include "../../../include/sqlite/sqlitewrapper.h"
#include "../../../include/utils/shvrefobject.h"
#include "../../../include/utils/shvstring.h"
#include "../../../include/utils/shvvectorref.h"
#include "../../../include/shvtypes.h"
#include "../../../include/modules/dataengine/shvdatarowlistc.h"
#include "../../../include/modules/dataengine/shvdatarowc.h"

// forward declares
class SHVDataFunctionImpl;

//-=========================================================================================================
/// SHVDataRowListCSQLite class - SQLite implementation of a constant datarow list.
/**
 * This object does not contain any data, but is merely an interface for a sqlite statement.
 */
class SHVDataRowListCSQLite: public SHVDataRowListC
{
public:
	SHVDataRowListCSQLite(SHVDataSession* session, const SHVDataStructC* dataStruct, const SHVString8C& alias);
#if __SHVSTRINGDEFAULT != utf8
	SHVDataRowListCSQLite(SHVDataSession* session, const SHVStringC& sql, const SHVDataRowKey* sortKey);
#endif
	SHVDataRowListCSQLite(SHVDataSession* session, const SHVStringUTF8C& sql, const SHVDataRowKey* sortKey);
	SHVDataRowListCSQLite(SHVDataSession* session, const SHVDataStructC* dataStruct, const SHVString8C& alias, const SHVStringC& condition, size_t index);

	// from SHVDataListC
	virtual const SHVDataRowC* GetCurrentRow() const;
	virtual SHVBool IsOk() const;
	virtual const SHVDataStructC* GetStruct() const;
	virtual const SHVString8C GetAlias() const;
	virtual int GetAliasID() const;
	virtual int GetRowCount() const;

	virtual SHVDataRowC* Find(const SHVDataRowKey* key);
	virtual SHVDataRowKey* GetPosition();
	virtual SHVBool SetPosition(const SHVDataRowKey* key);
	virtual SHVBool NextRow();
	virtual SHVDataRowListC* Reverse(const SHVStringC& condition);
	virtual SHVBool Reset();

	virtual const void* GetRowProvider() const;
	virtual SHVDataSession* GetDataSession();
	virtual SHVBool RowListValid() const;

protected:
	~SHVDataRowListCSQLite();

	virtual SHVStringBufferUTF8 BuildQuery(const SHVStringC& condition, bool reverse);
	virtual void InitializeFind();
	
	// from SHVDataRowListC
	virtual void AdjustRowCount(int delta);
	virtual SHVBool InternalRowChanged(SHVDataRow* row);

	void LockShared();
	void Unlock();

private:
friend class SHVDataFunctionImpl;
	SHVDataRowListCSQLite(SHVDataSession* session, SHVSQLiteStatement* statement, const SHVDataStructC* dataStruct, const SHVString8C& alias, size_t index);	
	SHVDataStructCRef StructCache;
	SHVDataRowCRef CurrentRow;
	SHVDataSessionRef DataSession;
	int AliasID;

protected:
	size_t SortIndex;
	SHVSQLiteStatementRef Statement;
	bool Eof;
	bool Bof;
	SHVBool Ok;
	int RowCount;
	SHVString8 Alias;
	bool HasShareLock;
};
typedef SHVRefObjectContainer<SHVDataRowListCSQLite> SHVDataRowListCSQLiteRef;


// ==================================== implementation - SHVDataRowList ==================================== //


#endif
