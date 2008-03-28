#ifndef __SHIVA_DATASESSION_H
#define __SHIVA_DATASESSION_H

#include "../../utils/shvrefobject.h"
#include "../../framework/shveventsubscriber.h"
#include "../../framework/shveventdata.h"

// forward declare
class SHVDataRowList;
class SHVDataRowListC;
class SHVDataRow;
class SHVDataRowKey;
class SHVDataFactory;

class SHVDataSession: public SHVRefObject
{
public:
	// constants
	enum EventTypes
	{
		EventChangeSet
	};


	virtual SHVBool StartEdit() = 0;
	virtual SHVBool Commit() = 0;
	virtual SHVBool Rollback() = 0;

	virtual SHVDataRowList* GetRows(const SHVString8C& tableName, const SHVStringC& condition, size_t index) = 0;
	virtual SHVDataRowList* GetRowsIndexed(const SHVString8C& tableName, const SHVStringC& condition, size_t index) = 0;
	virtual SHVDataRowListC* Query(const SHVStringC& query, const SHVDataRowKey* sortKey = NULL) = 0;
	virtual SHVDataRowListC* QueryTable(const SHVString8C& tableName, const SHVStringC& condition, size_t index) = 0;
	virtual SHVDataRowListC* QueryTableIndexed(const SHVString8C& tableName, const SHVStringC& condition, size_t index) = 0;
	virtual SHVBool ExecuteNonQuery(const SHVStringC& sql) = 0;

	virtual SHVBool IsEditting() const = 0;

	virtual void SubscribeDataChange(SHVEventSubscriberBase* sub) = 0;
	virtual bool AliasActive(const SHVString8C& alias) = 0;
	virtual void* GetProvider() = 0;
	virtual SHVDataFactory* GetFactory() const = 0;

	// inlines
	inline SHVBool SessionValid() const;


protected:
// friends
friend class SHVDataRowListC;
friend class SHVDataRowList;
friend class SHVDataFactory;

	virtual ~SHVDataSession() {}
	virtual void ClearOwnership() = 0;
	virtual SHVBool UpdateRow(SHVDataRow* row) = 0;
	virtual SHVBool IsValid() const = 0;
	virtual bool SchemaChanged() = 0;
	virtual void RegisterDataList(SHVDataRowListC* rowList) = 0;
	virtual void UnregisterDataList(SHVDataRowListC* rowList) = 0;

	// inlines
	inline SHVBool SessionReset();
	inline void SessionReposition();
	inline void UnregisterDataSession();
	inline void RowChanged(SHVDataRow* row);
	inline SHVBool DataListTempReset(SHVDataRowListC* dataList);
	inline void DataListReposition(SHVDataRowListC* dataList);
	inline bool CheckAlias(const SHVString8C& alias);
};
typedef SHVRefObjectContainer<SHVDataSession> SHVDataSessionRef;

#endif

// =========================================== implementation ============================================ //
#ifndef __SHIVA_DATASESSION_INL
#define __SHIVA_DATASESSION_INL

#include "shvdatafactory.h"
#include "shvdatarowlistc.h"

/*************************************
 * SessionValid
 *************************************/
SHVBool SHVDataSession::SessionValid() const
{
	return GetFactory() != NULL && IsValid();
}

/*************************************
 * UnregisterDataSession
 *************************************/
void SHVDataSession::UnregisterDataSession()
{
	if (GetFactory())
		GetFactory()->UnregisterDataSession(this);
}

/*************************************
 * RowChanged
 *************************************/
void SHVDataSession::RowChanged(SHVDataRow* row)
{
	GetFactory()->RowChanged(row); 
}

/*************************************
 * DataListTempReset
 *************************************/
SHVBool SHVDataSession::DataListTempReset(SHVDataRowListC* dataList)
{
	return dataList->TempReset();
}

/*************************************
 * DataListReposition
 *************************************/
void SHVDataSession::DataListReposition(SHVDataRowListC* dataList)
{
	dataList->Reposition();
}

/*************************************
 * CheckAlias
 *************************************/
bool SHVDataSession::CheckAlias(const SHVString8C& alias)
{
	if (GetFactory())
		return GetFactory()->CheckAlias(this, alias);
	else
		return false;
}
#endif