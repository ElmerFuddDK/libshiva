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
	SHVDataRowListC_Indexed(SHVDataSession* session, const SHVDataStructC* dataStruct, const SHVStringC& condition, size_t index);
protected:
	virtual ~SHVDataRowListC_Indexed();
	virtual int GetRowCount() const;
	virtual SHVStringBufferUTF8 BuildQuery(const SHVStringC& condition, bool reverse);
	virtual void AdjustRowCount(int delta);
private:
	SHVString8 IndexTableName;
	int RowCount;
};
typedef SHVRefObjectContainer<SHVDataRowListC_Indexed> SHVDataRowList_Indexed;



#endif