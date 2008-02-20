#ifndef __SHIVA_DATAENGINE_DATAROWKEY_H
#define __SHIVA_DATAENGINE_DATAROWKEY_H

#include "../../../include/utils/shvrefobject.h"
#include "../../../include/utils/shvstring.h"
#include "../../../include/utils/shvvector.h"
#include "../../../include/shvtypes.h"
#include "shvdatavariant.h"

//-=========================================================================================================
/// SHVDataRowKey class - Interface for the a key
/**
 * Interface for a generic shiva datarow
 */
class SHVDataRowKey: public SHVRefObject
{
public:
	struct KeyValuePair
	{
		SHVString8      Key;
		SHVDataVariant* Value;
		SHVBool         Desc;
	};

protected:
	virtual ~SHVDataRowKey() {}
public:
	virtual const SHVDataRowKey::KeyValuePair& operator[](size_t idx) const = 0;
	virtual size_t Count() const = 0;
	virtual SHVBool Unique() const = 0;
	virtual SHVBool KeyEquals(const SHVDataRowKey* key) const = 0;
	virtual SHVBool KeyDefEquals(const SHVDataRowKey* key) const = 0;
};
typedef SHVRefObjectContainer<SHVDataRowKey> SHVDataRowKeyRef;

#endif