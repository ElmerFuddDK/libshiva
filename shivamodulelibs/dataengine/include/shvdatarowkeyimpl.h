#ifndef __SHIVA_DATAENGINE_DATAROWKEY_IMPL_H
#define __SHIVA_DATAENGINE_DATAROWKEY_IMPL_H


#include "../../../../include/utils/shvvector.h"
#include "../shvdatarowkey.h"

//-=========================================================================================================
/// SHVDataRowKey class - Interface for the a key
/**
 * Interface for a generic shiva datarow
 */
class SHVDataRowKey_impl: public SHVDataRowKey
{
protected:
	~SHVDataRowKey_impl();
public:
	SHVDataRowKey_impl();
	SHVDataRowKey_impl(const SHVDataRowKey* key);
	virtual const SHVDataRowKey::KeyValuePair& operator[](size_t idx) const;

	virtual void AddKey(const SHVString8C& columnName, SHVDataVariant* value, SHVBool desc);

	virtual size_t Count() const;
	virtual SHVBool Unique() const;
	virtual void SetUnique(SHVBool flag);

	virtual SHVBool KeyEquals(const SHVDataRowKey* key) const;
	virtual SHVBool KeyDefEquals(const SHVDataRowKey* key) const;
private:
	SHVVector<KeyValuePair> Keys;
	SHVBool UniqueKey;
};
typedef SHVRefObjectContainer<SHVDataRowKey_impl> SHVDataRowKey_implRef;

#endif