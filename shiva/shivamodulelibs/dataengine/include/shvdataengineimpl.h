#ifndef __SHIVA_DATAENGINE_IMPL_H
#define __SHIVA_DATAENGINE_IMPL_H

#include "../../../include/modules/dataengine/shvdataengine.h"
#include "../../../include/utils/shvdll.h"
#include "../../../include/utils/shvvectorref.h"
#include "../../../include/sqlite/sqlitewrapper.h"

//-=========================================================================================================
/// SHVDataEngineImpl class - Implementation for the DataEngine module
/**
 * Is a factory for DataFactory objects, which enables multible connections to one or more
 * SQLite databases (files)
 */
class SHVDataEngineImpl: public SHVDataEngine
{
public:
	SHVDataEngineImpl(SHVModuleList& modules);
	virtual ~SHVDataEngineImpl();

	// from SHVModule
	virtual SHVBool Register();
	virtual void Unregister();

	// from SHVDataEngine
	virtual SHVBool RegisterTable(const SHVDataStructC* dataStruct, SHVDataSession* useSession = NULL);
	virtual SHVBool RegisterAlias(const SHVString8C& table, const SHVString8C& alias, bool clear = false, SHVDataSession* useSession = NULL);
	virtual SHVBool DropAlias(const SHVString8C& table, const SHVString8C& alias, SHVDataSession* useSession = NULL);
	virtual size_t RegisterIndex(const SHVString8C& table, SHVDataRowKey* IndexKey, SHVDataSession* useSession = NULL);
	virtual SHVBool UnregisterAlias(const SHVString8C& alias, SHVDataSession* useSession = NULL);
	virtual SHVBool ClearTable(const SHVString8C& table, SHVDataSession* useSession = NULL);

	virtual const SHVDataStructC* FindStruct(const SHVString8C& table) const;
	virtual const SHVDataSchema& GetDataSchema() const;
	virtual const SHVStringC& GetDatabase() const;

	virtual SHVDataSession* CreateSession();
	virtual SHVDataStruct* CreateStruct() const;
	virtual SHVDataStruct* CopyStruct(const SHVDataStructC* struc);
	virtual SHVDataVariant* CreateVariant() const;
	virtual SHVDataRowKey* CreateKey() const;
	virtual SHVDataRowKey* CopyKey(const SHVDataRowKey* key) const;
	virtual SHVDataStructC* RetrieveStruct(const SHVString8C table, SHVDataSession* useSession) const;
	virtual void BuildKeySQL(const SHVDataRowKey* key, SHVString8& condition, SHVString8& orderby, const SHVString8C& table, bool reverse = false) const;

	virtual SHVSQLiteWrapper* CreateConnection(SHVBool& Ok, const SHVStringC& dataBase);

	virtual SHVDataFactory* CreateFactory(const SHVStringC& database);
	virtual SHVDataFactory* GetDefaultFactory();


protected:
	// from SHVDataEngine
	virtual void SubscribeRowChange(SHVEventSubscriberBase* sub);
	virtual void RowChanged(SHVDataRow* row);


private:
	SHVDll SQLiteDll;
	SHVDataFactoryRef Factory;
};


#endif
