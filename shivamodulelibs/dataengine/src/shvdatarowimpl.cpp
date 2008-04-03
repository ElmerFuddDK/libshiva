#include "stdafx.h"

#include "../../../include/platformspc.h"
#include "../../../include/modules/dataengine/shvdatastructc.h"
#include "../../../include/modules/dataengine/shvdatasession.h"
#include "../include/shvdatarowimpl.h"
#include "../include/shvdatarowkeyimpl.h"

/*************************************
 * Constructors
 *************************************/
SHVDataRowImpl::SHVDataRowImpl(const SHVDataRowC* copyrow, SHVDataRowList* owner)
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

SHVDataRowImpl::SHVDataRowImpl(SHVDataRowList* owner)
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
 * SetString
 *************************************/
void SHVDataRowImpl::SetString(size_t colIdx, const SHVStringC& val)
{
	SHVASSERT(colIdx != SIZE_T_MAX);
	if (colIdx != SIZE_T_MAX)
	{		
		SetChanged();
		ColumnData[colIdx].Value.SetString(val);
	}
}


/*************************************
 * SetInt
 *************************************/
void SHVDataRowImpl::SetInt(size_t colIdx, SHVInt val)
{
	SHVASSERT(colIdx != SIZE_T_MAX);
	if (colIdx != SIZE_T_MAX)
	{
		SetChanged();
		ColumnData[colIdx].Value.SetInt(SHVInt(val));
	}
}


/*************************************
 * SetDouble
 *************************************/
void SHVDataRowImpl::SetDouble(size_t colIdx, SHVDouble val)
{
	SHVASSERT(colIdx != SIZE_T_MAX);
	if (colIdx != SIZE_T_MAX)
	{
		SetChanged();
		ColumnData[colIdx].Value.SetDouble(val);
	}
}


/*************************************
 * SetTime
 *************************************/
void SHVDataRowImpl::SetTime(size_t colIdx, const SHVTime& time)
{
	SHVASSERT(colIdx != SIZE_T_MAX);
	if (colIdx != SIZE_T_MAX)
	{
		SetChanged();
		ColumnData[colIdx].Value.SetTime(time);
	}
}

/*************************************
 * SetBool
 *************************************/
void SHVDataRowImpl::SetBool(size_t colIdx, SHVBool val)
{
	SHVASSERT(colIdx != SIZE_T_MAX);
	if (colIdx != SIZE_T_MAX)
	{
		SetChanged();
		ColumnData[colIdx].Value.SetBool(val);
	}
}

/*************************************
 * SetNull
 *************************************/
void SHVDataRowImpl::SetNull(size_t colIdx)
{
	SHVASSERT(colIdx != SIZE_T_MAX);
	if (colIdx != SIZE_T_MAX)
	{
		SetChanged();
		ColumnData[colIdx].Value.SetNull();
	}
}

/*************************************
 * SetCorr
 *************************************/
void SHVDataRowImpl::SetCorr(const SHVDataRowC* row)
{
size_t destCol;
	for (size_t col = 0; col < row->GetStruct()->GetColumnCount(); col++)
	{
		destCol = row->ColumnIndex((*row->GetStruct())[col]->GetColumnName());
		if (destCol != SIZE_T_MAX)
		{
		SHVDataVariantPtr val = (*row).GetValue(col);
			this->ColumnData[destCol].Value = *val;
		}
	}
}

/*************************************
 * OrginalValue
 *************************************/
const SHVDataVariant* SHVDataRowImpl::OriginalValue(size_t colIdx) const
{
	SHVASSERT(colIdx != SIZE_T_MAX);
	return colIdx != SIZE_T_MAX ? (const SHVDataVariant*) &ColumnData[colIdx].OrgValue : NULL;
}

/*************************************
 * GetRowListC
 *************************************/
SHVDataRowList* SHVDataRowImpl::GetRowList()
{
	return Owner;
}

/*************************************
 * Delete
 *************************************/
SHVBool SHVDataRowImpl::Delete()
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
SHVBool SHVDataRowImpl::HasChanges()
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
 * AsString
 *************************************/
SHVStringBuffer SHVDataRowImpl::AsString(size_t colIdx) const
{
	SHVASSERT(colIdx != SIZE_T_MAX);
	return (colIdx != SIZE_T_MAX ? ColumnData[colIdx].Value.AsString() : SHVString().ReleaseBuffer());
}

/*************************************
 * AsInt
 *************************************/
SHVInt SHVDataRowImpl::AsInt(size_t colIdx) const
{
	SHVASSERT(colIdx != SIZE_T_MAX);
	return (colIdx != SIZE_T_MAX ? ColumnData[colIdx].Value.AsInt() : SHVInt());
}

/*************************************
 * AsDouble
 *************************************/
SHVDouble SHVDataRowImpl::AsDouble(size_t colIdx) const
{
	SHVASSERT(colIdx != SIZE_T_MAX);
	return (colIdx != SIZE_T_MAX ? ColumnData[colIdx].Value.AsDouble() : SHVDouble());
}

/*************************************
 * AsTime
 *************************************/
SHVTime SHVDataRowImpl::AsTime(size_t colIdx) const
{
	SHVASSERT(colIdx != SIZE_T_MAX);
	return (colIdx != SIZE_T_MAX ? ColumnData[colIdx].Value.AsTime() : SHVTime());
}

/*************************************
 * AsBool
 *************************************/
SHVBool SHVDataRowImpl::AsBool(size_t colIdx) const
{
	SHVASSERT(colIdx != SIZE_T_MAX);
	return (colIdx != SIZE_T_MAX ? ColumnData[colIdx].Value.AsBool() : SHVBool());
}

/*************************************
 * IsNull
 *************************************/
SHVBool SHVDataRowImpl::IsNull(size_t colIdx) const
{
	SHVASSERT(colIdx != SIZE_T_MAX);
	if (colIdx != SIZE_T_MAX )
		return SHVBool(ColumnData[colIdx].Value.IsNull());
	else
		return SHVBool::True;
}

/*************************************
 * ColumnIndex
 *************************************/
size_t SHVDataRowImpl::ColumnIndex(const SHVString8C& columnName) const
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
 * GetKey
 *************************************/
SHVDataRowKey* SHVDataRowImpl::GetKey(size_t index) const
{
SHVDataStructCRef st = (SHVDataStructC*) Owner->GetStruct();
const SHVDataRowKey* idx = st->GetIndex(index);
SHVDataRowKeyImpl* retVal = NULL;
	if (idx)
	{
		retVal = new SHVDataRowKeyImpl();
		for (size_t i = 0; i < idx->Count(); i++)
		{
			retVal->AddKey((*idx)[i].Key, GetValue(ColumnIndex((*idx)[i].Key)), (*idx)[i].Desc);
		}
	}
	return retVal;
}

/*************************************
 * GetValue
 *************************************/
SHVDataVariant* SHVDataRowImpl::GetValue(size_t colIdx) const
{
	SHVASSERT(colIdx != SIZE_T_MAX);
	return (colIdx != SIZE_T_MAX ? new SHVDataVariantImpl(ColumnData[colIdx].Value) : new SHVDataVariantImpl());
}

/*************************************
 * MatchKey
 *************************************/
SHVBool SHVDataRowImpl::MatchKey(const SHVDataRowKey* key) const 
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
SHVBool SHVDataRowImpl::RowValid() const
{
	return GetRowState() != SHVDataRow::RowStateInvalid &&
		GetRowState() != SHVDataRow::RowStateDeleted;
}

/*************************************
 * GetStruct
 *************************************/
const SHVDataStructC* SHVDataRowImpl::GetStruct() const
{
	if (RowValid())
		return Owner->GetStruct();
	else
		return NULL;
}

/*************************************
 * GetAlias
 *************************************/
const SHVString8C SHVDataRowImpl::GetAlias() const
{
	if (RowValid())
		return Owner->GetAlias();
	else
		return "";
}

/*************************************
 * GetRowState
 *************************************/
int SHVDataRowImpl::GetRowState() const
{
	return RowState;
}

/*************************************
 * GetRowListC
 *************************************/
const SHVDataRowListC* SHVDataRowImpl::GetRowListC()
{
	return GetRowList();
}

/*************************************
 * Destructor
 *************************************/
SHVDataRowImpl::~SHVDataRowImpl()
{
	delete [] ColumnData;
}

/*************************************
 * InternalAcceptChanges
 *************************************/
void SHVDataRowImpl::InternalAcceptChanges()
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
void SHVDataRowImpl::InternalRejectChanges()
{
	RowState = SHVDataRow::RowStateInvalid;
}

