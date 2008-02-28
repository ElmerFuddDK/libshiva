#ifndef __SHIVA_DATAENGINE_H
#define __SHIVA_DATAENGINE_H

#include "../../../shivasqlite/include/sqlitewrapper.h"
#include "../../../include/framework/shvmodule.h"
#include "../../../include/utils/shvdll.h"
#include "shvdatafactory.h"
#define __DATAENGINE_VERSION_MAJOR     0
#define __DATAENGINE_VERSION_MINOR     1
#define __DATAENGINE_VERSION_RELEASE   0
#define __DATAENGINE_DEFAULT_DATABASE _T("DefaultDatabase")
#define __DATAENGINE_DATAPATH         _T("DataPath")
//-=========================================================================================================
/// SHVDataEngine class - Interface for the shiva data engine module
/**
 * Is a factory for DataFactory objects, which enables multible connections to one or more
 * SQLite databases (files)
 */
class SHVDataEngine: public SHVModule, public SHVDataFactory
{
public:
	virtual ~SHVDataEngine() {}
	virtual SHVBool RegisterTable(const SHVDataStructC* dataStruct) = 0;
	virtual const SHVDataStructC* FindStruct(const SHVString8C& table) const = 0;
	virtual const SHVDataSchema& GetDataSchema() const = 0;
	virtual const SHVStringC& GetDatabase() const = 0;

	virtual SHVDataSession* CreateSession() = 0;
	virtual SHVDataStruct* CreateStruct() const = 0;
	virtual SHVDataVariant* CreateVariant() const = 0;
	virtual SHVDataRowKey* CreateKey() const = 0;
	virtual void BuildKeySQL(const SHVDataRowKey* key, SHVString8& condition, SHVString8& orderby, bool reverse = false) const = 0;

	virtual SHVSQLiteWrapper* CreateConnection(SHVBool& Ok, const SHVStringC& dataBase) = 0;

	virtual SHVDataFactory* CreateFactory(const SHVString& database, const SHVDataSchema* schema = NULL) = 0;
protected:
	inline SHVDataEngine(SHVModuleList& modules): SHVModule(modules, "DataEngine") { }
};


#endif