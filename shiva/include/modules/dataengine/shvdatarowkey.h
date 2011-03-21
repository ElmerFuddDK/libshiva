#ifndef __SHIVA_DATAENGINE_DATAROWKEY_H
#define __SHIVA_DATAENGINE_DATAROWKEY_H

#include "../../utils/shvrefobject.h"
#include "../../utils/shvstring.h"
#include "../../utils/shvvector.h"
#include "../../shvtypes.h"
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
		bool            Desc;
	};


	virtual const SHVDataRowKey::KeyValuePair& operator[](size_t idx) const = 0;
	virtual void SetKeyValue(size_t idx, SHVInt val) = 0;
	virtual void SetKeyValue(size_t idx, SHVInt64 val) = 0;
	virtual void SetKeyValue(size_t idx, SHVDouble val) = 0;
	virtual void SetKeyValue(size_t idx, SHVBool val) = 0;
	virtual void SetKeyValue(size_t idx, const SHVTime& val) = 0;
	virtual void SetKeyValue(size_t idx, const SHVStringC& val) = 0;
	virtual void SetKeyNull(size_t idx) = 0;

	virtual void AddKey(const SHVString8C& columnName, SHVDataVariant* value, bool desc) = 0;
	virtual SHVDataRowKey& AddKey(const SHVString8C& columnName, bool desc) = 0;

	virtual size_t Count() const = 0;
	virtual SHVBool Unique() const = 0;
	virtual void SetUnique(SHVBool flag) = 0;

	virtual SHVBool KeyEquals(const SHVDataRowKey* key) const = 0;
	virtual SHVBool KeyDefEquals(const SHVDataRowKey* key) const = 0;


protected:
	virtual ~SHVDataRowKey() {}
};
typedef SHVRefObjectContainer<SHVDataRowKey> SHVDataRowKeyRef;

#endif
