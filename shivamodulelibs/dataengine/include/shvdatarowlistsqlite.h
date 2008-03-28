#ifndef __SHIVA_DATAENGINE_DATAROWLIST_SQLITE_H
#define __SHIVA_DATAENGINE_DATAROWLIST_SQLITE_H

#include "../../../include/sqlite/sqlitewrapper.h"
#include "../../../include/utils/shvrefobject.h"
#include "../../../include/utils/shvstring.h"
#include "../../../include/utils/shvvectorref.h"
#include "../../../include/utils/shvlist.h"
#include "../../../include/shvtypes.h"
#include "../../../include/modules/dataengine/shvdatarowlist.h"
#include "../../../include/modules/dataengine/shvdatasession.h"
#include "../../../include/modules/dataengine/shvdatarow.h"

//-=========================================================================================================
/// SHVDataRowListSQLite class - SQLite implementation of a datarow list.
/**
 * Objects of this class handles all the necessary logic for maintaining a sqlite datatable.
 */
class SHVDataRowCSQLite;
class SHVDataRowListSQLite: public SHVDataRowList
{
public:
	SHVDataRowListSQLite(SHVDataSession* dataSession, SHVDataRowListC *rowList);
	
	// from SHVDataRowList
	virtual SHVDataRow* EditCurrentRow();
	virtual SHVDataRow* AddRow();

	virtual void EnableEvents(bool enable);
	virtual bool GetEventsEnabled() const;
	virtual void EnableNonAccepted(bool enable);
	virtual bool GetNonAcceptedEnabled() const;

	// from SHVDataRowListC
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
	virtual SHVBool RowListValid() const;


protected:
	virtual ~SHVDataRowListSQLite();
	SHVDataRow* FindPending(const SHVDataRowKey* key);
	bool NextPendingAdded();
	
	// from SHVDataRowList
	virtual SHVBool AcceptChanges(SHVDataRow* row);
	virtual SHVBool RejectChanges(SHVDataRow* row);

	// from SHVDataRowListC
	virtual void AdjustRowCount(int delta);
	virtual SHVBool TempReset();
	virtual void Reposition();
	virtual SHVBool InternalRowChanged(SHVDataRow* row);
	

private:
	SHVDataRowListCRef RowList;
	SHVDataSessionRef DataSession;
	SHVDataRowCollection PendingRows;
	SHVListPos PendingPosAdded;
	int InsertedRows;
	bool EventsEnabled;
	bool NonAcceptedEnabled;
};
typedef SHVRefObjectContainer<SHVDataRowListSQLite> SHVDataRowListSQLiteRef;



#endif