#ifndef __SHIVA_DATAENGINE_IMPL_H
#define __SHIVA_DATAENGINE_IMPL_H

#include "../shvdataengine.h"
#include "../../../../include/utils/shvdll.h"
#include "../../../../include/utils/shvvectorref.h"
#include "../../../../shivasqlite/include/sqlitewrapper.h"

//-=========================================================================================================
/// SHVDataEngine_impl class - Implementation for the DataEngine module
/**
 * Is a factory for DataFactory objects, which enables multible connections to one or more
 * SQLite databases (files)
 */
class SHVDataEngine_impl: public SHVDataEngine
{
public:
	SHVDataEngine_impl(SHVModuleList& modules);
	virtual ~SHVDataEngine_impl();

	virtual SHVBool Register();
	virtual void Unregister();

	virtual SHVBool RegisterTable(const SHVDataStructC* dataStruct, bool createTable = false);
	virtual SHVBool RegisterAlias(const SHVString8C& table, const SHVString8C& alias, bool clear = false);
	virtual SHVBool UnregisterAlias(const SHVString8C& alias);
	virtual const SHVDataStructC* FindStruct(const SHVString8C& table) const;
	virtual const SHVDataSchema& GetDataSchema() const;
	virtual const SHVStringC& GetDatabase() const;

	virtual SHVDataSession* CreateSession();
	virtual SHVDataStruct* CreateStruct() const;
	virtual SHVDataVariant* CreateVariant() const;
	virtual SHVDataRowKey* CreateKey() const;
	virtual SHVDataRowKey* CopyKey(const SHVDataRowKey* key) const;
	virtual void BuildKeySQL(const SHVDataRowKey* key, SHVString8& condition, SHVString8& orderby, const SHVString8C& table, bool reverse = false) const;

	virtual SHVSQLiteWrapper* CreateConnection(SHVBool& Ok, const SHVStringC& dataBase);

	virtual SHVDataFactory* CreateFactory(const SHVString& database, const SHVDataSchema* schema = NULL);
	virtual SHVDataFactory* GetDefaultFactory();
protected:
	virtual void SubscribeRowChange(SHVEventSubscriberBase* sub);
	virtual void RegisterDataList(SHVDataRowListC* rowList);
	virtual void UnregisterDataList(SHVDataRowListC* rowList);
	virtual void RowChanged(SHVDataRow* row);
private:
	SHVDll SQLiteDll;
	SHVDataFactoryRef Factory;
};


#endif