#ifndef __SHIVA_DATAENGINE_DATAROW_H
#define __SHIVA_DATAENGINE_DATAROW_H

#include "..\..\..\include\utils\shvrefobject.h"
#include "..\..\..\include\utils\shvstring.h"
#include "..\..\..\include\utils\shvvectorref.h"
#include "..\..\..\include\shvtypes.h"
#include "shvdatarowc.h"

//-=========================================================================================================
/// SHVDataRow class - Interface for SHVDataRow
/**
 * Interface for a generic shiva datarow
 */

class SHVDataRow: public SHVDataRowC
{
public:
	enum SHVDataRowState
	{
		SHVDataRowState_Unattached,
		SHVDataRowState_Unchanged,
		SHVDataRowState_Deleted,
		SHVDataRowState_Changed,
		SHVDataRowState_New
	};
	virtual SHVStringBuffer AsString(const SHVString8C& colName) const = 0;
	virtual void SetValue(const SHVString8C& colName, const SHVStringC& val) = 0;

	virtual SHVInt AsInt(const SHVString8C& colName) const = 0;
	virtual void SetValue(const SHVString8C& colName, SHVInt val) = 0;

	virtual SHVDouble AsDouble(const SHVString8C& colName) const = 0;
	virtual void SetValue(const SHVString8C& colName, SHVDouble val) = 0;

	virtual SHVTime AsTime(const SHVString8C& colName) const = 0;
	virtual void SetValue(const SHVString8C& colName, const SHVTime& time) = 0;

	virtual SHVBool AsBool(const SHVString8C& colName) const = 0;
	virtual void SetValue(const SHVString8C& colName, SHVBool val) = 0;

	virtual SHVBool IsNull(const SHVString8C& colName) const = 0;
	virtual void SetNull(const SHVString8C& colName) = 0;

	virtual SHVDataVariant* GetValue(const SHVString8C& colName) const = 0;
	virtual SHVDataVariant* GetValue(size_t colIdx) const = 0;
	virtual const SHVDataVariant* OrginalValue(const SHVString8C& colName) const = 0;

	virtual SHVDataRowKey* GetKey(size_t index = 0) const = 0;
	virtual SHVBool MatchKey(const SHVDataRowKey* key) const = 0;
	virtual SHVBool RowValid() const  = 0; 
	virtual int GetRowState() const = 0;

	virtual SHVBool Delete() = 0;
	virtual SHVBool AcceptChanges() = 0;
	virtual SHVBool RejectChanges() = 0;
	virtual SHVBool HasChanges() = 0;
	virtual void ClearOwnership() = 0;

protected:
	virtual ~SHVDataRow() { }
};

typedef SHVRefObjectContainer<SHVDataRow> SHVDataRowRef;
typedef SHVVectorRef<SHVDataRow> SHVDataRowVector;


#endif