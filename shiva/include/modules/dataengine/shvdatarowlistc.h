#ifndef __SHIVA_DATAENGINE_DATAROWLISTC_H
#define __SHIVA_DATAENGINE_DATAROWLISTC_H

#include "../../shvtypes.h"
#include "../../utils/shvrefobject.h"
#include "../../utils/shvstring.h"

// forward declares
class SHVDataRow;
class SHVDataRowC;
class SHVDataStructC;
class SHVDataRowList;
class SHVDataSession;
class SHVDataRowKey;
class SHVDataFactory;

//-=========================================================================================================
/// SHVDataRowList class - Interface for SHVDataRowList
/**
 * Interface for a generic shiva datarow list
 */
class SHVDataRowListC: public SHVRefObject
{
public:
	virtual const SHVDataRowC* GetCurrentRow() const = 0;
	virtual SHVBool IsOk() const = 0;
	virtual const SHVDataStructC* GetStruct() const = 0;
	virtual const SHVString8C GetAlias() const = 0;
	virtual int GetAliasID() const = 0;
	virtual int GetRowCount() const = 0;

	virtual SHVDataRowC* Find(const SHVDataRowKey* key) = 0;
	virtual SHVDataRowKey* GetPosition() = 0;
	virtual SHVBool SetPosition(const SHVDataRowKey* key) = 0;
	virtual SHVBool NextRow() = 0;
	virtual SHVDataRowListC* Reverse(const SHVStringC& condition) = 0;
	virtual SHVBool Reset() = 0;

	virtual const void* GetRowProvider() const = 0;
	virtual SHVDataSession* GetDataSession() = 0;
	virtual SHVBool RowListValid() const = 0;


protected:
// friends
friend class SHVDataSession;
friend class SHVDataRowList;

	virtual ~SHVDataRowListC() {}
	virtual void AdjustRowCount(int delta) = 0;
	virtual SHVBool InternalRowChanged(SHVDataRow* row) = 0;
	inline int GetAliasID(const SHVString8C alias) const;

// inlines
	inline SHVBool InternalRowChanged(SHVDataRowListC* list, SHVDataRow* row);


private:
	int RowCount;
};
typedef SHVRefObjectContainer<SHVDataRowListC> SHVDataRowListCRef;

#endif


// =========================================== implementation ============================================ //

#ifndef __SHIVA_DATAENGINE_DATAROWLISTC_INL
#define __SHIVA_DATAENGINE_DATAROWLISTC_INL

#include "shvdatasession.h"

/*************************************
 * InternalRowChanged
 *************************************/
SHVBool SHVDataRowListC::InternalRowChanged(SHVDataRowListC* list, SHVDataRow* row)
{
	return list->InternalRowChanged(row);
}
/*************************************
 * InternalRowChanged
 *************************************/
int SHVDataRowListC::GetAliasID(const SHVString8C alias) const
{
	return ((SHVDataRowListC*) this)->GetDataSession()->GetAliasID(alias);
}

#endif
