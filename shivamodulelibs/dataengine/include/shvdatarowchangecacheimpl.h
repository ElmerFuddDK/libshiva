#ifndef __SHIVA_DATAENGINE_CHANGE_CACHE_IMPL_H
#define __SHIVA_DATAENGINE_CHANGE_CACHE_IMPL_H

#include "../../../include/modules/dataengine/shvdatarowchangecache.h"
#include "../../../include/utils/shvdynarray.h"
#include "../../../include/utils/shvptrcontainer.h"

class SHVDataRowChangeCacheImpl: public SHVDataRowChangeCache
{
public:

	struct CacheItem
	{
	public:
		inline CacheItem(SHVDataRowKey* key, int rowState, SHVDataRow* row);
		SHVDataRowKeyRef Key;
		SHVDataRowRef Row;
		int RowState;
	};

	SHVDataRowChangeCacheImpl();
	virtual ~SHVDataRowChangeCacheImpl();
	void AddItem(SHVDataRow* row, bool keepRow);
	void RemoveItem(const SHVDataRowC* row);
	void Clear();
	virtual const size_t GetRowCount() const;
	virtual const SHVDataRowKey* GetKey(size_t rowNo) const;
	virtual const SHVDataRowC* GetRow(size_t rowNo) const;
	virtual const int GetRowState(size_t rowNo) const;
private:
	SHVDynArray<CacheItem> ChangedItems;
};
typedef SHVPtrContainer<SHVDataRowChangeCacheImpl> SHVDataRowChangeCacheImplPtr;

// ============================================ implementation ============================================ //

/*************************************
 * CacheItem::Constructor
 *************************************/
SHVDataRowChangeCacheImpl::CacheItem::CacheItem(SHVDataRowKey* key, int rowState, SHVDataRow* row):
	Key(key), RowState(rowState), Row(row)
{
}

#endif
