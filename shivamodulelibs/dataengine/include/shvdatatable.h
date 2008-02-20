#ifndef __SHIVA_DATAENGINE_DATATABLE_H
#define __SHIVA_DATAENGINE_DATATABLE_H

#include "../../../include/utils/shvrefobject.h"
#include "../../../include/utils/shvstring.h"
#include "shvdatastructc.h"
#include "shvdatarowlist.h"

class SHVDataTable: public SHVRefObject
{
protected:
	virtual ~SHVDataTable() {}
public:
	virtual const SHVDataStructC* GetDataStruct() const = 0;
	virtual SHVDataRowList* Select(const SHVStringC& select) = 0;
	virtual SHVBool InMemory() const = 0;
};
typedef SHVRefObjectContainer<SHVDataTable> SHVDataTableRef;

#endif