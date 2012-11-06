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


	virtual SHVBool RegisterTable(const SHVDataStructC* dataStruct, SHVDataSession* useSession = NULL) = 0;
	virtual SHVBool RegisterAlias(const SHVString8C& table, const SHVString8C& alias, bool clear = false, SHVDataSession* useSession = NULL) = 0;
	virtual size_t RegisterIndex(const SHVString8C& table, SHVDataRowKey* IndexKey, SHVDataSession* useSession = NULL) = 0;
	virtual SHVBool UnregisterAlias(const SHVString8C& alias, SHVDataSession* useSession = NULL) = 0;
	virtual SHVBool DropAlias(const SHVString8C& table, const SHVString8C& alias, SHVDataSession* useSession = NULL) = 0;
	virtual SHVBool ClearTable(const SHVString8C& table, SHVDataSession* useSession = NULL) = 0;

	virtual const SHVDataStructC* FindStruct(const SHVString8C& table) const = 0;
	virtual const SHVDataSchema& GetDataSchema() const = 0;

	virtual SHVDataSession* CreateSession() = 0;
	virtual SHVDataStruct* CreateStruct() const = 0;
	virtual SHVDataStruct* CopyStruct(const SHVDataStructC* struc) = 0;
	virtual SHVDataVariant* CreateVariant() const = 0;
	virtual SHVDataRowKey* CreateKey() const = 0;
	virtual SHVDataRowKey* CopyKey(const SHVDataRowKey* key) const = 0;
	virtual SHVDataStructC* RetrieveStruct(const SHVString8C table, SHVDataSession* useSession = NULL) const = 0;

	virtual const SHVStringC& GetDatabase() const = 0;
	virtual void BuildKeySQL(const SHVDataRowKey* key, SHVString8& condition, SHVString8& orderby, const SHVString8C& table, bool reverse = false) const = 0;

	virtual void SubscribeRowChange(SHVEventSubscriberBase* sub) = 0;

	virtual SHVDataEngine& GetDataEngine() = 0;

	virtual SHVBool IsOk() const = 0;

	virtual void LockShared() = 0;
	virtual void LockExclusive() = 0;
	virtual SHVBool TryLockExclusive() = 0;
	virtual void Unlock() = 0;

protected:
// friends
friend class SHVDataSession;
friend class SHVDataEngine;

	virtual ~SHVDataFactory() {}
	virtual void RegisterDataSession(SHVDataSession* session) = 0;
	virtual void UnregisterDataSession(SHVDataSession* session) = 0;
	virtual void RowChanged(SHVDataRow* row) = 0;
	virtual SHVBool BeginTransaction(SHVDataSession* session, bool wait = false) = 0;
	virtual SHVBool EndTransaction(SHVDataSession* session) = 0;
	virtual SHVBool RollbackTransaction(SHVDataSession* session) = 0;
	virtual bool GetInTransaction() const = 0;
	virtual int GetAliasID(const SHVDataSession* dataSession,const SHVString8C& alias) const = 0;

	// inlines
	inline void SchemaChanged(SHVDataSession* session);
	inline SHVDataStruct* GetInternalStruct(SHVDataStructC* dataStruct);
	inline void BeginningTransaction(SHVDataEngine* engine);
	inline void FinishedTransaction(SHVDataEngine* engine, bool committed);
};
typedef SHVRefObjectContainer<SHVDataFactory> SHVDataFactoryRef;

//-=========================================================================================================
/// SHVDataFactoryExclusiveLocker class - Holds an exclusive on the database
/**
 * An exclusive lock can only be aquired if there is no other exclusive locks and
 * share locks. The locking mechanism if reference counted with in same thread. Thus
 * the same thread can aquire this lock n times, as long as it is released n times.
 */
class SHVDataFactoryExclusiveLocker
{
public:
	inline SHVDataFactoryExclusiveLocker(SHVDataFactory* factory);
	inline ~SHVDataFactoryExclusiveLocker();
private:
	SHVDataFactoryRef Factory;
};

#endif

// =========================================== implementation ============================================ //

#ifndef __SHIVA_DATAFACTORY_INL
#define __SHIVA_DATAFACTORY_INL

#include "shvdatasession.h"
#include "shvdataengine.h" // included here since data engine implements 2 datafactory inlines

/*************************************
 * SchemaChanged
 *************************************/
void SHVDataFactory::SchemaChanged(SHVDataSession* session)
{
	session->SchemaChanged();
}

/*************************************
 * GetInternalStruct
 *************************************/
SHVDataStruct* SHVDataFactory::GetInternalStruct(SHVDataStructC* dataStruct)
{
	return dataStruct->GetInternalStruct();
}

/*************************************
 * SHVDataFactoryExclusiveLocker
 *************************************/
SHVDataFactoryExclusiveLocker::SHVDataFactoryExclusiveLocker(SHVDataFactory* factory): Factory(factory)
{
	factory->LockExclusive();
}

SHVDataFactoryExclusiveLocker::~SHVDataFactoryExclusiveLocker()
{
	Factory->Unlock();
}

#endif

