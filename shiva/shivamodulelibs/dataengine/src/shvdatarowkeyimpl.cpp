/*
 *   Copyright (C) 2008 by Mogens Bak Nielsen, Lars Eriksen
 *
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 of the
 *   License, or (at your option) any later version with the following
 *   exeptions:
 *
 *   1) Static linking to the library does not constitute derivative work
 *      and does not require the author to provide source code for the
 *      application.
 *      Compiling applications with the source code directly linked in is
 *      Considered static linking as well.
 *
 *   2) You do not have to provide a copy of the license with programs
 *      that are linked against this code.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "stdafx.h"
#include "../../../include/platformspc.h"
#include "../include/shvdatarowkeyimpl.h"
#include "../include/shvdatavariantimpl.h"

/*************************************
 * Constructor
 *************************************/
SHVDataRowKeyImpl::SHVDataRowKeyImpl()
{
}

SHVDataRowKeyImpl::SHVDataRowKeyImpl(const SHVDataRowKey* key)
{
	for (size_t i = 0; i < key->Count(); i++)
	{
	KeyValuePair* k = new KeyValuePair();
		k->Key = (*key)[i].Key;
		if ((*key)[i].Value)
			k->Value = new SHVDataVariantImpl((*key)[i].Value);
		else
			k->Value = NULL;
		k->Desc = (*key)[i].Desc;
		Keys.Add(k);
	}
}

/*************************************
 * operator []
 *************************************/
const SHVDataRowKey::KeyValuePair& SHVDataRowKeyImpl::operator[](size_t idx) const
{
	return *Keys[idx];
}

/*************************************
 * SetKeyValue
 *************************************/
void SHVDataRowKeyImpl::SetKeyValue(size_t idx, SHVInt val)
{
	if (!Keys[idx]->Value)
		Keys[idx]->Value = new SHVDataVariantImpl(SHVInt(val));
	else
		*Keys[idx]->Value = val;
}

void SHVDataRowKeyImpl::SetKeyValue(size_t idx, SHVInt64 val)
{
	if (!Keys[idx]->Value)
		Keys[idx]->Value = new SHVDataVariantImpl(SHVInt64(val));
	else
		*Keys[idx]->Value = val;
}

void SHVDataRowKeyImpl::SetKeyValue(size_t idx, SHVDouble val)
{
	if (!Keys[idx]->Value)
		Keys[idx]->Value = new SHVDataVariantImpl(SHVDouble(val));
	else
		*Keys[idx]->Value = val;
}

void SHVDataRowKeyImpl::SetKeyValue(size_t idx, SHVBool val)
{
	if (!Keys[idx]->Value)
		Keys[idx]->Value = new SHVDataVariantImpl(SHVBool(val));
	else
		*Keys[idx]->Value = val;
}

void SHVDataRowKeyImpl::SetKeyValue(size_t idx, const SHVTime& val)
{
	if (!Keys[idx]->Value)
		Keys[idx]->Value = new SHVDataVariantImpl(val);
	else
		*Keys[idx]->Value = val;
}

void SHVDataRowKeyImpl::SetKeyValue(size_t idx, const SHVStringC& val)
{
	if (!Keys[idx]->Value)
		Keys[idx]->Value = new SHVDataVariantImpl(val);
	else
		*Keys[idx]->Value = val;
}

void SHVDataRowKeyImpl::SetKeyNull(size_t idx)
{
	if (Keys[idx]->Value)
		Keys[idx]->Value->SetNull();
}

/*************************************
 * AddKey
 *************************************/
SHVDataRowKey& SHVDataRowKeyImpl::AddKey(const SHVString8C& columnName, bool desc)
{
	KeyValuePair* k = new KeyValuePair();
	k->Key = columnName;
	k->Value = NULL;
	k->Desc = desc;
	Keys.Add(k);
	return *this;
}
SHVDataRowKey& SHVDataRowKeyImpl::AddKeyWithValue(const SHVString8C& columnName, SHVDataVariant* value, bool desc)
{
	KeyValuePair* k = new KeyValuePair();
	k->Key = columnName;
	k->Value = value;
	k->Desc = desc;
	Keys.Add(k);
	return *this;
}

/*************************************
 * Count
 *************************************/
size_t SHVDataRowKeyImpl::Count() const
{
	return Keys.CalculateCount();
}

/*************************************
 * Unique
 *************************************/
SHVBool SHVDataRowKeyImpl::Unique() const
{
	return UniqueKey;
}

/*************************************
 * SetUnique
 *************************************/
void SHVDataRowKeyImpl::SetUnique(SHVBool flag)
{
	UniqueKey = flag;
}

/*************************************
 * KeyEquals
 *************************************/
SHVBool SHVDataRowKeyImpl::KeyEquals(const SHVDataRowKey* key) const
{
SHVBool retVal = SHVBool::True;
const SHVDataRowKeyImpl& This = *this;
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
SHVBool SHVDataRowKeyImpl::KeyDefEquals(const SHVDataRowKey* key) const
{
SHVBool retVal = SHVBool::True;
const SHVDataRowKeyImpl& This = *this;
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
	else
		retVal = SHVBool::False;
	return retVal;
}

/*************************************
 * Destructor
 *************************************/
SHVDataRowKeyImpl::~SHVDataRowKeyImpl()
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

