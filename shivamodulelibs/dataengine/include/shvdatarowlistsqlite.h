#ifndef __SHIVA_DATAENGINE_DATAROWLIST_SQLITE_H
#define __SHIVA_DATAENGINE_DATAROWLIST_SQLITE_H

#include "../../../../shivasqlite/include/sqlitewrapper.h"
#include "../../../../include/utils/shvrefobject.h"
#include "../../../../include/utils/shvstring.h"
#include "../../../../include/utils/shvvectorref.h"
#include "../../../../include/utils/shvlist.h"
#include "../../../../include/shvtypes.h"
#include "../shvdatarowlist.h"
#include "../shvdatasession.h"
#include "../shvdatarow.h"

//-=========================================================================================================
/// SHVDataRowList_SQLite class - SQLite implementation of a datarow list.
/**
 * Objects of this class handles all the necessary logic for maintaining a sqlite datatable.
 */
class SHVDataRowC_SQLite;
class SHVDataRowList_SQLite: public SHVDataRowList
{
public:
	SHVDataRowList_SQLite(SHVDataSession* dataSession, SHVDataRowListC *rowList);
	virtual const SHVDataRowC* GetCurrentRow() const;
	virtual SHVBool IsOk() const;
	virtual const SHVDataStructC* GetStruct() const;
	virtual const SHVString8C GetAlias() const;
	virtual int GetRowCount() const;

	virtual SHVDataRowC* Find(const SHVDataRowKey* key);
	virtual SHVBool NextRow();
	virtual SHVDataRowListC* Reverse(const SHVStringC& condition);
	virtual SHVBool Reset();

	virtual const void* GetRowProvider() const;
	virtual SHVDataSession* GetDataSession();

	virtual SHVDataRow* EditCurrentRow();
	virtual SHVDataRow* AddRow();
	virtual void EnableEvents(bool enable);
	virtual bool GetEventsEnabled() const;
	virtual void EnableNonAccepted(bool enable);
	virtual bool GetNonAcceptedEnabled() const;

	virtual SHVBool RowListValid() const;
protected:
	virtual ~SHVDataRowList_SQLite();
	virtual SHVBool AcceptChanges(SHVDataRow* row);
	virtual SHVBool RejectChanges(SHVDataRow* row);
	virtual void AdjustRowCount(int delta);
	virtual SHVBool TempReset();
	virtual void Reposition();
	virtual SHVBool InternalRowChanged(SHVDataRow* row);
	
	SHVDataRow* FindPending(const SHVDataRowKey* key);
	bool NextPendingAdded();
private:
	SHVDataRowListCRef RowList;
	SHVDataSessionRef DataSession;
	SHVDataRowCollection PendingRows;
	SHVListPos PendingPosAdded;
	int InsertedRows;
	bool EventsEnabled;
	bool NonAcceptedEnabled;
};
typedef SHVRefObjectContainer<SHVDataRowList_SQLite> SHVDataRowList_SQLiteRef;



#endif