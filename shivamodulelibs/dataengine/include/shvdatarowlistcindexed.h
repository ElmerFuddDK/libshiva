#ifndef __SHIVA_DATAENGINE_DATAROWLIST_INDEXED_H
#define __SHIVA_DATAENGINE_DATAROWLIST_INDEXED_H

#include "shvdatarowlistcsqlite.h"

//-=========================================================================================================
/// SHVDataRowList_Indexed class - SQLite implementation of a datarow list.
/**
 * Keep the sortorder if have been changed, inserted or deleted.
 */
class SHVDataRowCSQLite;
class SHVDataRowListCIndexed: public SHVDataRowListCSQLite
{
public:
	SHVDataRowListCIndexed(SHVDataSession* session, const SHVDataStructC* dataStruct, const SHVString8C& alias, const SHVStringC& condition, size_t index);
	virtual SHVBool Reset();
protected:
	virtual ~SHVDataRowListCIndexed();
	virtual int GetRowCount() const;
	virtual SHVStringBufferUTF8 BuildQuery(const SHVStringC& condition, bool reverse);
	virtual SHVBool TempReset();
	virtual void Reposition();
	virtual SHVBool InternalRowChanged(SHVDataRow* row);
private:
	SHVString8 IndexTableName;
	long TempPos;
};
typedef SHVRefObjectContainer<SHVDataRowListCIndexed> SHVDataRowList_Indexed;



#endif