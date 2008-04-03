#ifndef __SHIVA_DATAENGINE_DATAROWKEY_IMPL_H
#define __SHIVA_DATAENGINE_DATAROWKEY_IMPL_H


#include "../../../include/utils/shvvector.h"
#include "../../../include/modules/dataengine/shvdatarowkey.h"

//-=========================================================================================================
/// SHVDataRowKeyImpl class - Implementation for the data key
/**
 * A key is actually a list of column name and maybe associated with value
 */
class SHVDataRowKeyImpl: public SHVDataRowKey
{
public:
	SHVDataRowKeyImpl();
	SHVDataRowKeyImpl(const SHVDataRowKey* key);

	// from SHVDataRowKey
	virtual const SHVDataRowKey::KeyValuePair& operator[](size_t idx) const;
	virtual void SetKeyValue(size_t idx, SHVInt val);
	virtual void SetKeyValue(size_t idx, SHVDouble val);
	virtual void SetKeyValue(size_t idx, SHVBool val);
	virtual void SetKeyValue(size_t idx, const SHVTime& val);
	virtual void SetKeyValue(size_t idx, const SHVStringC& val);
	virtual void SetKeyNull(size_t idx);

	virtual void AddKey(const SHVString8C& columnName, SHVDataVariant* value, bool desc);
	virtual SHVDataRowKey& AddKey(const SHVString8C& columnName, bool desc);

	virtual size_t Count() const;
	virtual SHVBool Unique() const;
	virtual void SetUnique(SHVBool flag);

	virtual SHVBool KeyEquals(const SHVDataRowKey* key) const;
	virtual SHVBool KeyDefEquals(const SHVDataRowKey* key) const;


protected:
	~SHVDataRowKeyImpl();


private:
	SHVVector<KeyValuePair> Keys;
	SHVBool UniqueKey;
};
typedef SHVRefObjectContainer<SHVDataRowKeyImpl> SHVDataRowKeyImplRef;

#endif