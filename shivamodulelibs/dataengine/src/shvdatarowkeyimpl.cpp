#include "stdafx.h"
#include "../../../../include/platformspc.h"
#include "../../include/shvdatarowkeyimpl.h"
#include "../../include/shvdatavariantimpl.h"

static int alloccount = 0;
/*************************************
 * Constructor
 *************************************/
SHVDataRowKey_impl::SHVDataRowKey_impl()
{
}
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

SHVDataRowKey_impl::SHVDataRowKey_impl(const SHVDataRowKey* key)
{
	for (size_t i = 0; i < key->Count(); i++)
	{
	KeyValuePair* k = new KeyValuePair();
		k->Key = (*key)[i].Key;
		if ((*key)[i].Value)
			k->Value = new SHVDataVariant_impl((*key)[i].Value);
		else
			k->Value = NULL;
		k->Desc = (*key)[i].Desc;
		Keys.Add(k);
	}
}

/*************************************
 * operator []
 *************************************/
const SHVDataRowKey::KeyValuePair& SHVDataRowKey_impl::operator[](size_t idx) const
{
	return *Keys[idx];
}

/*************************************
 * SetKeyValue
 *************************************/
void SHVDataRowKey_impl::SetKeyValue(size_t idx, SHVInt val)
{
	if (!Keys[idx]->Value)
		Keys[idx]->Value = new SHVDataVariant_impl(SHVInt(val));
	else
		*Keys[idx]->Value = val;
}

void SHVDataRowKey_impl::SetKeyValue(size_t idx, SHVDouble val)
{
	if (!Keys[idx]->Value)
		Keys[idx]->Value = new SHVDataVariant_impl(SHVDouble(val));
	else
		*Keys[idx]->Value = val;
}

void SHVDataRowKey_impl::SetKeyValue(size_t idx, SHVBool val)
{
	if (!Keys[idx]->Value)
		Keys[idx]->Value = new SHVDataVariant_impl(SHVBool(val));
	else
		*Keys[idx]->Value = val;
}

void SHVDataRowKey_impl::SetKeyValue(size_t idx, const SHVTime& val)
{
	if (!Keys[idx]->Value)
		Keys[idx]->Value = new SHVDataVariant_impl(val);
	else
		*Keys[idx]->Value = val;
}

void SHVDataRowKey_impl::SetKeyValue(size_t idx, const SHVStringC& val)
{
	if (!Keys[idx]->Value)
		Keys[idx]->Value = new SHVDataVariant_impl(val);
	else
		*Keys[idx]->Value = val;
}

void SHVDataRowKey_impl::SetKeyNull(size_t idx)
{
	if (Keys[idx]->Value)
		Keys[idx]->Value->SetNull();
}

/*************************************
 * AddKey
 *************************************/
void SHVDataRowKey_impl::AddKey(const SHVString8C& columnName, SHVDataVariant* value, bool desc)
{
	KeyValuePair* k = new KeyValuePair();
	k->Key = columnName;
	k->Value = value;
	k->Desc = desc;
	Keys.Add(k);
}

SHVDataRowKey& SHVDataRowKey_impl::AddKey(const SHVString8C& columnName, bool desc)
{
	KeyValuePair* k = new KeyValuePair();
	k->Key = columnName;
	k->Value = NULL;
	k->Desc = desc;
	Keys.Add(k);
	return *this;
}

/*************************************
 * Count
 *************************************/
size_t SHVDataRowKey_impl::Count() const
{
	return Keys.CalculateCount();
}

/*************************************
 * Unique
 *************************************/
SHVBool SHVDataRowKey_impl::Unique() const
{
	return UniqueKey;
}

/*************************************
 * SetUnique
 *************************************/
void SHVDataRowKey_impl::SetUnique(SHVBool flag)
{
	UniqueKey = flag;
}

/*************************************
 * KeyEquals
 *************************************/
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
				((This[i].Value && Key[i].Value) ||
				 (!This[i].Value && !Key[i].Value)) &&
				(!This[i].Value || *This[i].Value == *Key[i].Value);
		}
	}
	return retVal;
}

/*************************************
 * KeyDefEquals
 *************************************/
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
