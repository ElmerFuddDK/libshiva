#ifndef __SHIVA_DATASESSION_H
#define __SHIVA_DATASESSION_H

#include "../../utils/shvrefobject.h"
#include "../../framework/shveventsubscriber.h"
#include "../../framework/shveventdata.h"
#include "../../threadutils/shvmutex.h"

// forward declare
class SHVDataRowList;
class SHVDataRowListC;
class SHVDataFunction;
class SHVDataStatement;
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
	virtual SHVDataFunction* PrepareFunction(const SHVStringC& query, const SHVDataRowKey* sortKey = NULL) = 0;
	virtual SHVDataFunction* GetDataChangeFunction(SHVDataRowList* rowList, int function) = 0;
	virtual SHVDataStatement* PrepareStatement(const SHVStringC& query) = 0;
	virtual SHVDataRowListC* QueryTable(const SHVString8C& tableName, const SHVStringC& condition, size_t index) = 0;
	virtual SHVDataRowListC* QueryTableIndexed(const SHVString8C& tableName, const SHVStringC& condition, size_t index) = 0;
	virtual SHVDataRowList* CopyAlias(const SHVString8C& sourceAlias, const SHVString8C& destAlias) = 0;
	virtual SHVBool ExecuteNonQuery(const SHVStringC& sql) = 0;
	virtual int GetRecentChanges() = 0;
	virtual int GetTotalChanges() = 0;

	inline SHVBool IsEditting() const;

	virtual void SubscribeDataChange(SHVEventSubscriberBase* sub) = 0;
	virtual void* GetProvider(bool checkConnection = true) = 0;
	virtual SHVDataFactory* GetFactory() const = 0;
	virtual SHVStringBuffer GetErrorMessage() const = 0;

	// inlines
	inline SHVBool SessionValid() const;


protected:
// friends
friend class SHVDataRowListC;
friend class SHVDataRowList;
friend class SHVDataFactory;

	virtual ~SHVDataSession() {}
	virtual SHVBool UpdateRow(SHVDataRow* row) = 0;
	virtual SHVBool IsValid() const = 0;
	virtual void SchemaChanged() = 0;

	// inlines
	inline void RowChanged(SHVDataRow* row);
	inline void UnregisterDataSession();
	inline SHVBool InternalBeginTransaction();
	inline SHVBool InternalEndTransaction();
	inline SHVBool InternalRollbackTransaction();
	inline int GetAliasID(SHVDataRowListC* rowList) const;
	inline int GetAliasID(const SHVString8C alias) const;
};
typedef SHVRefObjectContainer<SHVDataSession> SHVDataSessionRef;
#endif

// =========================================== implementation ============================================ //

#ifndef __SHIVA_DATASESSION_INL
#define __SHIVA_DATASESSION_INL

#include "shvdatafactory.h"
#include "shvdatarowlistc.h"
#include "shvdatastatement.h"

/*************************************
 * IsEditting
 *************************************/
SHVBool SHVDataSession::IsEditting() const
{
	return GetFactory() != NULL && GetFactory()->GetInTransaction();
}

/*************************************
 * SessionValid
 *************************************/
SHVBool SHVDataSession::SessionValid() const
{
	return GetFactory() != NULL && IsValid();
}

/*************************************
 * RowChanged
 *************************************/
void SHVDataSession::RowChanged(SHVDataRow* row)
{
	GetFactory()->RowChanged(row); 
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
 * InternalStartTransaction
 *************************************/
inline SHVBool SHVDataSession::InternalBeginTransaction()
{
SHVBool retVal = SHVBool::False;
	if (GetFactory())
		retVal = GetFactory()->BeginTransaction(this, true);
	return retVal;
}

/*************************************
 * InternalEndTransaction
 *************************************/
inline SHVBool SHVDataSession::InternalEndTransaction()
{
SHVBool retVal = SHVBool::False;
	if (GetFactory())
		retVal = GetFactory()->EndTransaction(this);
	return retVal;
}

/*************************************
 * InternalRollbackTransaction
 *************************************/
inline SHVBool SHVDataSession::InternalRollbackTransaction()
{
SHVBool retVal = SHVBool::False;
	if (GetFactory())
		retVal = GetFactory()->RollbackTransaction(this);
	return retVal;
}

/*************************************
 * GetAliasID
 *************************************/
int SHVDataSession::GetAliasID(SHVDataRowListC* rowList) const
{
	return rowList->GetAliasID();
}

int SHVDataSession::GetAliasID(const SHVString8C alias) const
{
int retVal = -1;
	if (GetFactory())
		retVal = GetFactory()->GetAliasID(this, alias);
	return retVal;
}
#endif
