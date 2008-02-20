#include "stdafx.h"

#include "../../../../include/platformspc.h"
#include "../../include/dataengineimpl/shvdatarow_impl.h"
#include "../../include/dataengineimpl/shvdatarowkey_impl.h"
#include "../../include/shvdatastructc.h"

SHVDataRow_impl::SHVDataRow_impl(SHVDataRowC* copyrow, SHVDataRowList* owner)
{
SHVDataStructCRef st = (SHVDataStructC*) owner->GetStruct();
ColumnData = new KeyValuePair[st->GetColumnCount()];
	for (size_t i = 0; i < st->GetColumnCount(); i++)
	{
		ColumnData[i].Name = (*st)[i]->GetColumnName();
		switch ((*st)[i]->GetDataType())
		{
		case SHVDataVariant::SHVDataType_Int:
			ColumnData[i].Value.SetInt(copyrow->AsInt(ColumnData[i].Name));
			break;
		case SHVDataVariant::SHVDataType_Bool:
			ColumnData[i].Value.SetBool(copyrow->AsBool(ColumnData[i].Name));
			break;
		case SHVDataVariant::SHVDataType_Double:
			ColumnData[i].Value.SetDouble(copyrow->AsDouble(ColumnData[i].Name));
			break;
		case SHVDataVariant::SHVDataType_String:
			ColumnData[i].Value.SetString(copyrow->AsString(ColumnData[i].Name));
			break;
		case SHVDataVariant::SHVDataType_Time:
			ColumnData[i].Value.SetTime(copyrow->AsTime(ColumnData[i].Name));
			break;
		}
	}
	RowState = SHVDataRow::SHVDataRowState_Unchanged;
}

SHVDataRow_impl::SHVDataRow_impl(SHVDataRowList* owner)
{
SHVDataStructCRef st = (SHVDataStructC*) owner->GetStruct();
ColumnData = new KeyValuePair[st->GetColumnCount()];
	for (size_t i = 0; i < st->GetColumnCount(); i++)
	{
		switch ((*st)[i]->GetDataType())
		{
		case SHVDataVariant::SHVDataType_Int:
			ColumnData[i].Value.SetInt(SHVInt());
			break;
		case SHVDataVariant::SHVDataType_Double:
			ColumnData[i].Value.SetDouble(SHVDouble());
			break;
		case SHVDataVariant::SHVDataType_Bool:
			ColumnData[i].Value.SetBool(SHVBool::False);
			break;
		case SHVDataVariant::SHVDataType_Time:
			ColumnData[i].Value.SetTime(SHVTime());
			break;
		case SHVDataVariant::SHVDataType_String:
			ColumnData[i].Value.SetString(SHVString());
			break;
		}
	}
	RowState = SHVDataRow::SHVDataRowState_Unchanged;
}

SHVStringBuffer SHVDataRow_impl::AsString(const SHVString8C& colName) const
{
KeyValuePair* vp =	Find(colName);
	return (vp ? vp->Value.AsString() : SHVString().ReleaseBuffer());
}

void SHVDataRow_impl::SetValue(const SHVString8C& colName, const SHVStringC& val)
{
KeyValuePair* vp =	Find(colName);
	if (vp)
	{		
		RowState = (RowState != SHVDataRow::SHVDataRowState_New ? 
			SHVDataRow::SHVDataRowState_Changed : SHVDataRow::SHVDataRowState_New);
		vp->Value.SetString(val);
	}
}

SHVInt SHVDataRow_impl::AsInt(const SHVString8C& colName) const
{
KeyValuePair* vp =	Find(colName);
	return (vp ? vp->Value.AsInt() : SHVInt());
}

void SHVDataRow_impl::SetValue(const SHVString8C& colName, SHVInt val)
{
KeyValuePair* vp =	Find(colName);
	if (vp)
	{
		RowState = (RowState != SHVDataRow::SHVDataRowState_New ? 
			SHVDataRow::SHVDataRowState_Changed : SHVDataRow::SHVDataRowState_New);
		vp->Value.SetInt(SHVInt(val));
	}
}

SHVDouble SHVDataRow_impl::AsDouble(const SHVString8C& colName) const
{
KeyValuePair* vp =	Find(colName);
	return (vp ? vp->Value.AsDouble() : SHVDouble());
}

void SHVDataRow_impl::SetValue(const SHVString8C& colName, SHVDouble val)
{
KeyValuePair* vp =	Find(colName);
	if (vp)
	{
		RowState = (RowState != SHVDataRow::SHVDataRowState_New ? 
			SHVDataRow::SHVDataRowState_Changed : SHVDataRow::SHVDataRowState_New);
		vp->Value.SetDouble(val);
	}
}

SHVTime SHVDataRow_impl::AsTime(const SHVString8C& colName) const
{
KeyValuePair* vp =	Find(colName);
	return (vp ? vp->Value.AsTime() : SHVTime());
}

void SHVDataRow_impl::SetValue(const SHVString8C& colName, const SHVTime& time)
{
KeyValuePair* vp =	Find(colName);
	if (vp)
	{
		RowState = (RowState != SHVDataRow::SHVDataRowState_New ? 
			SHVDataRow::SHVDataRowState_Changed : SHVDataRow::SHVDataRowState_New);
		vp->Value.SetTime(time);
	}
}

SHVBool SHVDataRow_impl::AsBool(const SHVString8C& colName) const
{
KeyValuePair* vp =	Find(colName);
	return (vp ? vp->Value.AsBool() : SHVBool());
}

void SHVDataRow_impl::SetValue(const SHVString8C& colName, SHVBool val)
{
KeyValuePair* vp =	Find(colName);
	if (vp)
	{
		RowState = (RowState != SHVDataRow::SHVDataRowState_New ? 
			SHVDataRow::SHVDataRowState_Changed : SHVDataRow::SHVDataRowState_New);
		vp->Value.SetBool(val);
	}
}

SHVBool SHVDataRow_impl::IsNull(const SHVString8C& colName) const
{
KeyValuePair* vp =	Find(colName);
	if (vp)
		return SHVBool(vp->Value.IsNull());
	else
		return SHVBool::True;
}

void SHVDataRow_impl::SetNull(const SHVString8C& colName)
{
KeyValuePair* vp =	Find(colName);
	SHVASSERT(vp);
	if (vp)
	{
		RowState = (RowState != SHVDataRow::SHVDataRowState_New ? 
			SHVDataRow::SHVDataRowState_Changed : SHVDataRow::SHVDataRowState_New);
		vp->Value.SetNull();
	}
}

SHVDataVariant* SHVDataRow_impl::GetValue(size_t colIdx) const
{
	return new SHVDataVariant_impl(ColumnData[colIdx].Value);
}

SHVDataVariant* SHVDataRow_impl::GetValue(const SHVString8C& colName) const
{
KeyValuePair* vp = Find(colName);
	return vp ? new SHVDataVariant_impl(&vp->Value) : NULL;
}

const SHVDataVariant* SHVDataRow_impl::OrginalValue(const SHVString8C& colName) const
{
KeyValuePair* vp = Find(colName);
	return vp ? &vp->OrgValue : NULL;
}

SHVDataRowKey* SHVDataRow_impl::GetKey(size_t index) const
{
SHVDataStructCRef st = (SHVDataStructC*) Owner->GetStruct();
SHVDataRowKeyRef idx = (SHVDataRowKey *) st->GetIndex(index);
SHVDataRowKey_impl* retVal = NULL;
	if (!idx.IsNull())
	{
		retVal = new SHVDataRowKey_impl;
		for (size_t i = 0; i < idx->Count(); i++)
		{
			retVal->AddKey((*idx)[i].Key, new SHVDataVariant_impl((*idx)[i].Value), (*idx)[i].Desc);
		}
	}
	return retVal;
}

SHVBool SHVDataRow_impl::MatchKey(const SHVDataRowKey* key) const 
{
SHVBool retVal = true;
	for (size_t i = 0; i < key->Count() && retVal; i++)
	{
	KeyValuePair* found = Find((*key)[i].Key);
		if (found)
			retVal = *(*key)[i].Value == found->Value;
		else
			retVal = false;
	}
	return retVal;
}

SHVBool SHVDataRow_impl::RowValid() const
{
	return GetRowState() != SHVDataRow::SHVDataRowState_Unattached &&
		GetRowState() != SHVDataRow::SHVDataRowState_Deleted;
}

int SHVDataRow_impl::GetRowState() const
{
	return RowState;
}

SHVBool SHVDataRow_impl::Delete()
{
	SHVASSERT(RowValid());

SHVBool retVal(RowValid());

	if (RowValid())
		RowState = SHVDataRow::SHVDataRowState_Deleted;
		
	return retVal;
}

SHVBool SHVDataRow_impl::AcceptChanges()
{
	return Owner->AcceptChanges(this);
}

SHVBool SHVDataRow_impl::RejectChanges()
{
	return Owner->RejectChanges(this);
}

SHVBool SHVDataRow_impl::HasChanges()
{
SHVDataStructCRef st = (SHVDataStructC *) Owner->GetStruct();
bool hasChanges = false;
	for (size_t i = 0; i < st->GetColumnCount() && !hasChanges; i++)
	{
		hasChanges = ColumnData[i].OrgValue.GetDataType() != SHVDataVariant::SHVDataType_Undefined;
	}
	if (!hasChanges && RowState != SHVDataRow::SHVDataRowState_New)
		RowState = SHVDataRow::SHVDataRowState_Unchanged;

	return hasChanges;
}
void SHVDataRow_impl::ClearOwnership()
{
	RowState = SHVDataRow::SHVDataRowState_Unattached;
	Owner = NULL;
}

SHVDataRow_impl::~SHVDataRow_impl()
{
	delete [] ColumnData;
}

SHVDataRow_impl::KeyValuePair* SHVDataRow_impl::Find(const SHVString8C& columnName) const
{
KeyValuePair* retVal = NULL;

	SHVASSERT(RowState != SHVDataRow::SHVDataRowState_Unattached);

	if (RowState != SHVDataRow::SHVDataRowState_Unattached)
	{
	SHVDataStructCRef st = (SHVDataStructC *) Owner->GetStruct();
		for (size_t i = 0; i < st->GetColumnCount() && !retVal; i++)
		{
			if ((*st)[i]->GetColumnName() == columnName)
				retVal = &ColumnData[i];
		}
	}
	return retVal;
}
