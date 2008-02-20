#include "stdafx.h"
#include "../../../../include/platformspc.h"
#include "../../include/dataengineimpl/shvdatarowkey_impl.h"
#include "../../include/dataengineimpl/shvdatavariant_impl.h"


SHVDataRowKey_impl::~SHVDataRowKey_impl()
{
	for (size_t i = 0; i < Keys.CalculateCount(); i++)
	{
		if (Keys[i]->Value)
		{
			delete Keys[i]->Value;
			Keys[i]->Value = NULL;
		}
	}
	Keys.Clear();
}

SHVDataRowKey_impl::SHVDataRowKey_impl()
{
}

SHVDataRowKey_impl::SHVDataRowKey_impl(const SHVDataRowKey* key)
{
	for (size_t i = 0; i < key->Count(); i++)
	{
	KeyValuePair* k = new KeyValuePair();
		k->Key = (*key)[i].Key;
		if ((*key)[i].Value)
			k->Value = new SHVDataVariant_impl((*key)[i].Value);
		k->Desc = (*key)[i].Desc;
		Keys.Add(k);
	}
}

const SHVDataRowKey::KeyValuePair& SHVDataRowKey_impl::operator[](size_t idx) const
{
	return *Keys[idx];
}

void SHVDataRowKey_impl::AddKey(const SHVString8C& columnName, SHVDataVariant* value, SHVBool desc)
{
	KeyValuePair* k = new KeyValuePair();
	k->Key = columnName;
	k->Value = value;
	k->Desc = desc;
	Keys.Add(k);
}

size_t SHVDataRowKey_impl::Count() const
{
	return Keys.CalculateCount();
}

SHVBool SHVDataRowKey_impl::Unique() const
{
	return UniqueKey;
}

void SHVDataRowKey_impl::SetUnique(SHVBool flag)
{
	UniqueKey = flag;
}

SHVBool SHVDataRowKey_impl::KeyEquals(const SHVDataRowKey* key) const
{
SHVBool retVal = SHVBool::True;
const SHVDataRowKey_impl& This = *this;
const SHVDataRowKey& Key = *key;

	if (Count() == key->Count())
	{
		for (size_t i = 0; i < key->Count() && retVal; i++)
		{
			retVal =
				This[i].Key == Key[i].Key &&
				This[i].Desc == Key[i].Desc &&
				This[i].Value && Key[i].Value &&
				This[i].Value == Key[i].Value;
		}
	}
	return retVal;
}

SHVBool SHVDataRowKey_impl::KeyDefEquals(const SHVDataRowKey* key) const
{
SHVBool retVal = SHVBool::True;
const SHVDataRowKey_impl& This = *this;
const SHVDataRowKey& Key = *key;
	if (Count() == key->Count())
	{
		for (size_t i = 0; i < key->Count() && retVal; i++)
		{
			retVal =
				This[i].Key == Key[i].Key &&
				This[i].Desc == Key[i].Desc;
		}
	}
	return retVal;
}
