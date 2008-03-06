#ifndef __SHIVA_DATASESSION_H
#define __SHIVA_DATASESSION_H

#include "../../../include/framework/shveventsubscriber.h"
#include "../../../include/utils/shvrefobject.h"
#include "../../../include/framework/shveventdata.h"

// forward declare
class SHVDataRowList;
class SHVDataRowListC;
class SHVDataRow;
class SHVDataRowKey;
class SHVDataFactory;

class SHVDataSession: public SHVRefObject
{
public:
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
	inline SHVBool SessionValid() const;

	virtual void SubscribeDataChange(SHVEventSubscriberBase* sub) = 0;
	virtual void* GetProvider() = 0;
	virtual SHVDataFactory* GetFactory() const = 0;
protected:
	friend class SHVDataRowListC;
	friend class SHVDataRowList;
	friend class SHVDataFactory;
	virtual void ClearOwnership() = 0;
	virtual SHVBool UpdateRow(SHVDataRow* row) = 0;
	virtual SHVBool IsValid() const = 0;
	inline SHVBool SessionReset();
	inline void RegisterDataList(SHVDataRowListC* rowList);
	inline void UnregisterDataList(SHVDataRowListC* rowList);
	inline void UnregisterDataSession();
	inline void RowChanged(SHVDataRow* row);
	virtual ~SHVDataSession() {}
};
typedef SHVRefObjectContainer<SHVDataSession> SHVDataSessionRef;

#endif

// =========================================== implementation ============================================ //
#ifndef __SHIVA_DATASESSION_INL
#define __SHIVA_DATASESSION_INL

#include "shvdatafactory.h"

/*************************************
 * SessionReset
 *************************************/
SHVBool SHVDataSession::SessionReset()
{
	return GetFactory() != NULL && GetFactory()->SessionReset(this);
}

/*************************************
 * SessionValid
 *************************************/
SHVBool SHVDataSession::SessionValid() const
{
	return GetFactory() != NULL && IsValid();
}

/*************************************
 * RegisterDataList
 *************************************/
void SHVDataSession::RegisterDataList(SHVDataRowListC* rowList)
{ 
	GetFactory()->RegisterDataList(rowList);   
}

/*************************************
 * UnregisterDataList
 *************************************/
void SHVDataSession::UnregisterDataList(SHVDataRowListC* rowList) 
{ 
	GetFactory()->UnregisterDataList(rowList); 
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


#endif