#include "stdafx.h"

#include "../../../../include/platformspc.h"
#include "../../../../include/modules/dataengine/shvdatastructc.h"
#include "../../../../include/modules/dataengine/shvdatasession.h"
#include "../../include/shvdatarowimpl.h"
#include "../../include/shvdatarowkeyimpl.h"

/*************************************
 * Constructors
 *************************************/
SHVDataRow_impl::SHVDataRow_impl(const SHVDataRowC* copyrow, SHVDataRowList* owner)
{
SHVDataStructCRef st = (SHVDataStructC*) owner->GetStruct();
ColumnData = new RowValues[st->GetColumnCount()];
	Owner = owner;
	for (size_t i = 0; i < st->GetColumnCount(); i++)
	{
		switch ((*st)[i]->GetDataType())
		{
		case SHVDataVariant::TypeInt:
			ColumnData[i].OrgValue.SetInt(copyrow->AsInt(i));
			ColumnData[i].Value.SetInt(copyrow->AsInt(i));
			break;
		case SHVDataVariant::TypeBool:
			ColumnData[i].OrgValue.SetBool(copyrow->AsBool(i));
			ColumnData[i].Value.SetBool(copyrow->AsBool(i));
			break;
		case SHVDataVariant::TypeDouble:
			ColumnData[i].OrgValue.SetDouble(copyrow->AsDouble(i));
			ColumnData[i].Value.SetDouble(copyrow->AsDouble(i));
			break;
		case SHVDataVariant::TypeString:
			ColumnData[i].OrgValue.SetString(copyrow->AsString(i));
			ColumnData[i].Value.SetString(copyrow->AsString(i));
			break;
		case SHVDataVariant::TypeTime:
			ColumnData[i].OrgValue.SetTime(copyrow->AsTime(i));
			ColumnData[i].Value.SetTime(copyrow->AsTime(i));
			break;
		}
	}
	RowState = SHVDataRow::RowStateUnchanged;
}

SHVDataRow_impl::SHVDataRow_impl(SHVDataRowList* owner)
{
SHVDataStructCRef st = (SHVDataStructC*) owner->GetStruct();
ColumnData = new RowValues[st->GetColumnCount()];
	Owner = owner;
	for (size_t i = 0; i < st->GetColumnCount(); i++)
	{
		switch ((*st)[i]->GetDataType())
		{
		case SHVDataVariant::TypeInt:
			ColumnData[i].OrgValue.SetInt(SHVInt());
			ColumnData[i].Value.SetInt(SHVInt());
			break;
		case SHVDataVariant::TypeDouble:
			ColumnData[i].OrgValue.SetDouble(SHVDouble());
			ColumnData[i].Value.SetDouble(SHVDouble());
			break;
		case SHVDataVariant::TypeBool:
			ColumnData[i].OrgValue.SetBool(SHVBool::False);
			ColumnData[i].Value.SetBool(SHVBool::False);
			break;
		case SHVDataVariant::TypeTime:
			ColumnData[i].OrgValue.SetTime(SHVTime());
			ColumnData[i].Value.SetTime(SHVTime());
			break;
		case SHVDataVariant::TypeString:
			ColumnData[i].OrgValue.SetString(SHVString());
			ColumnData[i].Value.SetString(SHVString());
			break;
		}
	}
	RowState = SHVDataRow::RowStateAdding;
}

/*************************************
 * AsString
 *************************************/
SHVStringBuffer SHVDataRow_impl::AsString(size_t colIdx) const
{
	SHVASSERT(colIdx != SIZE_T_MAX);
	return (colIdx != SIZE_T_MAX ? ColumnData[colIdx].Value.AsString() : SHVString().ReleaseBuffer());
}

/*************************************
 * SetString
 *************************************/
void SHVDataRow_impl::SetString(size_t colIdx, const SHVStringC& val)
{
	SHVASSERT(colIdx != SIZE_T_MAX);
	if (colIdx != SIZE_T_MAX)
	{		
		SetChanged();
		ColumnData[colIdx].Value.SetString(val);
	}
}

/*************************************
 * AsInt
 *************************************/
SHVInt SHVDataRow_impl::AsInt(size_t colIdx) const
{
	SHVASSERT(colIdx != SIZE_T_MAX);
	return (colIdx != SIZE_T_MAX ? ColumnData[colIdx].Value.AsInt() : SHVInt());
}

/*************************************
 * SetInt
 *************************************/
void SHVDataRow_impl::SetInt(size_t colIdx, SHVInt val)
{
	SHVASSERT(colIdx != SIZE_T_MAX);
	if (colIdx != SIZE_T_MAX)
	{
		SetChanged();
		ColumnData[colIdx].Value.SetInt(SHVInt(val));
	}
}

/*************************************
 * AsDouble
 *************************************/
SHVDouble SHVDataRow_impl::AsDouble(size_t colIdx) const
{
	SHVASSERT(colIdx != SIZE_T_MAX);
	return (colIdx != SIZE_T_MAX ? ColumnData[colIdx].Value.AsDouble() : SHVDouble());
}

/*************************************
 * SetDouble
 *************************************/
void SHVDataRow_impl::SetDouble(size_t colIdx, SHVDouble val)
{
	SHVASSERT(colIdx != SIZE_T_MAX);
	if (colIdx != SIZE_T_MAX)
	{
		SetChanged();
		ColumnData[colIdx].Value.SetDouble(val);
	}
}

/*************************************
 * AsTime
 *************************************/
SHVTime SHVDataRow_impl::AsTime(size_t colIdx) const
{
	SHVASSERT(colIdx != SIZE_T_MAX);
	return (colIdx != SIZE_T_MAX ? ColumnData[colIdx].Value.AsTime() : SHVTime());
}

/*************************************
 * SetTime
 *************************************/
void SHVDataRow_impl::SetTime(size_t colIdx, const SHVTime& time)
{
	SHVASSERT(colIdx != SIZE_T_MAX);
	if (colIdx != SIZE_T_MAX)
	{
		SetChanged();
		ColumnData[colIdx].Value.SetTime(time);
	}
}

/*************************************
 * AsBool
 *************************************/
SHVBool SHVDataRow_impl::AsBool(size_t colIdx) const
{
	SHVASSERT(colIdx != SIZE_T_MAX);
	return (colIdx != SIZE_T_MAX ? ColumnData[colIdx].Value.AsBool() : SHVBool());
}

/*************************************
 * AsBool
 *************************************/
void SHVDataRow_impl::SetBool(size_t colIdx, SHVBool val)
{
	SHVASSERT(colIdx != SIZE_T_MAX);
	if (colIdx != SIZE_T_MAX)
	{
		SetChanged();
		ColumnData[colIdx].Value.SetBool(val);
	}
}

/*************************************
 * IsNull
 *************************************/
SHVBool SHVDataRow_impl::IsNull(size_t colIdx) const
{
	SHVASSERT(colIdx != SIZE_T_MAX);
	if (colIdx != SIZE_T_MAX )
		return SHVBool(ColumnData[colIdx].Value.IsNull());
	else
		return SHVBool::True;
}

/*************************************
 * SetNull
 *************************************/
void SHVDataRow_impl::SetNull(size_t colIdx)
{
	SHVASSERT(colIdx != SIZE_T_MAX);
	if (colIdx != SIZE_T_MAX)
	{
		SetChanged();
		ColumnData[colIdx].Value.SetNull();
	}
}

/*************************************
 * ColumnIndex
 *************************************/
size_t SHVDataRow_impl::ColumnIndex(const SHVString8C& columnName) const
{
size_t retVal;
	SHVASSERT(RowValid());
	if (RowValid())
	{
		if (!Owner->GetStruct()->FindColumnIndex(retVal, columnName))
			retVal = SIZE_T_MAX;
	}
	return retVal;
}

/*************************************
 * GetValue
 *************************************/
SHVDataVariant* SHVDataRow_impl::GetValue(size_t colIdx) const
{
	SHVASSERT(colIdx != SIZE_T_MAX);
	return (colIdx != SIZE_T_MAX ? new SHVDataVariant_impl(ColumnData[colIdx].Value) : new SHVDataVariant_impl());
}

/*************************************
 * OrginalValue
 *************************************/
const SHVDataVariant* SHVDataRow_impl::OriginalValue(size_t colIdx) const
{
	SHVASSERT(colIdx != SIZE_T_MAX);
	return colIdx != SIZE_T_MAX ? (const SHVDataVariant*) &ColumnData[colIdx].OrgValue : NULL;
}

/*************************************
 * GetKey
 *************************************/
SHVDataRowKey* SHVDataRow_impl::GetKey(size_t index) const
{
SHVDataStructCRef st = (SHVDataStructC*) Owner->GetStruct();
const SHVDataRowKey* idx = st->GetIndex(index);
SHVDataRowKey_impl* retVal = NULL;
	if (idx)
	{
		retVal = new SHVDataRowKey_impl;
		for (size_t i = 0; i < idx->Count(); i++)
		{
			retVal->AddKey((*idx)[i].Key, GetValue(ColumnIndex((*idx)[i].Key)), (*idx)[i].Desc);
		}
	}
	return retVal;
}

/*************************************
 * MatchKey
 *************************************/
SHVBool SHVDataRow_impl::MatchKey(const SHVDataRowKey* key) const 
{
SHVBool retVal = true;
	for (size_t i = 0; i < key->Count() && retVal; i++)
	{
	size_t colIdx = ColumnIndex((*key)[i].Key);
		if (colIdx != SIZE_T_MAX)
			retVal = *(*key)[i].Value == ColumnData[colIdx].Value;
		else
			retVal = false;
	}
	return retVal;
}

/*************************************
 * RowValid
 *************************************/
SHVBool SHVDataRow_impl::RowValid() const
{
	return GetRowState() != SHVDataRow::RowStateInvalid &&
		GetRowState() != SHVDataRow::RowStateDeleted;
}

/*************************************
 * GetStruct
 *************************************/
const SHVDataStructC* SHVDataRow_impl::GetStruct() const
{
	if (RowValid())
		return Owner->GetStruct();
	else
		return NULL;
}

/*************************************
 * GetAlias
 *************************************/
const SHVString8C SHVDataRow_impl::GetAlias() const
{
	if (RowValid())
		return Owner->GetAlias();
	else
		return "";
}

/*************************************
 * GetRowState
 *************************************/
int SHVDataRow_impl::GetRowState() const
{
	return RowState;
}

/*************************************
 * Delete
 *************************************/
SHVBool SHVDataRow_impl::Delete()
{
	SHVASSERT(RowValid());

SHVBool retVal(RowValid());

	if (RowValid())
		RowState = SHVDataRow::RowStateDeleting;
		
	return retVal;
}

/*************************************
 * HasChanges
 *************************************/
SHVBool SHVDataRow_impl::HasChanges()
{
SHVDataStructCRef st = (SHVDataStructC *) Owner->GetStruct();
bool hasChanges = false;
	for (size_t i = 0; i < st->GetColumnCount() && !hasChanges; i++)
	{
		hasChanges = ColumnData[i].OrgValue.GetDataType() != SHVDataVariant::TypeUndefined;
	}
	if (!hasChanges && RowState != SHVDataRow::RowStateAdding)
		RowState = SHVDataRow::RowStateUnchanged;

	return hasChanges;
}

/*************************************
 * InternalAcceptChanges
 *************************************/
void SHVDataRow_impl::InternalAcceptChanges()
{
	if (RowState == SHVDataRow::RowStateDeleting)
		RowState = SHVDataRow::RowStateDeleted;
	if (RowState ==SHVDataRow::RowStateAdding)
		RowState = SHVDataRow::RowStateAdded;
	if (RowState ==SHVDataRow::RowStateChanging)
		RowState = SHVDataRow::RowStateChanged;	
}

/*************************************
 * InternalRejectChanges
 *************************************/
void SHVDataRow_impl::InternalRejectChanges()
{
	RowState = SHVDataRow::RowStateInvalid;
}

/*************************************
 * GetRowListC
 *************************************/
const SHVDataRowListC* SHVDataRow_impl::GetRowListC()
{
	return GetRowList();
}

/*************************************
 * GetRowListC
 *************************************/
SHVDataRowList* SHVDataRow_impl::GetRowList()
{
	return Owner;
}

/*************************************
 * Destructor
 *************************************/
SHVDataRow_impl::~SHVDataRow_impl()
{
	delete [] ColumnData;
}

