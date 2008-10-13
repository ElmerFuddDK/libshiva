#ifndef __SHIVA_DATAENGINE_DATAROWLIST_INDEXED_H
#define __SHIVA_DATAENGINE_DATAROWLIST_INDEXED_H

#include "shvdatarowlistcsqlite.h"

//-=========================================================================================================
/// SHVDataRowList_Indexed class - SQLite implementation of a datarow list.
/**
 * Keep the sortorder if it has been changed, inserted or deleted.
 */
class SHVDataRowCSQLite;
class SHVDataRowListCIndexed: public SHVDataRowListCSQLite
{
public:
	SHVDataRowListCIndexed(SHVDataSession* session, const SHVDataStructC* dataStruct, const SHVString8C& alias, const SHVStringC& condition, size_t index);

	// from SHVDataRowListSQLite
	virtual SHVBool Reset();
	virtual int GetRowCount() const;
	virtual SHVDataRowKey* GetPosition();

protected:
	virtual ~SHVDataRowListCIndexed();

	// from SHVDataRowListSQLite
	virtual SHVStringBufferUTF8 BuildQuery(const SHVStringC& condition, bool reverse);
	virtual void InitializeFind();
	virtual SHVBool InternalRowChanged(SHVDataRow* row);

private:
	SHVString8 IndexTableName;
};
typedef SHVRefObjectContainer<SHVDataRowListCIndexed> SHVDataRowList_Indexed;



#endif