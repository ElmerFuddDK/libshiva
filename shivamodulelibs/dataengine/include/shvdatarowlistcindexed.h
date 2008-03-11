#ifndef __SHIVA_DATAENGINE_DATAROWLIST_INDEXED_H
#define __SHIVA_DATAENGINE_DATAROWLIST_INDEXED_H

#include "shvdatarowlistc_sqlite.h"

//-=========================================================================================================
/// SHVDataRowList_Indexed class - SQLite implementation of a datarow list.
/**
 * Keep the sortorder if have been changed, inserted or deleted.
 */
class SHVDataRowC_SQLite;
class SHVDataRowListC_Indexed: public SHVDataRowListC_SQLite
{
public:
	SHVDataRowListC_Indexed(SHVDataSession* session, const SHVDataStructC* dataStruct, const SHVString8C& alias, const SHVStringC& condition, size_t index);
	virtual SHVBool Reset();
protected:
	virtual ~SHVDataRowListC_Indexed();
	virtual int GetRowCount() const;
	virtual SHVStringBufferUTF8 BuildQuery(const SHVStringC& condition, bool reverse);
	virtual SHVBool TempReset();
	virtual void Reposition();
	virtual SHVBool InternalRowChanged(SHVDataRow* row);
private:
	SHVString8 IndexTableName;
	long TempPos;
};
typedef SHVRefObjectContainer<SHVDataRowListC_Indexed> SHVDataRowList_Indexed;



#endif