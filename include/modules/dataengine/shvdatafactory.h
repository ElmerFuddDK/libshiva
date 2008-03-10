#ifndef __SHIVA_DATAFACTORY_H
#define __SHIVA_DATAFACTORY_H

#include "../../../include/framework/shveventdata.h"
#include "../../../include/utils/shvstring.h"
#include "shvdatastructc.h"

// forward declare
class SHVDataSession;
class SHVDataStruct;
class SHVDataVariant;
class SHVDataRow;
class SHVDataRowListC;
class SHVDataEngine;

//-=========================================================================================================
/// SHVDataFactory class - Interface for the shiva data factory
/**
 * Is the data object factory used to instanciate data engine objects
 */
class SHVDataFactory: public SHVRefObject
{
public:
	enum EventTypes
	{
		EventRowChanged
	};
	virtual SHVBool RegisterTable(const SHVDataStructC* dataStruct) = 0;
	virtual const SHVDataStructC* FindStruct(const SHVString8C& table) const = 0;
	virtual const SHVDataSchema& GetDataSchema() const = 0;
	virtual SHVDataSession* CreateSession() = 0;
	virtual SHVDataStruct* CreateStruct() const = 0;
	virtual SHVDataVariant* CreateVariant() const = 0;
	virtual SHVDataRowKey* CreateKey() const = 0;
	virtual SHVDataRowKey* CopyKey(const SHVDataRowKey* key) const = 0;
	virtual const SHVStringC& GetDatabase() const = 0;
	virtual void BuildKeySQL(const SHVDataRowKey* key, SHVString8& condition, SHVString8& orderby, const SHVString8C& table, bool reverse = false) const = 0;
	virtual void SubscribeRowChange(SHVEventSubscriberBase* sub) = 0;
	virtual SHVDataEngine& GetDataEngine() = 0;
	virtual SHVStringBuffer GetErrorMessage() const = 0;
	virtual SHVBool IsOk() const = 0;
protected:
	friend class SHVDataSession;
	friend class SHVDataEngine;
	virtual void RegisterDataList(SHVDataRowListC* rowList) = 0;
	virtual void UnregisterDataList(SHVDataRowListC* rowList) = 0;
	virtual void RegisterDataSession(SHVDataSession* session) = 0;
	virtual void UnregisterDataSession(SHVDataSession* session) = 0;
	virtual void RowChanged(SHVDataRow* row) = 0;
	virtual SHVBool SessionReset(SHVDataSession* session) = 0;
	virtual void SessionReposition(SHVDataSession* session) = 0;
	inline void ReleaseDataSession(SHVDataSession* session);
	inline SHVBool DataListTempReset(SHVDataRowListC* dataList);
	inline void DataListReposition(SHVDataRowListC* dataList);
	virtual ~SHVDataFactory() {}
};
typedef SHVRefObjectContainer<SHVDataFactory> SHVDataFactoryRef;

#endif

// =========================================== implementation ============================================ //

#ifndef __SHIVA_DATAFACTORY_INL
#define __SHIVA_DATAFACTORY_INL

#include "shvdatasession.h"
#include "shvdatarowlistc.h"

/*************************************
 * ReleaseDataSession
 *************************************/
void SHVDataFactory::ReleaseDataSession(SHVDataSession* session)
{
	session->ClearOwnership();
}

/*************************************
 * DataListTempReset
 *************************************/
SHVBool SHVDataFactory::DataListTempReset(SHVDataRowListC* dataList)
{
	return dataList->TempReset();
}

/*************************************
 * DataListReposition
 *************************************/
void SHVDataFactory::DataListReposition(SHVDataRowListC* dataList)
{
	dataList->Reposition();
}
#endif

