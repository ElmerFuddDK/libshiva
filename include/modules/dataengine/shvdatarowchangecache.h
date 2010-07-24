#ifndef __SHIVA_DATAENGINE_CHANGE_CACHE_H
#define __SHIVA_DATAENGINE_CHANGE_CACHE_H

#include "../../shvtypes.h"
#include "../../utils/shvrefobject.h"
#include "shvdatarowlist.h"

class SHVDataRowChangeCache
{
public:
	virtual ~SHVDataRowChangeCache() { }
	virtual size_t GetRowCount() const = 0;
	virtual const SHVDataRowKey* GetKey(size_t rowNo) const = 0;
	virtual const SHVDataRowC* GetRow(size_t rowNo) const = 0;
	virtual int GetRowState(size_t rowNo) const = 0;
};

#endif
