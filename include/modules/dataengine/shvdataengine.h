#ifndef __SHIVA_DATAENGINE_H
#define __SHIVA_DATAENGINE_H

#include "../../sqlite/sqlitewrapper.h"
#include "../../framework/shvmodule.h"
#include "../../utils/shvdll.h"
#include "shvdatafactory.h"

#define __DATAENGINE_VERSION_MAJOR     0
#define __DATAENGINE_VERSION_MINOR     1
#define __DATAENGINE_VERSION_RELEASE   0
#define __DATAENGINE_DEFAULT_DATABASE _T("defaultdatabase")
#define __DATAENGINE_DATAPATH         _T("datapath")

//-=========================================================================================================
/// SHVDataEngine class - Interface for the shiva data engine module
/**
 * Is a factory for DataFactory objects, which enables multible connections to one or more
 * SQLite databases (files)
 */
class SHVDataEngine: public SHVModule
{
public:
	virtual ~SHVDataEngine() {}
	virtual SHVBool RegisterTable(const SHVDataStructC* dataStruct, SHVDataSession* useSession = NULL) = 0;
	virtual SHVBool RegisterAlias(const SHVString8C& table, const SHVString8C& alias, bool clear = false, SHVDataSession* useSession = NULL) = 0;
	virtual size_t RegisterIndex(const SHVString8C& table, SHVDataRowKey* IndexKey, SHVDataSession* useSession = NULL) = 0;
	virtual SHVBool UnregisterAlias(const SHVString8C& alias, SHVDataSession* useSession = NULL) = 0;
	virtual SHVBool ClearTable(const SHVString8C& table, SHVDataSession* useSession = NULL) = 0;

	virtual const SHVDataStructC* FindStruct(const SHVString8C& table) const = 0;
	virtual const SHVDataSchema& GetDataSchema() const = 0;
	virtual const SHVStringC& GetDatabase() const = 0;

	virtual SHVDataSession* CreateSession() = 0;
	virtual SHVDataStruct* CreateStruct() const = 0;
	virtual SHVDataVariant* CreateVariant() const = 0;
	virtual SHVDataRowKey* CreateKey() const = 0;
	virtual SHVDataRowKey* CopyKey(const SHVDataRowKey* key) const = 0;
	virtual SHVDataStructC* RetrieveStruct(const SHVString8C table, SHVDataSession* useSession = NULL) const = 0;
	virtual void BuildKeySQL(const SHVDataRowKey* key, SHVString8& condition, SHVString8& orderby, const SHVString8C& table, bool reverse = false) const = 0;

	virtual SHVSQLiteWrapper* CreateConnection(SHVBool& Ok, const SHVStringC& dataBase) = 0;

	virtual SHVDataFactory* CreateFactory(const SHVStringC& database) = 0;
	virtual SHVDataFactory* GetDefaultFactory() = 0;

	// inlines 
	inline void FactoryRowChanged(SHVDataRow* row);


protected:
	virtual void SubscribeRowChange(SHVEventSubscriberBase* sub) = 0;
	virtual void RegisterDataList(SHVDataRowListC* rowList) = 0;
	virtual void UnregisterDataList(SHVDataRowListC* rowList) = 0;
	virtual void RowChanged(SHVDataRow* row) = 0;

	// inlines
	inline SHVDataEngine(SHVModuleList& modules): SHVModule(modules, "DataEngine") { }
};
// ============================================ implementation ============================================ //
#ifndef __SHIVA_DATAENGINE_INL
#define __SHIVA_DATAENGINE_INL

/*************************************
 * FactoryRowChanged
 *************************************/
void SHVDataEngine::FactoryRowChanged(SHVDataRow* row)
{
	RowChanged(row);
}
#endif


#endif