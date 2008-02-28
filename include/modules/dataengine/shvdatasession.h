#ifndef __SHIVA_DATASESSION_H
#define __SHIVA_DATASESSION_H

#include "../../../include/framework/shveventsubscriber.h"
#include "../../../include/utils/shvrefobject.h"
#include "../../../include/framework/shveventdata.h"
#include "shvdatarowlist.h"

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
	virtual SHVDataRowListC* Query(const SHVStringC& query, const SHVDataRowKey* sortKey = NULL) = 0;
	virtual SHVDataRowListC* QueryTable(const SHVString8C& tableName, const SHVStringC& condition, size_t index) = 0;
	virtual SHVBool ExecuteNonQuery(const SHVStringC& sql) = 0;

	virtual SHVBool IsEditting() const = 0;
	virtual SHVBool UpdateRow(SHVDataRow* row) = 0;

	virtual void SubscribeDataChange(SHVEventSubscriberBase* sub) = 0;
	virtual void* GetProvider() = 0;
protected:
	~SHVDataSession() { }
};
typedef SHVRefObjectContainer<SHVDataSession> SHVDataSessionRef;
typedef SHVPtrContainer<SHVDataRowVector> SHVDataRowVectorPtr;
typedef SHVEventData<SHVDataRowVectorPtr> SHVEventDataChangeSet;


#endif