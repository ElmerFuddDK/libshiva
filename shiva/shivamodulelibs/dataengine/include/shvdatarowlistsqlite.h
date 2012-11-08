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
#include "../../../include/modules/dataengine/shvdatafunction.h"
#include "shvdatarowchangecacheimpl.h"
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
	virtual SHVBool StartEdit();
	virtual SHVBool EndEdit();
	virtual SHVBool CancelEdit();

	virtual SHVDataRow* EditCurrentRow();
	virtual SHVDataRow* AddRow();
	virtual SHVBool DeleteRow(const SHVDataRowKey* key);

	virtual void EnableEvents(bool enable);
	virtual bool GetEventsEnabled() const;
	virtual void EnableNonAccepted(bool enable);
	virtual bool GetNonAcceptedEnabled() const;
	virtual void EnableCacheChanges(bool enable);
	virtual bool GetCacheChangesEnabled() const;
	virtual void EnableFullRowCache(bool enable);
	virtual bool GetFullRowCache();
	virtual void EnableReplaceIfDuplicate(bool enable);
	virtual bool GetReplaceIfDuplicate();
	virtual const SHVDataRowChangeCache* GetChangeCache() const;

	// from SHVDataRowListC
	virtual const SHVDataRowC* GetCurrentRow() const;
	virtual SHVBool IsOk() const;
	virtual const SHVDataStructC* GetStruct() const;
	virtual const SHVString8C GetAlias() const;
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
	virtual ~SHVDataRowListSQLite();
	SHVDataRow* FindPending(const SHVDataRowKey* key);
	bool NextPendingAdded();
	
	// from SHVDataRowList
	virtual SHVBool AcceptChanges(SHVDataRow* row);
	virtual SHVBool RejectChanges(SHVDataRow* row);
	virtual SHVDataFunction* GetDataChangeFunction(int func);
	virtual bool SetDataChangeFunction(SHVDataFunction* datafunc, int func);

	// from SHVDataRowListC
	virtual void AdjustRowCount(int delta);
	virtual SHVBool InternalRowChanged(SHVDataRow* row);
	virtual int GetAliasID() const;	

private:
	SHVDataRowListCRef RowList;
	SHVDataFunctionRef InsertFunction;
	SHVDataFunctionRef UpdateFunction;
	SHVDataFunctionRef DeleteFunction;
	SHVDataSessionRef DataSession;
	SHVDataRowCollection PendingRows;
	SHVListPos PendingPosAdded;
	int InsertedRows;
	bool EventsEnabled;
	bool NonAcceptedEnabled;
	bool FullRowCache;
	bool ReplaceIfDuplicate;
	SHVDataRowKeyRef SavePosition;
	SHVDataRowChangeCacheImplPtr ChangeCache;
};
typedef SHVRefObjectContainer<SHVDataRowListSQLite> SHVDataRowListSQLiteRef;



#endif
