#ifndef __SHIVA_DATAFACTORY_H
#define __SHIVA_DATAFACTORY_H

#include "shvdatasession.h"
#include "shvdatastruct.h"

class SHVDataFactory: public SHVRefObject
{
public:
	virtual SHVBool RegisterTable(const SHVDataStructC* dataStruct) = 0;
	virtual const SHVDataStructC* FindStruct(const SHVString8C& table) const = 0;
	virtual const SHVDataSchema& GetDataSchema() const = 0;
	virtual SHVDataSession* CreateSession() = 0;
	virtual SHVDataStruct* CreateStruct() const = 0;
	virtual SHVDataVariant* CreateVariant() const = 0;
	virtual SHVDataRowKey* CreateKey() const = 0;
	virtual const SHVStringC& GetDatabase() const = 0;
	virtual void BuildKeySQL(const SHVDataRowKey* key, SHVString8& condition, SHVString8& orderby, bool reverse = false) const = 0;
protected:
	virtual ~SHVDataFactory() {}
};
typedef SHVRefObjectContainer<SHVDataFactory> SHVDataFactoryRef;

#endif