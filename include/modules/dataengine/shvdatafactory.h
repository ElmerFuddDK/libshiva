#ifndef __SHIVA_DATAFACTORY_H
#define __SHIVA_DATAFACTORY_H

#include "../../framework/shveventdata.h"
#include "../../utils/shvstring.h"
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


	virtual SHVBool RegisterTable(const SHVDataStructC* dataStruct, bool createTable = false) = 0;
	virtual SHVBool RegisterAlias(const SHVString8C& table, const SHVString8C& alias, bool clear = false, SHVDataSession* useSession = NULL) = 0;
	virtual SHVBool UnregisterAlias(const SHVString8C& alias, SHVDataSession* useSession = NULL) = 0;

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
// friends
friend class SHVDataSession;
friend class SHVDataEngine;

	virtual ~SHVDataFactory() {}
	virtual void RegisterDataSession(SHVDataSession* session) = 0;
	virtual void UnregisterDataSession(SHVDataSession* session) = 0;
	virtual bool CheckAlias(SHVDataSession* session, const SHVString8C& alias) = 0;
	virtual void RowChanged(SHVDataRow* row) = 0;
	virtual bool LockTransaction() = 0;
	virtual void UnlockTransaction() = 0;
	virtual SHVBool BeginTransaction(SHVDataSession* session) = 0;
	virtual SHVBool EndTransaction(SHVDataSession* session) = 0;
	virtual SHVBool RollbackTransaction(SHVDataSession* session) = 0;

	// inlines
	inline void ReleaseDataSession(SHVDataSession* session);
};
typedef SHVRefObjectContainer<SHVDataFactory> SHVDataFactoryRef;

#endif

// =========================================== implementation ============================================ //

#ifndef __SHIVA_DATAFACTORY_INL
#define __SHIVA_DATAFACTORY_INL

#include "shvdatasession.h"

/*************************************
 * ReleaseDataSession
 *************************************/
void SHVDataFactory::ReleaseDataSession(SHVDataSession* session)
{
	session->ClearOwnership();
}
#endif

