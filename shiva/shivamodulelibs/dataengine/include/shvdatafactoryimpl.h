#ifndef __SHIVA_DATAFACTORY_IMPL_H
#define __SHIVA_DATAFACTORY_IMPL_H

#include "../../../include/modules/dataengine/shvdatafactory.h"
#include "../../../include/modules/dataengine/shvdataengine.h"
#include "../../../include/modules/dataengine/shvdatarow.h"

#include "../../../include/utils/shvlist.h"
#include "../../../include/utils/shvhashtable.h"
#include "../../../include/threadutils/shvmutex.h"
#include "../../../include/threadutils/shvreadwritelock.h"
#include "../../../include/sqlite/sqlitewrapper.h"

//-=========================================================================================================
///  SHVDataStructReg class - Registration class for data structures
/**
 */
class SHVDataStructReg
{
public:
	inline SHVDataStructReg(const SHVString8C& name, int aliasID, const SHVDataStructC* datastruct);
	inline const SHVDataStructC& GetStruct() const;
	inline const SHVString8C& GetAlias() const;
	inline int GetAliasID() const;
	inline bool GetDeleted() const;
	inline void SetDeleted(bool val);
	inline bool GetTemp() const;
	inline void SetTemp(bool val);
private:
	SHVDataStructCRef Struct;
	SHVString8 Alias;
	int AliasId;
	bool Deleted;
	bool Temp;
};
typedef SHVList<SHVDataStructReg> SHVDataSchemaAlias;

//-=========================================================================================================
/// SHVDataFactoryImpl class - Implementation of the data factory
/**
 * Is the factory for the data engine objects. 
 */
class SHVDataFactoryImpl: public SHVDataFactory
{
public:
	SHVDataFactoryImpl(SHVDataEngine& engine, const SHVStringC& database);

	// from SHVDataFactory
	virtual SHVBool RegisterTable(const SHVDataStructC* dataStruct, SHVDataSession* useSession = NULL);
	virtual SHVBool RegisterAlias(const SHVString8C& table, const SHVString8C& alias, bool clear = false, SHVDataSession* useSession = NULL);
	virtual size_t RegisterIndex(const SHVString8C& table, SHVDataRowKey* IndexKey, SHVDataSession* useSession = NULL);
	virtual SHVBool UnregisterAlias(const SHVString8C& alias, SHVDataSession* useSession = NULL);
	virtual SHVBool DropAlias(const SHVString8C& table, const SHVString8C& alias, SHVDataSession* useSession = NULL);
	virtual SHVBool ClearTable(const SHVString8C& table, SHVDataSession* useSession = NULL);

	virtual const SHVDataStructC* FindStruct(const SHVString8C& table) const;
	virtual const SHVDataSchema& GetDataSchema() const;

	virtual SHVDataSession* CreateSession();
	virtual SHVDataStruct* CreateStruct() const;
	virtual SHVDataStruct* CopyStruct(const SHVDataStructC* struc);
	virtual SHVDataVariant* CreateVariant() const;
	virtual SHVDataRowKey* CreateKey() const;
	virtual SHVDataRowKey* CopyKey(const SHVDataRowKey* key) const;
	virtual SHVDataStructC* RetrieveStruct(const SHVString8C table, SHVDataSession* useSession = NULL) const;

	virtual const SHVStringC& GetDatabase() const;
	virtual void BuildKeySQL(const SHVDataRowKey* key, SHVString8& condition, SHVString8& orderby, const SHVString8C& table, bool reverse = false) const;

	virtual void SubscribeRowChange(SHVEventSubscriberBase* sub);

	virtual SHVDataEngine& GetDataEngine();

	virtual SHVBool IsOk() const;

	virtual void LockShared();
	virtual void LockExclusive();
	virtual SHVBool TryLockExclusive();
	virtual void Unlock();

protected:
	virtual ~SHVDataFactoryImpl();
	virtual SHVBool CreateTable(SHVSQLiteWrapper* sqlite, const SHVDataStructC* dataStruct);
	virtual SHVBool CreateView(SHVSQLiteWrapper* sqlite, const SHVDataStructC* dataStruct, const SHVString8C& viewName, int& id);
	virtual SHVBool CreateIndex(SHVSQLiteWrapper* sqlite, const SHVDataStructC* dataStruct, size_t index);
	virtual int GetAliasID(SHVSQLiteWrapper* sqlite, const SHVString8C& alias, bool create);
	virtual bool TableMatch(SHVSQLiteWrapper* sqlite, const SHVDataStructC* dataStruct, const SHVString8C& tableName, bool& exists);
	virtual SHVDataStructReg* InternalFindAlias(const SHVString8C& table) const;
	virtual size_t InternalFindStruct(const SHVString8C& table) const;
	virtual void SchemaChanged(SHVSQLiteWrapper* sqlite);
	virtual SHVBool InternalDropAlias(const SHVString8C& table, const SHVString8C& alias, SHVDataStructReg* aliasfound, SHVDataSession* useSession);

	// from SHVDataFactory
	virtual void RegisterDataSession(SHVDataSession* session);
	virtual void UnregisterDataSession(SHVDataSession* session);
	virtual void RowChanged(SHVDataRow* row);
	virtual SHVBool BeginTransaction(SHVDataSession* session, bool wait = false);
	virtual SHVBool EndTransaction(SHVDataSession* session);
	virtual SHVBool RollbackTransaction(SHVDataSession* session);
	virtual bool GetInTransaction() const;
	virtual SHVBool InternalBeginTransaction(SHVSQLiteWrapper* sqlite);
	virtual SHVBool InternalEndTransaction(SHVSQLiteWrapper* sqlite);
	virtual SHVBool InternalRollbackTransaction(SHVSQLiteWrapper* sqlite);
	virtual int GetAliasID(const SHVDataSession* dataSession, const SHVString8C& alias) const;

private:
// friends
friend class SHVDataEngineImpl;

	SHVDataEngine& DataEngine;
	SHVString Database;
	SHVDataSchema Schema;
	SHVDataSchemaAlias Alias;
	SHVList<SHVDataSession*> ActiveSessions;
	SHVEventSubscriberBaseRef DataChangedSubscription;
	SHVReadWriteLock Lock;
	SHVReadWriteLock SessionLock;
	int InTransaction;
	SHVBool Ok;
	bool SchemaHasChanged;
#ifdef DEBUG
	SHVMutex SharedLocksMutex;
	SHVHashTable<SHVThreadBase::ThreadID, int> SharedLocks;
	SHVThreadBase::ThreadID OwnerThread;
#endif
};

// ============================================ implementation ============================================ //

/*************************************
 * Constructors
 *************************************/
SHVDataStructReg::SHVDataStructReg(const SHVString8C& name, int aliasID, const SHVDataStructC* datastruct): Struct((SHVDataStructC*) datastruct), Alias(name), AliasId(aliasID), Deleted(false)
{
}

/*************************************
 * GetStruct
 *************************************/
const SHVDataStructC& SHVDataStructReg::GetStruct() const
{
	return *Struct.AsConst();
}

/*************************************
 * GetAlias
 *************************************/
const SHVString8C& SHVDataStructReg::GetAlias() const
{
	return Alias;
}

/*************************************
 * GetAliasID
 *************************************/
int SHVDataStructReg::GetAliasID() const
{
	return AliasId;
}

/*************************************
 * GetDeleted
 *************************************/
bool SHVDataStructReg::GetDeleted() const
{
	return Deleted;
}

/*************************************
 * SetDeleted
 *************************************/
void SHVDataStructReg::SetDeleted(bool val)
{
	Deleted = val;
}
/*************************************
 * GetTemp
 *************************************/
bool SHVDataStructReg::GetTemp() const
{
	return Temp;
}

/*************************************
 * SetTemp
 *************************************/
void SHVDataStructReg::SetTemp(bool val)
{
	Temp = val;
}

#endif
